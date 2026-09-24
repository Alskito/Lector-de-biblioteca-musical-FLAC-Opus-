#pragma once
#include <vector>
#include <functional>
#include "cancion.hpp"
 
enum class CriterioOrden { Titulo, Artista, Album, Anio, Pista };
 
// Devuelve el comparador (a < b) correspondiente al criterio elegido.
std::function<bool(const Song&, const Song&)> comparadorPara(CriterioOrden criterio);
 
// Ordena 'canciones' in-place usando insertion sort, según 'comparador'.
// La implementación está en sorter.cpp — ahí va tu algoritmo.
void insertionSort(std::vector<Song>& canciones,
    const std::function<bool(const Song&, const Song&)>& comparador);