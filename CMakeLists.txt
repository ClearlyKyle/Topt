cmake_minimum_required(VERSION 3.10)

project(Topt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-gfx CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)

add_library(topt INTERFACE topt.h) # interface for header only library
target_include_directories(topt INTERFACE ${CMAKE_SOURCE_DIR}) # path to folder with src (not the actual source)

add_executable(Asteroids src/Asteroids/Asteroids.cpp)
target_link_libraries(Asteroids PRIVATE topt SDL2::SDL2main SDL2::SDL2-static)

add_executable(Raycaster src/Raycaster/Raycaster.cpp)
target_link_libraries(Raycaster PRIVATE topt SDL2::SDL2main SDL2::SDL2-static)

add_executable(TicTacToe src/TicTacToe/TicTacToe.cpp)
target_link_libraries(TicTacToe PRIVATE topt SDL2::SDL2main SDL2::SDL2_gfx SDL2::SDL2-static)

add_executable(15 src/15/15.cpp)
target_link_libraries(15 PRIVATE topt SDL2::SDL2main SDL2::SDL2_ttf SDL2::SDL2-static)