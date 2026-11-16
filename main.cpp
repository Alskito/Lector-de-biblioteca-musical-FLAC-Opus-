#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <future>
#include <string>
#include <thread>

#include "song.hpp"
#include "metadata.hpp"
#include "sorter.hpp"
#include "playbackQueue.hpp"
#include "search.hpp"
#include "playlist.hpp"
#include "audio.hpp"
 
#ifdef _WIN32
#include <windows.h>
#endif
 
void clearScreen() {
    std::cout << std::flush; 
#ifdef _WIN32
    int result = std::system("cls");
#else
    int result = std::system("clear");
#endif
    (void)result;
}
 

void waitForEnter() {
    std::cout << "\nPress enter to continue...";
    std::string ignored;
    std::getline(std::cin, ignored);
}
 
void printSongs(const std::vector<Song>& songs) {
    for (const auto& song : songs) {
        std::cout << song.title << " — " << song.artist
                  << " [" << song.album << "]"
                  << (song.year ? " (" + std::to_string(song.year) + ")" : "")
                  << '\n';
    }
}
 
int numberFromInput(const std::string& input); 

bool pickSong(const std::vector<Song>& library, Song& chosen, std::string& notice) {
    std::cout << "\nSearch by title, artist or album (enter to cancel): ";
    std::string query;
    if (!std::getline(std::cin, query) || query.empty()) {
        return false;
    }

    std::vector<std::size_t> matches = findSongs(library, query);
    if (matches.empty()) {
        notice = "No songs found for '" + query + "'.";
        return false;
    }

    for (std::size_t index = 0; index < matches.size(); index++) {
        const Song& song = library[matches[index]];
        std::cout << "  " << index + 1 << ". " << song.title << " — " << song.artist
                  << " [" << song.album << "]\n";
    }

    std::cout << "Number of the song (enter to cancel): ";
    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        return false;
    }

    int number = numberFromInput(input);
    if (number < 1 || number > static_cast<int>(matches.size())) {
        notice = "Invalid number '" + input + "'.";
        return false;
    }

    chosen = library[matches[number - 1]];
    return true;
}

enum class PlayerState { Stopped, Playing, Paused };

std::string stateName(PlayerState state) {
    switch (state) {
        case PlayerState::Playing: return "Playing";
        case PlayerState::Paused: return "Paused";
        default: return "Stopped";
    }
}

std::string formatDuration(unsigned int seconds) {
    std::string rest = std::to_string(seconds % 60);
    if (rest.size() < 2) {
        rest = "0" + rest;
    }
    return std::to_string(seconds / 60) + ":" + rest;
}

enum class Action { None, PlayPause, Stop, Next, Previous, Remove, AddAfter, AddBefore, Exit };


Action actionFor(std::string command) {
    std::size_t first = command.find_first_not_of(" \t\r");
    std::size_t last = command.find_last_not_of(" \t\r");
    command = (first == std::string::npos) ? "" : command.substr(first, last - first + 1);

    for (char& letter : command) {
        letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
    }

    if (command == "p" || command == "play" || command == "pause") return Action::PlayPause;
    if (command == "s" || command == "stop") return Action::Stop;
    if (command == "n" || command == "next") return Action::Next;
    if (command == "b" || command == "back" || command == "prev" || command == "previous") return Action::Previous;
    if (command == "r" || command == "remove") return Action::Remove;
    if (command == "a" || command == "add") return Action::AddAfter;
    if (command == "i" || command == "insert") return Action::AddBefore;
    if (command == "e" || command == "exit" || command == "q" || command == "quit") return Action::Exit;
    return Action::None;
}

bool startSong(AudioPlayer& audio, const std::string& musicFolder, const Song& song, std::string& notice) {
    std::filesystem::path file = std::filesystem::path(musicFolder) / std::filesystem::u8path(song.path);
    if (!audio.play(file.u8string())) {
        notice = "Could not play '" + song.title + "': " + audio.message();
        return false;
    }
    return true;
}

