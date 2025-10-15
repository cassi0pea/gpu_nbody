//
// Created by cassie on 9/26/25.
//
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <random>
#include <iostream>

#include "Constants.h"
#include "quadtree.h"
#include "simulation.h"
#include "render.h"



int main(int argc, char * argv[]){
    if (argc < 2) {
        std::cerr << "Error: Please provide a number of frames for the simulation to generate \n";
        return 0;
    }


    // create the simulation. all data generation happens in there
    Simulation sim = Simulation();

    // Makes arrays to hold the data of the image
    // they arent vectors because they don't need to be resized
    char * image = new char[WIDTH*HEIGHT*3];
    double * hdImage = new double[WIDTH*HEIGHT*3];

    int stepcount = atoi(argv[1]);
    for (int i=0; i<stepcount;i++ ) {
        sim.step();
        std::cout << "Step " << sim.frame << "\n";
        /*
        if (sim.bodies.size() > 0) {
            std::cout << "Step " << sim.frame << " - First body pos: ("
                      << sim.bodies[0].pos.x << ", " << sim.bodies[0].pos.y << ")\n";
            std::cout << "  Pixel coords: ("
                      << toPixelSpace(sim.bodies[0].pos.x, WIDTH) << ", "
                      << toPixelSpace(sim.bodies[0].pos.y, HEIGHT) << ")\n";
            std::cout << "  Velocity mag: " << magnitude(sim.bodies[0].vel) << "\n";
        }
         */
        createFrame(image, hdImage, sim.bodies, sim.frame);
    }
    std::cout << "Simulation completed successfully. Generating video... \n";
    system("./make_video.sh");


}