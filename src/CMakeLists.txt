cmake_minimum_required(VERSION 3.17)
project(quantum)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-mcmodel=medium")

add_executable(Brightkite-1 Brightkite-1.cpp)
add_executable(Brightkite-2 Brightkite-2.cpp)
add_executable(gowalla-1 gowalla-1.cpp)
add_executable(gowalla-2 gowalla-2.cpp)
add_executable(Brightkite-3 Brightkite-3.cpp)
add_executable(gowalla-3 gowalla-3.cpp)