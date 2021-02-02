cmake_minimum_required(VERSION 3.13)

project(WinIMerge)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(FreeImage REQUIRED)
find_package(FreeImagePlus REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(gtkmm-3.0 REQUIRED gtkmm-3.0 IMPORTED_TARGET)

#add_library(WinIMerge)

#target_sources(
#    WinIMerge
#    PRIVATE
#        src/image.hpp
#        src/ImgConverter.hpp
#        src/ImgDiffBuffer.hpp
#        src/ImgMergeBuffer.hpp
#        src/ImgMergeWindow.hpp
#        src/ImgToolWindow.hpp
#        src/ImgWindow.hpp
#        src/resource.h
#        src/Win78Libraries.cpp
#        src/Win78Libraries.h
#        src/WinIMerge.cpp
#        src/WinIMerge.h
#        src/WinIMergeLib.cpp
#        src/WinIMergeLib.h
#)
#
#target_link_libraries(
#    WinIMerge
#    PRIVATE
#        FreeImagePlus)

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
        FreeImage
        FreeImagePlus
        PkgConfig::gtkmm-3.0
        stdc++fs)

set_property(TARGET cmdidiff PROPERTY CXX_STANDARD 17)
set_property(TARGET cmdidiff PROPERTY CXX_EXTENSIONS OFF)
