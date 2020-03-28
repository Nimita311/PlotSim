cmake_minimum_required(VERSION 3.0)
project(PlotSimLib)

set(SOURCE_FILES
    app.cc
    simulator.cc
    tile.cc
)

add_library(plotsimlib SHARED STATIC ${SOURCE_FILES})
target_link_libraries(plotsimlib ${SDL2_LIBRARIES})
