#include <iostream>
#include <cstring>
#include <fstream>
#include "simulation.h"
#include "Constants.h"
#include "render.h"



void render_frame(Simulation sim) {
    /*  Simulation variables
        double delta_t;              // Time step
        std::size_t frame;           // Frame counter
        std::vector<Body> bodies;    // All bodies in the simulation
        Quadtree ygg;                // The Barnes–Hut quadtree ("Yggdrasil")
     */

    /*  We'll render in 3 steps
     *  1. Clear the image array
     *  2. Render the particles in the array
     *  3. Write that array to ppm file
     *
     */

}
void renderClear(char* image, double* hdImage)
{
    memset(image, 0, WIDTH*HEIGHT*3);
    memset(hdImage, 0, WIDTH*HEIGHT*3*sizeof(double));
}

double toPixelSpace(double p, int size)
{
    //const double SYSTEM_SIZE = 3.5;    // Max distance from center in your length units
    //const double RENDER_SCALE = 2.5;   // Extra zoom/padding

    // Map from [-SYSTEM_SIZE*RENDER_SCALE, +SYSTEM_SIZE*RENDER_SCALE] to [0, size]
    // p=0 (center) -> size/2 (middle pixel)
    // p=-8.75 -> 0
    // p=+8.75 -> size
    return (size/2.0) * (1.0 + p/(SYSTEM_SIZE*RENDER_SCALE));
}

double magnitude(const vec2& v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

double clamp(double x)
{
    return fmax(fmin(x,1.0),0.0);
}

void colorAt(int x, int y, const struct color& c, double f, double* hdImage)
{
    int pix = 3*(x+WIDTH*y);
    hdImage[pix+0] += c.r*f; // Add red contribution
    hdImage[pix+1] += c.g*f; // Add grn contribution
    hdImage[pix+2] += c.b*f; // Add blu contribution
}

void colorDot(double x, double y, double vMag, double* hdImage)
{
    // These are weird and arbitrary. They were chosen with much more care in the Peter Whidden implementation.
    constexpr double velocityMax = 4; // MAX_VEL_COLOR
    constexpr double velocityMin = .1; // MIN_VEL_COLOR

    if (vMag < velocityMin)
        return;
    const double vPortion = sqrt((vMag-velocityMin) / velocityMax);
    color c;
    c.r = clamp(4*(vPortion-0.333));
    c.g = clamp(fmin(4*vPortion,4.0*(1.0-vPortion)));
    c.b = clamp(4*(0.5-vPortion));

    // Compute pixel position once
    const double xPixel = toPixelSpace(x, WIDTH);
    const double yPixel = toPixelSpace(y, HEIGHT);
    const int xP = static_cast<int>(floor(xPixel));
    const int yP = static_cast<int>(floor(yPixel));

    // Precompute constants
    constexpr double sharpnessSq = PARTICLE_SHARPNESS * PARTICLE_SHARPNESS;
    constexpr double exponent = 0.75;

    // Calculate bounds with safety checks
    const int xMin = xP - DOT_SIZE/2;
    const int xMax = xP + DOT_SIZE/2;
    const int yMin = yP - DOT_SIZE/2;
    const int yMax = yP + DOT_SIZE/2;

    for (int i = xMin; i < xMax; i++)
    {
        // Bounds check and precompute x component
        if (i < 0 || i >= WIDTH) continue;

        const double dx = i - xPixel;
        const double expX = exp(sharpnessSq * dx * dx);

        for (int j = yMin; j < yMax; j++)
        {
            // Bounds check
            if (j < 0 || j >= HEIGHT) continue;

            const double dy = j - yPixel;
            const double expY = exp(sharpnessSq * dy * dy);

            const double cFactor = PARTICLE_BRIGHTNESS /
                                   (pow(expX + expY, exponent) + 1.0);

            colorAt(i, j, c, cFactor, hdImage);
        }
    }

}

void renderBodies(std::vector<Body> bodies, double* hdImage)
{
    int rendered = 0;
    // could cause slight issues, probably worth it though
    #pragma omp parallel for
    for (const Body& body : bodies) {
        double x = toPixelSpace(body.pos.x, WIDTH);
        double y = toPixelSpace(body.pos.y, HEIGHT);

        // dot size is just "8" and apparently defines the "range of pixels to render" (???)
        if (x>DOT_SIZE && x<WIDTH-DOT_SIZE &&
            y>DOT_SIZE && y<HEIGHT-DOT_SIZE)
        {
            rendered++;
            double vMag = magnitude(body.vel);
            colorDot(body.pos.x, body.pos.y, vMag, hdImage);
        }
    }
    //std::cout << "Rendered " << rendered << " out of " << bodies.size() << " bodies\n";
}

void writeRender(char* data, double* hdImage, int step)
{

    for (int i=0; i<WIDTH*HEIGHT*3; i++)
    {
        data[i] = int(255.0*clamp(hdImage[i]));
    }

    int frame = step;
    char name[128];
    sprintf(name, "images/Step%05i.ppm", frame);
    std::ofstream file (name, std::ofstream::binary);

    if (file.is_open())
    {
//		size = file.tellg();
        file << "P6\n" << WIDTH << " " << HEIGHT << "\n" << "255\n";
        file.write(data, WIDTH*HEIGHT*3);
        file.close();
    }

}

void createFrame(char* image, double* hdImage, std::vector<Body> bodies, int step)
{
    renderClear(image, hdImage);
    renderBodies(bodies, hdImage);
    writeRender(image, hdImage, step);
}

/*  TODO:
 * Adapt renderBodies to use the vector of bodies instead of the pointer
 * implement toPixelSpace
 * implement Magnitude
 *
 */


