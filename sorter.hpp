#pragma once
#include <vector>
#include <functional>
#include "cancion.hpp"
 
enum class CriterioOrden { Titulo, Artista, Album, Anio, Pista };
 
std::function<bool(const Song&, const Song&)> comparadorPara(CriterioOrden criterio);
 
void insertionSort(std::vector<Song>& canciones,
    const std::function<bool(const Song&, const Song&)>& comparador);
