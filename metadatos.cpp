#include "metadatos.hpp"
 
#include <algorithm>
#include <cctype>
 
#include <taglib/fileref.h>
#include <taglib/tag.h>
 
namespace fs = std::filesystem;
 
namespace {
 
bool FormatoSoportado(const fs::path& p) {
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                    [](unsigned char c) { return std::tolower(c); });
    return ext == ".flac" || ext == ".opus";
}
 
Song leerMetadatos(const fs::path& archivo) {
    Song s;
    s.path = archivo.string();
 
#ifdef _WIN32
    std::wstring rutaW = archivo.wstring();
    TagLib::FileRef f(rutaW.c_str());
#else
    std::string rutaStr = archivo.string();
    TagLib::FileRef f(rutaStr.c_str());
#endif

    if (f.isNull() || !f.tag()) {
        s.title = archivo.stem().string(); // respaldo si no hay tags
        return s;
    }
 
    TagLib::Tag* tag = f.tag();
    s.title  = tag->title().isEmpty() ? archivo.stem().string()
                                       : tag->title().to8Bit(true);
    s.artist = tag->artist().to8Bit(true);
    s.album  = tag->album().to8Bit(true);
    s.year   = tag->year();
    s.track  = tag->track();
 
    if (f.audioProperties()) {
        s.duration = f.audioProperties()->lengthInSeconds();
    }
 
    return s;
}
 
} 
 
std::vector<Song> escanearCarpeta(const fs::path& carpeta) {
    std::vector<Song> canciones;
    for (const auto& entry : fs::recursive_directory_iterator(carpeta)) {
        if (entry.is_regular_file() && esFormatoSoportado(entry.path())) {
            canciones.push_back(leerMetadatos(entry.path()));
        }
    }
    return canciones;
}