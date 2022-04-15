cmake_minimum_required(VERSION 3.21)

project(YH-Master-Thesis VERSION 1.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/json/)

add_executable(YH-Master-Thesis
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Timetable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TimetableConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Individual.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Solver.cpp)

# 列车牵引计算
add_executable(TrainTractionCalculation
        src/TrainTractionCalculation.cpp
        src/TractionCalculator.cpp)


