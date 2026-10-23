#include "metadata.hpp"
 
#include <algorithm>
#include <cctype>
 
#include <taglib/fileref.h>
#include <taglib/tag.h>
 
namespace fs = std::filesystem;
 
namespace {
 
bool isSupportedFormat(const fs::path& file) {
    std::string ext = file.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char letter) { return std::tolower(letter); });
    return ext == ".flac" || ext == ".opus";
}
 
Song readMetadata(const fs::path& file) {
    Song song;
    song.path = file.string();
 
#ifdef _WIN32
    std::wstring widePath = file.wstring();
    TagLib::FileRef fileRef(widePath.c_str());
#else
    std::string narrowPath = file.string();
    TagLib::FileRef fileRef(narrowPath.c_str());
#endif
 
    if (fileRef.isNull() || !fileRef.tag()) {
        song.title = file.stem().string(); // fallback if there are no tags
        return song;
    }
 
    TagLib::Tag* tag = fileRef.tag();
    song.title  = tag->title().isEmpty() ? file.stem().string()
                                         : tag->title().to8Bit(true);
    song.artist = tag->artist().to8Bit(true);
    song.album  = tag->album().to8Bit(true);
    song.year   = tag->year();
    song.track  = tag->track();
 
    if (fileRef.audioProperties()) {
        song.duration = fileRef.audioProperties()->lengthInSeconds();
    }
 
    return song;
}
 
}
 
std::vector<Song> scanFolder(const fs::path& folder) {
    std::vector<Song> songs;
    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (entry.is_regular_file() && isSupportedFormat(entry.path())) {
            songs.push_back(readMetadata(entry.path()));
        }
    }
    return songs;
}