void playQueue(const std::vector<Song>& songs, const std::vector<Song>& library,
               const std::string& musicFolder, AudioPlayer& audio) {
    PlaybackQueue queue;
    queue.load(songs);

    PlayerState state = PlayerState::Stopped;
    std::string command;
    std::string notice;
    std::future<bool> input; 
    bool reading = false;    
    bool redraw = true;

    while (!queue.empty()) {
        if (redraw) {
            clearScreen();
            if (!notice.empty()) {
                std::cout << notice << "\n\n";
                notice.clear();
            }
            const Song& song = queue.current();
            std::cout << stateName(state) << ": " << song.title << " — " << song.artist
                      << "  (" << formatDuration(song.duration) << ")  "
                      << queue.position() << " of " << queue.length()
                      << ", " << queue.length() - queue.position() << " left\n"
                      << "[p] play/pause  [s] stop  [n] next  [b] back  [a] add after  [i] insert before  [r] remove  [e] exit\n> "
                      << std::flush;
            redraw = false;
        }

        if (!reading) {
            input = std::async(std::launch::async, [&command]() {
                return static_cast<bool>(std::getline(std::cin, command));
            });
            reading = true;
        }

        if (input.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
            if (state == PlayerState::Playing && audio.finished()) {
                bool hasNext = queue.next();
                if (!hasNext) {
                    notice = "End of the queue.";
                }
                if (!hasNext || !startSong(audio, musicFolder, queue.current(), notice)) {
                    audio.stop();
                    state = PlayerState::Stopped;
                }
                redraw = true;
            }
            continue;
        }

        reading = false;
        if (!input.get()) {
            break;
        }

        Action action = actionFor(command);
        if (action == Action::Exit) {
            break;
        }

        if (action == Action::PlayPause) {
            if (!audio.available()) {
                notice = "There is no audio: " + audio.message();
            } else if (state == PlayerState::Stopped) {
                if (startSong(audio, musicFolder, queue.current(), notice)) {
                    state = PlayerState::Playing;
                }
            } else if (state == PlayerState::Playing) {
                audio.pause();
                state = PlayerState::Paused;
            } else {
                audio.resume();
                state = PlayerState::Playing;
            }
        } else if (action == Action::Stop) {
            audio.stop();
            state = PlayerState::Stopped;
        } else if (action == Action::Next || action == Action::Previous || action == Action::Remove) {
            bool changed = true;
            if (action == Action::Next) {
                changed = queue.next();
                if (!changed) {
                    notice = "This is the last song in the queue.";
                }
            } else if (action == Action::Previous) {
                changed = queue.previous();
                if (!changed) {
                    notice = "This is the first song in the queue.";
                }
            } else {
                queue.removeCurrent();
            }

            if (changed) {
                if (state == PlayerState::Playing && !queue.empty()) {
                    if (!startSong(audio, musicFolder, queue.current(), notice)) {
                        state = PlayerState::Stopped;
                    }
                } else {
                    audio.stop();
                    state = PlayerState::Stopped;
                }
            }
        } else if (action == Action::AddAfter || action == Action::AddBefore) {
            Song chosen;
            if (pickSong(library, chosen, notice)) {
                if (action == Action::AddAfter) {
                    queue.addAfter(chosen);
                    notice = "Added '" + chosen.title + "' after the current song.";
                } else {
                    queue.addBefore(chosen);
                    notice = "Added '" + chosen.title + "' before the current song.";
                }
            }
        } else {
            notice = "Unknown command '" + command + "'.";
        }
        redraw = true;
    }

    audio.stop();
}

void searchSongs(const std::vector<Song>& songs) {
    clearScreen();
    std::string query;

    while (true) {
        std::cout << "\nSearch by title, artist or album (enter to go back to the menu): ";
        if (!std::getline(std::cin, query) || query.empty()) {
            break;
        }

        std::vector<std::size_t> matches = findSongs(songs, query);
        if (matches.empty()) {
            std::cout << "No songs found for '" << query << "'.\n";
            continue;
        }

        std::cout << "Found " << matches.size() << " song(s):\n";
        for (std::size_t index : matches) {
            const Song& song = songs[index];
            std::cout << "  " << song.title << " — " << song.artist
                      << " [" << song.album << "]\n";
        }
    }
}

