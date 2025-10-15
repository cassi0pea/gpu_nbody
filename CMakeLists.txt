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
        quadtree.cpp
        nbody_main.cpp
        Constants.h
        quadtree.h
        simulation.cpp
        utils.cpp
        render.cpp
        render.h
        simulation.h
        utils.h)

# Link OpenMP if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(gpu_nbody PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(gpu_nbody PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_options(gpu_nbody PRIVATE ${OpenMP_CXX_FLAGS})
endif()