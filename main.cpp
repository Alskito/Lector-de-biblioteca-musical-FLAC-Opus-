#include <iostream>
#include <chrono>
#include <thread>
 
#include "song.hpp"
#include "metadata.hpp"
#include "sorter.hpp"
#include "playbackQueue.hpp"
 
#ifdef _WIN32
#include <windows.h>
#endif
 
void printSongs(const std::vector<Song>& songs) {
    for (const auto& song : songs) {
        std::cout << song.title << " — " << song.artist
                  << " [" << song.album << "]"
                  << (song.year ? " (" + std::to_string(song.year) + ")" : "")
                  << '\n';
    }
}
 
void playQueue(const std::vector<Song>& songs) {
    PlaybackQueue queue;
    queue.load(songs);
 
    std::string command;
    while (!queue.empty()) {
        const Song& song = queue.current();
        std::cout << "\nNow playing: " << song.title << " — " << song.artist
                  << "  (" << queue.position() << " of " << queue.length()
                  << ", " << queue.length() - queue.position() << " left)\n"
                  << "[n] next  [p] previous  [r] remove from queue  [e] exit\n> ";
 
        if (!std::getline(std::cin, command) || command == "e") {
            break;
        }
 
        if (command == "n") {
            if (!queue.next()) {
                std::cout << "This is the last song in the queue.\n";
            }
        } else if (command == "p") {
            if (!queue.previous()) {
                std::cout << "This is the first song in the queue.\n";
            }
        } else if (command == "r") {
            queue.removeCurrent();
        } else {
            std::cout << "Unknown command '" << command << "'.\n";
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
 
int main(int argc, char** argv) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif
 
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <music_folder> [1-5]\n";
        return 1;
    }
 
    auto songs = scanFolder(argv[1]);
    std::cout << "Found " << songs.size() << " songs.\n";
 
    SortCriterion criterion = (argc >= 3) ? criterionFromInput(argv[2]) : askCriterion();
    mergeSort(songs, comparatorFor(criterion));
 
    std::cout << "\n--- Sorted by " << criterionName(criterion) << " ---\n";
    printSongs(songs);
 
    playQueue(songs);
 
    return 0;
}
