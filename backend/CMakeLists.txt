cmake_minimum_required(VERSION 3.15.0)

project(backend)

find_package(Boost 1.70.0 REQUIRED)

find_package(Qt5 COMPONENTS 
Core 
WebSockets
REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(THRIFT_SRC
${CMAKE_BINARY_DIR}/generated-thrift/protocol_constants.cpp
${CMAKE_BINARY_DIR}/generated-thrift/protocol_types.cpp
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/generated-thrift/protocol_constants.cpp ${CMAKE_BINARY_DIR}/generated-thrift/protocol_types.cpp
    PRE_BUILD    
    #COMMAND if not exist generated-thrift mkdir generated-thrift
    COMMAND ${CMAKE_COMMAND} -E make_directory generated-thrift
    COMMAND ${CMAKE_SOURCE_DIR}/../ipc-protocol/thrift-0.13.0.exe -r -out ${CMAKE_BINARY_DIR}/generated-thrift -gen cpp:moveable_types ${CMAKE_SOURCE_DIR}/../ipc-protocol/protocol.thrift	
)

add_executable(backend
    main.cpp
	backend.cpp
	${THRIFT_SRC}
)

target_include_directories(backend PRIVATE
${CMAKE_SOURCE_DIR}/3rdparty
${Boost_INCLUDE_DIRS}
)

target_link_libraries(backend PRIVATE
Qt5::Core
Qt5::WebSockets
${CMAKE_SOURCE_DIR}/3rdparty/thriftmd$<$<CONFIG:Debug>:d>.lib
)

