#include "sorter.hpp"
#include <cctype>
#include <string>
#include <utility>

namespace {

using Comparador = std::function<bool(const Song&, const Song&)>;

std::string aMinusculas(const std::string& texto) {
    std::string resultado = texto;
 
    for (char& letra : resultado) {
        letra = static_cast<char>(std::tolower(static_cast<unsigned char>(letra)));
    }
 
    return resultado;
}
 
void mezclar(std::vector<Song>& canciones,
             std::vector<Song>& auxiliar,
             std::size_t inicio,
             std::size_t medio,
             std::size_t fin,
             const Comparador& esMenor) {
 
    std::size_t izquierda = inicio;   
    std::size_t derecha   = medio;    
    std::size_t destino   = inicio;  
 
    
    while (izquierda < medio && derecha < fin) {
        if (esMenor(canciones[derecha], canciones[izquierda])) {
            auxiliar[destino] = std::move(canciones[derecha]);
            derecha++;
        } else {
            auxiliar[destino] = std::move(canciones[izquierda]);
            izquierda++;
        }
        destino++;
    }
 
    while (izquierda < medio) {
        auxiliar[destino] = std::move(canciones[izquierda]);
        izquierda++;
        destino++;
    }
 
    while (derecha < fin) {
        auxiliar[destino] = std::move(canciones[derecha]);
        derecha++;
        destino++;
    }
 
    for (std::size_t i = inicio; i < fin; i++) {
        canciones[i] = std::move(auxiliar[i]);
    }
}
 
void ordenarRango(std::vector<Song>& canciones,
                  std::vector<Song>& auxiliar,
                  std::size_t inicio,
                  std::size_t fin,
                  const Comparador& esMenor) {
 
    if (fin - inicio < 2) {
        return;
    }
 
    std::size_t medio = inicio + (fin - inicio) / 2;
 
    ordenarRango(canciones, auxiliar, inicio, medio, esMenor);  
    ordenarRango(canciones, auxiliar, medio, fin, esMenor);    
    mezclar(canciones, auxiliar, inicio, medio, fin, esMenor);  
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
 
void mergeSort(std::vector<Song>& canciones, const Comparador& comparador) {
    std::vector<Song> auxiliar(canciones.size());
 
    ordenarRango(canciones, auxiliar, 0, canciones.size(), comparador);
}
 
