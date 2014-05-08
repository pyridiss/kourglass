cmake_minimum_required(VERSION 2.8)

project (timetrak)

find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})

add_subdirectory(src)