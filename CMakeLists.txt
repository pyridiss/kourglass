cmake_minimum_required(VERSION 2.8)

project (timetrak)

find_package(KDE4 REQUIRED)

include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

set(timetrak_SRCS main.cpp)
kde4_add_executable(timetrak ${timetrak_SRCS})

target_link_libraries(timetrak ${KDE4_KDEUI_LIBS})

set_target_properties(timetrak PROPERTIES COMPILE_FLAGS "-std=c++11 -g -Wall")

install(TARGETS timetrak ${INSTALL_TARGETS_DEFAULT_ARGS})