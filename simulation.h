//
// simulation.h
// Created by cassie on 10/3/2025.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "quadtree.h"
#include "utils.h"
#include "Constants.h"

// ==============================================
//  Simulation
//  Represents one instance of an N-body simulation.
//  Holds all bodies, the quadtree, and simulation state.
// ==============================================
struct Simulation {
    double delta_t;              // Time step
    std::size_t frame;           // Frame counter
    std::vector<Body> bodies;    // All bodies in the simulation
    Quadtree ygg;                // The Barnes–Hut quadtree ("Yggdrasil")

    // Constructors
    Simulation();
    Simulation(double delta_t, std::size_t frame, std::vector<Body> bodies, Quadtree ygg);

    // Core simulation steps
    void step();     // Advance one simulation step
    void iterate();  // Update positions/velocities
    void collide();  // Handle collisions (currently stub)
    void attract();  // Compute gravitational acceleration
};

#endif // SIMULATION_H
