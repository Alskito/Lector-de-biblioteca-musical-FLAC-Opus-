#include "playlist.hpp"
#include <algorithm>
#include <fstream>

bool savePlaylistM3u8(const std::vector<Song>& songs, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }

    file << "#EXTM3U\n";
    for (const Song& song : songs) {
        file << "./" << song.path << "\n"; 
    }

    return file.good();
}

bool loadPlaylistM3u8(const std::vector<Song>& library, const std::string& filename,
                      std::vector<Song>& loaded, int& missing) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }

    loaded.clear();
    missing = 0;

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("\xEF\xBB\xBF", 0) == 0) {
            line.erase(0, 3); 
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#') {
            continue; 
        }

        std::replace(line.begin(), line.end(), '\\', '/'); 
        if (line.rfind("./", 0) == 0) {
            line.erase(0, 2);
        }

        bool found = false;
        for (const Song& song : library) {
            if (song.path == line) {
                loaded.push_back(song);
                found = true;
                break;
            }
        }
        if (!found) {
            missing++;
        }
    }

    return true;
}
