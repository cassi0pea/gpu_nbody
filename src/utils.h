//
// Created by cassie on 10/4/25.
//

#ifndef GPU_NBODY_UTILS_H
#define GPU_NBODY_UTILS_H
//
// utils.h
// Created by cassie on 10/3/25.
//

#include <vector>
#include <random>
#include "Constants.h"
#include "quadtree.h"

// === Randomization Globals ===
// Declare (but don't define) the random engine and distributions
// These are defined in utils.cpp
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> mass_dist;
extern std::uniform_real_distribution<> pos_dist;

// === Function Prototypes ===

// Builds a quadtree from a list of bodies
Quadtree build_quadtree(std::vector<Body>& bodies);

// Generates a list of bodies with random positions/masses
std::vector<Body> gen_bodies(double n);
std::vector<Body> gen_bodies_disk(double n);


#endif //GPU_NBODY_UTILS_H
