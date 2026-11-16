#include "search.hpp"
#include <cctype>
 
namespace {
 
std::string toLowerCase(const std::string& text) {
    std::string result = text;
 
    for (char& letter : result) {
        letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
    }
 
    return result;
}
 
bool contains(const std::string& text, const std::string& lowerQuery) {
    return toLowerCase(text).find(lowerQuery) != std::string::npos;
}
 
}
 
std::vector<std::size_t> findSongs(const std::vector<Song>& songs, const std::string& query) {
    std::vector<std::size_t> matches;
    std::string lowerQuery = toLowerCase(query);
 
    if (lowerQuery.empty()) {
        return matches;
    }
 
    for (std::size_t index = 0; index < songs.size(); index++) {
        const Song& song = songs[index];
 
        if (contains(song.title, lowerQuery) ||
            contains(song.artist, lowerQuery) ||
            contains(song.album, lowerQuery)) {
            matches.push_back(index);
        }
    }
 
    return matches;
}