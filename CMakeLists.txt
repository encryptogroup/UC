cmake_minimum_required(VERSION 3.6)
project(ValiantUC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cmake-build-debug)

file(GLOB GAMMA_SOURCES src/Gamma/gamma/*.cpp src/Gamma/util/*.cpp)
file(GLOB GAMMA_HEADERS src/Gamma/gamma/*.h src/Gamma/util/*.h src/config.h)
add_library(GAMMA_LIB ${GAMMA_SOURCES} ${GAMMA_HEADERS} src/UC.cpp)

file(GLOB UC4_SOURCE_FILES src/uc/4way/*.cpp src/uc/4way/*.h src/config.h)

file(GLOB UC2_SOURCE_FILES src/uc/2way/*.cpp src/uc/2way/*.h src/config.h)

add_executable(UC src/UC.cpp ${UC2_SOURCE_FILES} ${UC4_SOURCE_FILES} src/debug.cpp src/debug.h)
target_link_libraries(UC GAMMA_LIB)

add_executable(bristol src/bristol_to_SHDL.cpp)
