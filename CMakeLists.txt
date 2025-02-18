cmake_minimum_required(VERSION 3.27)
project(Satellite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

add_executable(Satellite
        src/main.cpp
        src/satellite.cpp src/satellite.h
)

target_link_libraries(Satellite Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
