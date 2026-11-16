cmake_minimum_required(VERSION 3.16)
project(lector_musica CXX)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
find_package(Threads REQUIRED)
 
add_executable(lector
    main.cpp
    metadata.cpp
    sorter.cpp
    search.cpp
    playlist.cpp
    audio.cpp
)
target_include_directories(lector PRIVATE ${TAGLIB_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_directories(lector PRIVATE ${TAGLIB_LIBRARY_DIRS} ${SDL2_MIXER_LIBRARY_DIRS})
target_compile_options(lector PRIVATE ${TAGLIB_CFLAGS_OTHER} ${SDL2_MIXER_CFLAGS_OTHER})
target_link_libraries(lector PRIVATE ${TAGLIB_LIBRARIES} SDL2_mixer SDL2 Threads::Threads)
