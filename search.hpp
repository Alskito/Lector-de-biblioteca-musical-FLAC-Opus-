#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "song.hpp"
 
std::vector<std::size_t> findSongs(const std::vector<Song>& songs, const std::string& query);
