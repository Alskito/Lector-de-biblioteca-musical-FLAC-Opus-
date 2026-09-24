#pragma once
#include <string>
 
struct Song {
    std::string path;
    std::string title;
    std::string artist;
    std::string album;
    unsigned int year = 0;
    unsigned int track = 0;
    unsigned int duration = 0; // segundos
};
 