cmake_minimum_required(VERSION 3.21)

project(YH-Master-Thesis VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/matplotlib-cpp/)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/json/)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/matplotlib-cpp/)

add_library(Station_static_lib
        STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Station.cpp)
target_include_directories(Station_static_lib
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_library(Interval_static_lib STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Interval.cpp)
target_include_directories(Interval_static_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_library(Mission_static_lib STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Mission.cpp)
target_include_directories(Mission_static_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_library(Timetable_static_lib STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Timetable.cpp)
target_include_directories(Timetable_static_lib PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/json/
        )

add_library(TimetableConfig_static_lib STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/TimetableConfig.cpp)
target_include_directories(TimetableConfig_static_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)


add_library(GA_static_lib STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/GA.cpp)
target_include_directories(GA_static_lib PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/include/)


add_library(Individual_static_lib STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Individual.cpp)
target_include_directories(Individual_static_lib PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/include/)

#add_library(TractionCalculator_static_lib STATIC src/TractionCalculator.cpp)
#target_include_directories(TractionCalculator_static_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)



add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/matplotlib-cpp/)

add_executable(YH-Master-Thesis ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(YH-Master-Thesis PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/matplotlib-cpp/
        )

target_link_libraries(
        YH-Master-Thesis
        matplotlib_cpp
        Station_static_lib
        Interval_static_lib
        Mission_static_lib
        Timetable_static_lib
        TimetableConfig_static_lib
        GA_static_lib
        Individual_static_lib
#        TractionCalculator_static_lib
)


# 列车牵引计算
add_library(TractionCalculator_static_lib STATIC src/TractionCalculator.cpp)
target_include_directories(TractionCalculator_static_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_executable(TrainTractionCalculation src/TrainTractionCalculation.cpp)
target_include_directories(TrainTractionCalculation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_link_libraries(TrainTractionCalculation
        TractionCalculator_static_lib
        )