void warnAndWait(const std::string& message) {
    std::cerr << message << " Sorting by title...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
 
SortCriterion criterionFromNumber(int number) {
    switch (number) {
        case 1: return SortCriterion::Title;
        case 2: return SortCriterion::Artist;
        case 3: return SortCriterion::Album;
        case 4: return SortCriterion::Year;
        case 5: return SortCriterion::Track;
        default:
            warnAndWait("Unknown option '" + std::to_string(number) + "'.");
            return SortCriterion::Title;
    }
}
 
std::string criterionName(SortCriterion criterion) {
    switch (criterion) {
        case SortCriterion::Title:  return "title";
        case SortCriterion::Artist: return "artist";
        case SortCriterion::Album:  return "album";
        case SortCriterion::Year:   return "year";
        case SortCriterion::Track:  return "track";
    }
    return "";
}
 
SortCriterion criterionFromInput(const std::string& input) {
    try {
        return criterionFromNumber(std::stoi(input));
    } catch (...) {
        warnAndWait("Invalid input '" + input + "'.");
        return SortCriterion::Title;
    }
}
 
SortCriterion askCriterion() {
    std::cout << "\nWhich attribute do you want to sort by?\n"
                 "  1. Title\n"
                 "  2. Artist\n"
                 "  3. Album\n"
                 "  4. Year\n"
                 "  5. Track\n"
                 "Option: ";
    std::string input;
    std::getline(std::cin, input);
    return criterionFromInput(input);
}
 
bool containsSong(const std::vector<Song>& playlist, const Song& song) {
    for (const Song& entry : playlist) {
        if (entry.path == song.path) {
            return true;
        }
    }
    return false;
}
 
int numberFromInput(const std::string& input) {
    try {
        return std::stoi(input);
    } catch (...) {
        return -1;
    }
}
 
void printPlaylist(const std::vector<Song>& playlist) {
    if (playlist.empty()) {
        std::cout << "The playlist is empty.\n";
        return;
    }
 
    for (std::size_t i = 0; i < playlist.size(); i++) {
        std::cout << "  " << i + 1 << ". " << playlist[i].title << " — " << playlist[i].artist
                  << " [" << playlist[i].album << "]\n";
    }
}
 
void addSongs(const std::vector<Song>& songs, std::vector<Song>& playlist) {
    clearScreen();
    std::string query;
    std::cout << "Search by title, artist or album (enter to cancel): ";
    if (!std::getline(std::cin, query) || query.empty()) {
        return;
    }
 
    std::vector<std::size_t> matches = findSongs(songs, query);
    if (matches.empty()) {
        std::cout << "No songs found for '" << query << "'.\n";
        waitForEnter();
        return;
    }
 
    for (std::size_t i = 0; i < matches.size(); i++) {
        const Song& song = songs[matches[i]];
        std::cout << "  " << i + 1 << ". " << song.title << " — " << song.artist
                  << " [" << song.album << "]\n";
    }
 
    std::string input;
    while (true) {
        std::cout << "Number of the song to add (enter to finish): ";
        if (!std::getline(std::cin, input) || input.empty()) {
            break;
        }
 
        int number = numberFromInput(input);
        if (number < 1 || number > static_cast<int>(matches.size())) {
            std::cout << "Invalid number '" << input << "'.\n";
            continue;
        }
 
        const Song& song = songs[matches[number - 1]];
        if (containsSong(playlist, song)) {
            std::cout << "'" << song.title << "' is already in the playlist.\n";
        } else {
            playlist.push_back(song);
            std::cout << "Added '" << song.title << "'.\n";
        }
    }
}
 
void removeSong(std::vector<Song>& playlist) {
    if (playlist.empty()) {
        std::cout << "The playlist is empty.\n";
        waitForEnter();
        return;
    }
 
    clearScreen();
    printPlaylist(playlist);
    std::cout << "Number of the song to remove (enter to cancel): ";
    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        return;
    }
 
    int number = numberFromInput(input);
    if (number < 1 || number > static_cast<int>(playlist.size())) {
        std::cout << "Invalid number '" << input << "'.\n";
        waitForEnter();
        return;
    }
 
    std::cout << "Removed '" << playlist[number - 1].title << "'.\n";
    playlist.erase(playlist.begin() + (number - 1));
}
 
void savePlaylist(const std::vector<Song>& playlist, const std::string& musicFolder) {
    if (playlist.empty()) {
        std::cout << "The playlist is empty, add some songs first.\n";
        waitForEnter();
        return;
    }
 
    std::cout << "Name your playlist (enter to cancel): ";
    std::string name;
    if (!std::getline(std::cin, name) || name.empty()) {
        return;
    }
    name += ".m3u8";
 
    std::string path = (std::filesystem::path(musicFolder) / name).string();
 
    if (savePlaylistM3u8(playlist, path)) {
        std::cout << "Saved " << playlist.size() << " song(s) to '" << path << "'.\n";
    } else {
        std::cout << "Could not write '" << path << "'.\n";
    }
    waitForEnter();
}

