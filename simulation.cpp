#include "simulation.h"


// Implementation of Simulation methods
Simulation::Simulation()
        : delta_t(0.05),
          frame(0),
          bodies(gen_bodies_disk(NUM_BODIES)),
          ygg(build_quadtree(bodies)) {}

Simulation::Simulation(double delta_t, std::size_t frame, std::vector<Body> bodies, Quadtree ygg)
        : delta_t(delta_t), frame(frame), bodies(std::move(bodies)), ygg(std::move(ygg)) {}

void Simulation::step() {

    iterate();
    //collide(); // i want collision detection but that seems like a WHOLE thing so we're ignoring it for now
    attract();
    frame += 1;
}

void Simulation::iterate() {
    #pragma omp parallel for
    for (Body& body : bodies) {
        body.update(delta_t);
    }
}

void Simulation::collide() {
    // TODO: Implement collision detection/response
}

void Simulation::attract() {
    //printf("attracting!\n");
    Quad root;
    root.new_containing(bodies);
    ygg.reset(root);

    for (Body& body : bodies) {
        ygg.insert(body.pos, body.mass);
    }

    ygg.propogate();

    // TODO: GPU parelelize this
    #pragma omp parallel for
    for (Body& body : bodies) {
        body.accel = ygg.accel(body.pos);
    }
}
