cmake_minimum_required(VERSION 3.6)
project(ValiantUC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE String
            "Choose the type how to build, options are: None, Release, Debug, RelWithDebInfo, MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cmake-build-debug)
endif(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)

file(GLOB GAMMA_SOURCES src/Gamma/gamma/*.cpp src/Gamma/util/*.cpp)
file(GLOB GAMMA_HEADERS src/Gamma/gamma/*.h src/Gamma/util/*.h src/config.h)
add_library(GAMMA_LIB ${GAMMA_SOURCES} ${GAMMA_HEADERS} src/UC.cpp src/Gamma/util/hybrid_size.h src/Gamma/util/hybrid_size.cpp)

file(GLOB UC4_SOURCE_FILES src/uc/4way/*.cpp src/uc/4way/*.h src/config.h)

file(GLOB UC2_SOURCE_FILES src/uc/2way/*.cpp src/uc/2way/*.h src/config.h)

add_library(UC_LIB ${UC2_SOURCE_FILES} ${UC4_SOURCE_FILES} src/debug.cpp src/debug.h)
target_link_libraries(GAMMA_LIB)

add_executable(UC src/UC.cpp)
target_link_libraries(UC GAMMA_LIB)
target_link_libraries(UC UC_LIB)

add_executable(bristol src/bristol_to_SHDL.cpp src/config.h)
