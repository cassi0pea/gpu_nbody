//
// Created by cassie on 10/3/25.
//
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <random>

#include "Constants.h"
#include "quadtree.h"


// setting up the randomizer for body generation later
// create random device and engine
std::random_device rd;                   // non-deterministic seed
std::mt19937 gen(rd());                  // Mersenne Twister engine

// create distribution
std::uniform_real_distribution<> mass_dist(BODY_MIN_MASS, BODY_MAX_MASS);
std::uniform_real_distribution<> pos_dist(0,std::min(HEIGHT/2,WIDTH/2));
// takes the min so we dont generate bodies out-of-bounds

//  builds a quadtree given a list of bodies
//  gets a reference to the list
Quadtree build_quadtree(std::vector<Body>& bodies) {
    Quadtree ygg;
    Quad root;
    root.new_containing(bodies);
    ygg.reset(root); // wipes the tree clean, rebases it with the new root

    // for each body in bodies (getting a reference, read-only)
    for (const Body& body : bodies) {
        ygg.insert(body.pos, body.mass);
    }
    printf("Yggdrasil built!\n");
    return ygg;
}

/* Helper function to generate a vector (list) of bodies with randomized properties
 *      input: a double n representing the number of bodies to generate
 *      returns: list of bodies
 *      side effects: none
 */
std::vector<Body> gen_bodies(double n) {
    // creates a list of body objects of length n
    std::vector<Body> bodies(n);
    // If we're using random body masses, generates masses as well as positions randomly within set ranges
    // Otherwise, just generates the positions randomly
    if (RANDOM_BODY_MASS == 1) {
        for (Body& body : bodies) {
            body.mass = mass_dist(gen);
            body.pos.x = pos_dist(gen);
            body.pos.y = pos_dist(gen);
        }
    } else {
        for (Body& body : bodies) {
            body.mass = BODY_FIXED_MASS;
            body.pos.x = pos_dist(gen);
            body.pos.y = pos_dist(gen);
            //printf("Body position (in Kg): %lf , %lf", body.pos.x, body.pos.y);
            //printf("\n");

        }
    }

    // Makes one body absolutely fuckoff huge
    // this is to mimic a central "star" or black hole
    // leaving the position random for now just to see what happens
    bodies[0].mass = BODY_FIXED_MASS * 1024;
    bodies[0].pos.x = WIDTH/2;
    bodies[0].pos.y = HEIGHT/2;

    printf("Done generating bodies\n");
    return bodies;
}


std::vector<Body> gen_bodies_disk(double n) {
    std::vector<Body> bodies(n);

    // Central massive body (star/black hole)
    bodies[0].mass = 100;
    bodies[0].pos = vec2(HEIGHT/2, WIDTH/2);  // Center of screen
    bodies[0].vel = vec2(0, 0);
    bodies[0].accel = vec2(0, 0);

    // Disk parameters
    const double centerX = HEIGHT/2;
    const double centerY = WIDTH/2;
    const double innerRadius = INNER_RADIUS; // was 50   // Minimum distance from center
    const double outerRadius = OUTER_RADIUS;  // was 400 //Maximum distance from center
    const double diskThickness = 0.08; // How "flat" the disk is

    // Random distributions
    std::uniform_real_distribution<> angle_dist(0, 2 * 3.14159265);
    std::uniform_real_distribution<> radius_dist(innerRadius, outerRadius);
    std::uniform_real_distribution<> thickness_dist(-diskThickness, diskThickness);
    std::uniform_real_distribution<> mass_variation(0.8, 1.2);

    for (size_t i = 1; i < n; i++) {
        // Mass in your mass units
        bodies[i].mass = 0.001 * mass_variation(gen);  // Small bodies

        // Position - centered at origin
        double angle = angle_dist(gen);
        double radius = radius_dist(gen);  // in your length units

        bodies[i].pos.x = radius * cos(angle);
        bodies[i].pos.y = radius * sin(angle) + thickness_dist(gen);

        // Orbital velocity using your scaled G
        // v = sqrt(G * M / r) where everything is in your units
        const double G_scaled = G;  // Your calculated G
        double centralMass = bodies[0].mass;
        double orbitalSpeed = sqrt(G_scaled * centralMass / radius);

        // Add some eccentricity
        std::uniform_real_distribution<> velocity_variation(0.9, 1.1);
        orbitalSpeed *= velocity_variation(gen);

        // Velocity perpendicular to radius (tangent for circular orbit)
        bodies[i].vel.x = -orbitalSpeed * sin(angle);
        bodies[i].vel.y = orbitalSpeed * cos(angle);

        bodies[i].accel = vec2(0, 0);
    }

    printf("Done generating %zu bodies in disk configuration\n", n);
    return bodies;
}
