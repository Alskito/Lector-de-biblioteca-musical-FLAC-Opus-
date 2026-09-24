#include <iostream>
#include <chrono>
#include <thread>

#include "cancion.hpp"
#include "metadatos.hpp"
#include "sorter.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

void imprimir(const std::vector<Song>& canciones) {
    for (const auto& s : canciones) {
        std::cout << s.title << " — " << s.artist
                  << " [" << s.album << "]"
                  << (s.year ? " (" + std::to_string(s.year) + ")" : "")
                  << '\n';
    }
}

void avisarYEsperar(const std::string& mensaje) {
    std::cerr << mensaje << " Ordenando por título...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

CriterioOrden criterioDesdeNumero(int numero) {
    switch (numero) {
        case 1: return CriterioOrden::Titulo;
        case 2: return CriterioOrden::Artista;
        case 3: return CriterioOrden::Album;
        case 4: return CriterioOrden::Anio;
        case 5: return CriterioOrden::Pista;
        default:
            avisarYEsperar("Opción desconocida '" + std::to_string(numero) + "'.");
            return CriterioOrden::Titulo;
    }
}

std::string nombreCriterio(CriterioOrden criterio) {
    switch (criterio) {
        case CriterioOrden::Titulo:  return "título";
        case CriterioOrden::Artista: return "artista";
        case CriterioOrden::Album:   return "álbum";
        case CriterioOrden::Anio:    return "año";
        case CriterioOrden::Pista:   return "pista";
    }
    return "";
}

CriterioOrden criterioDesdeEntrada(const std::string& entrada) {
    try {
        return criterioDesdeNumero(std::stoi(entrada));
    } catch (...) {
        avisarYEsperar("Entrada inválida '" + entrada + "'.");
        return CriterioOrden::Titulo;
    }
}

CriterioOrden pedirCriterio() {
    std::cout << "\n¿Por qué atributo quieres ordenar?\n"
                 "  1. Título\n"
                 "  2. Artista\n"
                 "  3. Álbum\n"
                 "  4. Año\n"
                 "  5. Pista\n"
                 "Opción: ";
    std::string entrada;
    std::getline(std::cin, entrada);
    return criterioDesdeEntrada(entrada);
}

int main(int argc, char** argv) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <carpeta_de_musica> [1-5]\n";
        return 1;
    }

    auto canciones = escanearCarpeta(argv[1]);
    std::cout << "Se encontraron " << canciones.size() << " canciones.\n";

    CriterioOrden criterio = (argc >= 3) ? criterioDesdeEntrada(argv[2]) : pedirCriterio();
    insertionSort(canciones, comparadorPara(criterio));

    std::cout << "\n--- Ordenadas por " << nombreCriterio(criterio) << " ---\n";
    imprimir(canciones);

    return 0;
}