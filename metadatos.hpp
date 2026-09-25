#pragma once
#include <vector>
#include <filesystem>
#include "cancion.hpp"
 
std::vector<Song> escanearCarpeta(const std::filesystem::path& carpeta);
