#pragma once
#include <vector>
#include <filesystem>
#include "song.hpp"
 
std::vector<Song> scanFolder(const std::filesystem::path& folder);
 