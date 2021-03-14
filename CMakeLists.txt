cmake_minimum_required(VERSION 3.13)

project(WinIMerge)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(FreeImage REQUIRED)
find_package(FreeImagePlus REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(gtkmm-3.0 REQUIRED gtkmm-3.0 IMPORTED_TARGET)

add_library(WinIMerge)

target_sources(
    WinIMerge
    PRIVATE
        src/image.hpp
        src/ImgConverter.hpp
        src/ImgDiffBuffer.hpp
#        src/ImgMergeBuffer.hpp
#        src/ImgMergeWindow.hpp
#        src/ImgToolWindow.hpp
        src/ImgWindow.hpp
#        src/Ocr.hpp
#        src/resource.h
#        src/Win78Libraries.cpp
#        src/Win78Libraries.h
#        src/WinIMerge.cpp
#        src/WinIMerge.h
#        src/WinIMergeLib.cpp
#        src/WinIMergeLib.h
)

target_link_libraries(
    WinIMerge
    PUBLIC
        FreeImage
        FreeImagePlus
        PkgConfig::gtkmm-3.0
        stdc++fs)

target_include_directories(
    WinIMerge
    PUBLIC
        src)

set_property(TARGET WinIMerge PROPERTY LINKER_LANGUAGE CXX)
set_property(TARGET WinIMerge PROPERTY CXX_STANDARD 17)
set_property(TARGET WinIMerge PROPERTY CXX_EXTENSIONS OFF)

add_executable(cmdidiff)

target_sources(
    cmdidiff
    PRIVATE
        src/image.hpp
        src/ImgConverter.hpp
        src/ImgDiffBuffer.hpp
        src/cidiff.cpp)

target_link_libraries(
    cmdidiff
    PRIVATE
        WinIMerge)

add_subdirectory(examples)

set_property(TARGET cmdidiff PROPERTY CXX_STANDARD 17)
set_property(TARGET cmdidiff PROPERTY CXX_EXTENSIONS OFF)
