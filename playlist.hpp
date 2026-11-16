#pragma once
#include <string>
#include <vector>
#include "song.hpp"

bool savePlaylistM3u8(const std::vector<Song>& songs, const std::string& filename);

bool loadPlaylistM3u8(const std::vector<Song>& library, const std::string& filename,
                      std::vector<Song>& loaded, int& missing);
