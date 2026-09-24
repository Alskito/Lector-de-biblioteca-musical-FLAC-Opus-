cmake_minimum_required(VERSION 3.16)
project(lector_musica CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)

add_executable(lector
    main.cpp
    metadatos.cpp
    sorter.cpp
)
target_include_directories(lector PRIVATE ${TAGLIB_INCLUDE_DIRS})
target_link_libraries(lector PRIVATE ${TAGLIB_LIBRARIES})
target_compile_options(lector PRIVATE ${TAGLIB_CFLAGS_OTHER})
