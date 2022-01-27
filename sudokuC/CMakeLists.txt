cmake_minimum_required(VERSION 3.15)
project(sudokuC)

set(CMAKE_CXX_STANDARD 17)

add_executable(sudokuC main.cpp sudokuSolver.cpp NodeHandler.cpp)