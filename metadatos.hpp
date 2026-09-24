#pragma once
#include <vector>
#include <filesystem>
#include "cancion.hpp"
 
// Recorre una carpeta (y subcarpetas) y devuelve todas las canciones
// .flac / .opus encontradas, ya con sus metadatos leídos.
std::vector<Song> escanearCarpeta(const std::filesystem::path& carpeta);