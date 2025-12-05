cmake_minimum_required(VERSION 4.0)
project(gpu_nbody)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found!")
else()
    message(WARNING "OpenMP not found - continuing without parallelization")
endif()

add_executable(gpu_nbody
        src/quadtree.cpp
        src/nbody_main.cpp
        src/Constants.h
        src/quadtree.h
        src/simulation.cpp
        src/utils.cpp
        src/render.cpp
        src/render.h
        src/simulation.h
        src/utils.h)

# Link OpenMP if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(gpu_nbody PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(gpu_nbody PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(gpu_nbody PRIVATE ${OpenMP_CXX_FLAGS})
endif()