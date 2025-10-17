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


// I wanna implement charged particles
/*  We could do this by assigning half of the bodies a positive charge, the other half a negative charge
 *  then calculating attraction / repulsion according to whatever laws govern that sort of thing
 *  in order to preserve barnes-hut performance gains we also need some notion of a "center of charge"
 *
 *  currently, mass is a value in our special units, and we have some logic for when that sort of thing is inherited
 *  i *thinnk* charge actually works the same way tbh -- in couloumbs, maybe
 *  so we give
 *
 */
int main(int argc, char * argv[]){
    if (argc < 2) {
        std::cerr << "Error: Please provide a number of frames for the simulation to generate \n";
        return 0;
    }

    system("rm images/*");

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