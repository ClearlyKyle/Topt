cmake_minimum_required(VERSION 3.10)

project(Topt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)

add_library(topt INTERFACE topt.h) # interface for header only library
target_include_directories(topt INTERFACE ${CMAKE_SOURCE_DIR}) # path to folder with src (not the actual source)

add_executable(Asteroids asteroids.cpp)
target_link_libraries(Asteroids PRIVATE topt SDL2::SDL2main SDL2::SDL2-static)

add_executable(Raycaster raycaster.cpp)
target_link_libraries(Raycaster PRIVATE topt SDL2::SDL2main SDL2::SDL2-static)