void openPlaylist(const std::vector<Song>& songs, std::vector<Song>& playlist, const std::string& musicFolder) {
    clearScreen();

    std::vector<std::filesystem::path> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(musicFolder)) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(),
                           [](unsigned char letter) { return std::tolower(letter); });
            if (entry.is_regular_file() && (extension == ".m3u8" || extension == ".m3u")) {
                files.push_back(entry.path());
            }
        }
    } catch (const std::exception& problem) {
        std::cout << "Could not read the folder: " << problem.what() << "\n";
        waitForEnter();
        return;
    }

    if (files.empty()) {
        std::cout << "There are no .m3u8 playlists in '" << musicFolder << "'.\n";
        waitForEnter();
        return;
    }
    std::sort(files.begin(), files.end());

    for (std::size_t index = 0; index < files.size(); index++) {
        std::cout << "  " << index + 1 << ". " << files[index].filename().u8string() << "\n";
    }
    std::cout << "Number of the playlist to open (enter to cancel): ";
    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        return;
    }

    int number = numberFromInput(input);
    if (number < 1 || number > static_cast<int>(files.size())) {
        std::cout << "Invalid number '" << input << "'.\n";
        waitForEnter();
        return;
    }

    if (!playlist.empty()) {
        std::cout << "This replaces the current playlist. Continue? (y/n): ";
        std::getline(std::cin, input);
        if (input != "y" && input != "Y") {
            return;
        }
    }

    std::vector<Song> loaded;
    int missing = 0;
    const std::filesystem::path& file = files[number - 1];
    if (!loadPlaylistM3u8(songs, file.string(), loaded, missing)) {
        std::cout << "Could not read '" << file.filename().u8string() << "'.\n";
        waitForEnter();
        return;
    }

    playlist = std::move(loaded);
    std::cout << "Opened '" << file.filename().u8string() << "': " << playlist.size() << " song(s).\n";
    if (missing > 0) {
        std::cout << missing << " line(s) did not match any song of the library and were skipped.\n";
    }
    waitForEnter();
}

void playlistMenu(const std::vector<Song>& songs, std::vector<Song>& playlist, const std::string& musicFolder, AudioPlayer& audio) {
    std::string input;
    while (true) {
        clearScreen();
        std::cout << "=== Playlist (" << playlist.size() << " song(s)) ===\n"
                     "  1. Add songs\n"
                     "  2. Show playlist\n"
                     "  3. Remove a song\n"
                     "  4. Save as .m3u8\n"
                     "  5. Open a saved playlist\n"
                     "  6. Play playlist\n"
                     "  7. Back to main menu\n"
                     "Option: ";
 
        if (!std::getline(std::cin, input) || input == "7") {
            break;
        }
 
        if (input == "1") {
            addSongs(songs, playlist);
        } else if (input == "2") {
            clearScreen();
            printPlaylist(playlist);
            waitForEnter();
        } else if (input == "3") {
            removeSong(playlist);
        } else if (input == "4") {
            savePlaylist(playlist, musicFolder);
        } else if (input == "5") {
            openPlaylist(songs, playlist, musicFolder);
        } else if (input == "6") {
            if (playlist.empty()) {
                std::cout << "The playlist is empty.\n";
                waitForEnter();
            } else {
                playQueue(playlist, songs, musicFolder, audio);
            }
        } else {
            std::cout << "Unknown option '" << input << "'.\n";
            waitForEnter();
        }
    }
}
 
int main(int argc, char** argv) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif
 
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <music_folder>\n";
        return 1;
    }
 
    std::string musicFolder = argv[1];
 
    std::error_code error;
    if (!std::filesystem::is_directory(musicFolder, error)) {
        std::cerr << "'" << musicFolder << "' is not a folder.\n";
        return 1;
    }
 
    std::vector<Song> songs;
    try {
        songs = scanFolder(musicFolder);
    } catch (const std::exception& problem) {
        std::cerr << "Could not read the folder: " << problem.what() << '\n';
        return 1;
    }
    std::cout << "Found " << songs.size() << " songs.\n";

    AudioPlayer audio;
    if (!audio.start()) {
        std::cerr << "Audio is not available (" << audio.message()
                  << "). You can browse the library, but nothing will play.\n";
        waitForEnter();
    } else if (!audio.message().empty()) {
        std::cerr << "Warning: " << audio.message() << "\n";
        waitForEnter();
    }
 
    SortCriterion criterion = askCriterion();
    mergeSort(songs, comparatorFor(criterion));
 
    std::vector<Song> playlist;
    std::string input;
    while (true) {
        clearScreen();
        std::cout << "=== Menu (" << songs.size() << " songs) ===\n"
                     "  1. Show library (sorted by " << criterionName(criterion) << ")\n"
                     "  2. Change sort criterion\n"
                     "  3. Search a song\n"
                     "  4. Play queue\n"
                     "  5. Create playlist (.m3u8)\n"
                     "  6. Exit\n"
                     "Option: ";
 
        if (!std::getline(std::cin, input)) {
            break;
        }
 
        if (input == "1") {
            clearScreen();
            std::cout << "--- Sorted by " << criterionName(criterion) << " ---\n";
            printSongs(songs);
            waitForEnter();
        } else if (input == "2") {
            criterion = askCriterion();
            mergeSort(songs, comparatorFor(criterion));
        } else if (input == "3") {
            searchSongs(songs);
        } else if (input == "4") {
            playQueue(songs, songs, musicFolder, audio);
        } else if (input == "5") {
            playlistMenu(songs, playlist, musicFolder, audio);
        } else if (input == "6") {
            break;
        } else {
            std::cout << "Unknown option '" << input << "'.\n";
            waitForEnter();
        }
    }
 
    return 0;
}
