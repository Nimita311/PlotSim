cmake_minimum_required(VERSION 3.0)
project(PlotSim)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(SDL2 REQUIRED)
set(COMMON_INC_DIRS
    ${SDL2_INCLUDE_DIRS}
    lib
    maps
)
include_directories(${COMMON_INC_DIRS})

add_subdirectory(lib)
add_subdirectory(part1)
add_subdirectory(part2)

add_executable(plotsim1 part1/plotsim.cc)
add_executable(plotsim2 part2/plotsim.cc)
target_link_libraries(plotsim1 plotsimlib plotsimpart1)
target_link_libraries(plotsim2 plotsimlib plotsimpart2)
target_include_directories(plotsim1 PUBLIC part1)
target_include_directories(plotsim2 PUBLIC part2)
