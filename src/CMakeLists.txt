cmake_minimum_required(VERSION 3.17)
project(quantum)

set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_FLAGS "-mcmodel=medium") # on Mac, this flag does not work, so removed 20250602

add_executable(Brightkite-aug Brightkite-aug.cpp)

add_executable(Brightkite-v-N Brightkite-v-N.cpp)
add_executable(Brightkite-v-k Brightkite-v-k.cpp)
add_executable(Brightkite-v-B Brightkite-v-B.cpp)
add_executable(Brightkite-dyn-v-N Brightkite-dyn-v-N.cpp)

# add_executable(Bright_KNN-v-N Bright_KNN-v-N.cpp)

add_executable(IMDb-process IMDb-process.cpp)

add_executable(IMDb_search-v-N IMDb_search-v-N.cpp)
add_executable(IMDb_search-v-k IMDb_search-v-k.cpp)
add_executable(IMDb_maximum-v-N IMDb_maximum-v-N.cpp)
add_executable(IMDb_maximum-v-k IMDb_maximum-v-k.cpp)

add_executable(TPCH_join1 TPCH_join1.cpp)
add_executable(TPCH_join2 TPCH_join2.cpp)

add_executable(TPCH_join1_rand TPCH_join1_rand.cpp)
add_executable(TPCH_join2_rand TPCH_join2_rand.cpp)