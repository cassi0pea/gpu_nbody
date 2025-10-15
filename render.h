#pragma once
#include <vector>
#include <string>
#include "simulation.h"

void createFrame(char* image, double* hdImage, std::vector<Body> bodies, int step);
void writeRender(char* data, double* hdImage, int step);
void renderBodies(std::vector<Body> bodies, double* hdImage);
void colorDot(double x, double y, double vMag, double* hdImage);
void colorAt(int x, int y, const struct color& c, double f, double* hdImage);
double clamp(double x);
double magnitude(const vec2& v);
double toPixelSpace(double p, int size);
void renderClear(char* image, double* hdImage);

struct color
{
    double r, g, b;
};