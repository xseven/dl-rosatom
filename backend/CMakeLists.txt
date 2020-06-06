cmake_minimum_required(VERSION 3.15.0)

project(backend)

find_package(Qt5 COMPONENTS 
Core 
WebSockets
REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(backend
    main.cpp
	backend.cpp
)

target_link_libraries(backend 
Qt5::Core
Qt5::WebSockets
)