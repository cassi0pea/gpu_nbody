//
// Created by cassie on 9/28/25.
//

#ifndef GPU_NBODY_CONSTANTS_H
#define GPU_NBODY_CONSTANTS_H

#define WIDTH	1024*2 // Image render width
#define HEIGHT	1024*2 // Image render height
    // I do eventually want this to run in real time, but this'll still be useful for that

#define SPACE_UNIT 1.5e11 // fundemental unit of distance -- about one AU
#define MASS_UNIT  4e22 // fundemental unit of mass -- in this case, in kg, a little under the weight of a mole of moles, or about half the mass of the moon
#define NUM_BODIES (1e5) // Number of bodies -- goal is 1e9, or a billion bodies
#define BODY_MIN_MASS (1024*1) // Minimum value for the mass of a body in Kg
#define BODY_MAX_MASS (1024*64) // Maximum value for the mass of a body in Kg
#define BODY_FIXED_MASS MASS_UNIT // Just a fixed mass value, for a simpler first version
#define RANDOM_BODY_MASS 0 // Whether or not to randomize body mass -- initialized to "no"
#define THETA 1     // the barnes-hut approximation factor
#define EPSILON 1   // a "smoothing variable". not really sure what it does.
#define PI 3.1415926535
#define G 0.01 // gravity scaled for our space and mass constants

/// Electric constants
#define CHARGED_PARTICLES 0 // whether or not to give generated bodies electric charge

// the tricky thing here is that we picked a spacial unit that makes sense for astronomical bodies, but that's implicitly
// not what we're using for the electrostatic attraction
// because if it was the electrostatic forces would be super tiny UNLESS the charges were fuckoff gigantic

// another problem is that coloumbs law apparently isn't sound for quickly moving bodies


#define MAX_VEL_COLOR 40.0  // Both in km/s
#define MIN_VEL_COLOR 14.0
#define PARTICLE_BRIGHTNESS 0.55//0.03 for 256/512k, 0.4 for 16k
#define PARTICLE_SHARPNESS 1.0 // Probably leave this alone
#define DOT_SIZE 16 // 15  // Range of pixels to render

#define INNER_RADIUS 0.3
#define OUTER_RADIUS 1.5 // size of the disk
#define SYSTEM_SIZE 1.5 // max distance from disk allowed
#define RENDER_SCALE 3.5 // zoom level -- shrink to zoom in


#endif //GPU_NBODY_CONSTANTS_H
