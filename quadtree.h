//
// Created by cassie on 9/29/25.
//
#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <limits>
#include "Constants.h"
#include <algorithm>
#include <valarray>

#ifndef GPU_NBODY_QUADTREE_H
#define GPU_NBODY_QUADTREE_H
/*  Struct that holds an (x,y) coordinate
 *  Example usage:
 *
 *  vec2 point(2, 8);
 *  point.x += 11; // or whatever
 *
 *  We also use this for real vectors like velocity
 */
struct vec2 {
    // Constructor
    vec2() = default;
    vec2 (double x, double y): x(x), y(y) {}
    double x;
    double y;

    // Equality operator
    bool operator==(const vec2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const vec2& other) const {
        return !(*this == other);
    }

    vec2 operator*(const double& other) const {
        return vec2(x*other, y*other);
    }

    vec2 operator+(const vec2& other) const {
        return vec2(x+other.x, y+other.y);
    }
    vec2 operator/(const double& other) const {
        return vec2(x/other, y/other);
    }

    //this should go in the impl file for consistency really
    // but. oh well.
    // Magnitude squared
    double mag_sq() const {
        return x * x + y * y;
    }

    // (Optional) full magnitude
    double mag() const {
        return std::sqrt(mag_sq());
    }

};

/*  Struct that defines a single body in the simulation
 *  bodies have 3 properties: Mass, Velocity, and Acceleration
 *  Mass is obvious and constant
 *  Velocity is where it's going and how fast
 *  Acceleration is how fast its changing how fast it's going. (rate of change of velocity)
 */
struct Body {
    Body() = default;
    Body (double mass, vec2 pos, vec2 vel, vec2 accel): mass(mass), pos(pos), vel(vel), accel(accel) {}
    vec2 pos;
    vec2 vel;
    vec2 accel;
    double radius;
    double mass; // the mass of this singular body. will be constant unless i decide to get sillay with it
    void update(double delta_t);
};

// struct that defines the bounding box of the node
struct Quad {
    //Quad() = default;
    vec2 center;    // the point at the center of the box
    double length;  // the side length of the box.
    void new_containing(const std::vector<Body>& bodies);
    int find_quadrant(vec2 pos);
    Quad into_quadrant(int quadrant) const;
    std::array<Quad, 4> subdivide_quad() const;
};

struct Node {
    Node() = default;
    Node (Quad quad): quad(quad) {}
    Node (Quad quad, std::size_t next): quad(quad), next(next) {}
    std::size_t children = 0; // stores the index of the first child in the ygg list
    std::size_t next = 0; // stores the index of the next full size node after the kiddos
    vec2 centm; // for "center of mass"
    double mass; // for total mass of all bodies in the node
    Quad quad; //stores the data that defines the bounding box
    bool has_children();
    bool is_empty();
    bool is_leaf();
};

// Fundamental structure of the program
// really it's just a list of nodes
struct Quadtree {
    /*
        nodes[i]   = the actual quadtree node
        parents[i] = index of parent of nodes[i], in nodes[]
     */
    std::vector<Node> nodes;
    std::vector<std::size_t> parents;
    // Methods:
    void insert(vec2 pos, double mass);
    void reset(Quad root);
    std::size_t subdivide(std::size_t node);
    void propogate();
    vec2 accel(vec2& body_pos);
};


#endif //GPU_NBODY_QUADTREE_H
