#include "sorter.hpp"
#include <algorithm>

namespace {
std::string aMinusculas(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return r;
    }
}

std::function<bool(const Song&, const Song&)> comparadorPara(CriterioOrden criterio) {
    switch (criterio) {
        case CriterioOrden::Titulo:
            return [](const Song& a, const Song& b) { return aMinusculas(a.title) < aMinusculas(b.title); };
        case CriterioOrden::Artista:
            return [](const Song& a, const Song& b) { return aMinusculas(a.artist) < aMinusculas(b.artist); };
        case CriterioOrden::Album:
            return [](const Song& a, const Song& b) { return aMinusculas(a.album) < aMinusculas(b.album); };
        case CriterioOrden::Anio:
            return [](const Song& a, const Song& b) { return a.year < b.year; };
        case CriterioOrden::Pista:
        default:
            return [](const Song& a, const Song& b) { return a.track < b.track; };
    }
}
 
void insertionSort(std::vector<Song>& canciones,
     const std::function<bool(const Song&, const Song&)>& comparador) {
    for (std::size_t i = 1; i < canciones.size(); ++i) {
        for (long j = static_cast<long>(i);
             j > 0 && comparador(canciones[j], canciones[j - 1]);
             --j) {
            std::swap(canciones[j], canciones[j - 1]);
        }
    }
}
 