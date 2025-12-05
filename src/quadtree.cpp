#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <limits>
#include "Constants.h"
#include <algorithm>
#include <array>
#include <omp.h>

#include "quadtree.h"

// ## IMPLEMENTATION FILE ##


/*  Method to create a root node that encompasses all given bodies
 *  inputs: a list of bodies
 *  outputs: nothing?
 *  side effects: the properties of the effected node change to encompass all points given
 *
 *  note: I hated the name "new_containing" initially but it DOES actually make sense
 *          it makes a new node containing (bounding) these bodies
 */

void Body::update(double delta_t) {
    vel = vel + (accel * delta_t);
    pos = pos + (vel * delta_t);
}

void Quad::new_containing(const std::vector<Body>& bodies) {
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();

    // for body in bodies:
    for (const Body& body : bodies) {
        // count down and up such that you end up with values that bound all points in the list
        min_x = std::min(min_x, body.pos.x);
        min_y = std::min(min_y, body.pos.y);
        max_x = std::max(max_x, body.pos.x);
        max_y = std::max(max_y, body.pos.y);
    }
    center = vec2((min_x + max_x) * .5, (min_y + max_y) *.5);
    length = std::max(max_x - min_x, max_y - min_y);
}

/*  Method to find the subquadrant of a box that a given position is in
 *  inputs:         an vec2 (probably an x,y coord)
 *  outputs:        a number from 0 to 3 representing one of 4 quadrants
 *  side effects:   none
 */
int Quad::find_quadrant(vec2 pos) {
    /*  NW = 0
     *  NE = 1
     *  SW = 2
     *  SE = 3
     */
    if (pos.x <= center.x && pos.y >= center.y) {return 0;}
    if (pos.x >  center.x && pos.y >= center.y) {return 1;}
    if (pos.x <= center.x && pos.y <  center.y) {return 2;}
    return 3; // pos.x > center.x && pos.y < center.y
}

// Returns one sub-quad for a given quadrant
Quad Quad::into_quadrant(int quadrant) const  {
    // generates dummy quadrant
    Quad q;
    // gives it half the side length of it's parent and the same center
    q.length = length * 0.5;
    q.center = center;

    double offset = q.length * 0.5;
    //moves the child quad to the right subposition
    switch (quadrant) {
        case 0: // NW
            q.center.x -= offset;
            q.center.y += offset;
            break;
        case 1: // NE
            q.center.x += offset;
            q.center.y += offset;
            break;
        case 2: // SW
            q.center.x -= offset;
            q.center.y -= offset;
            break;
        case 3: // SE
            q.center.x += offset;
            q.center.y -= offset;
            break;
    }
    //returns the created quadrant
    return q;
}

// Returns all four sub-quads [NW, NE, SW, SE]
std::array<Quad, 4> Quad::subdivide_quad() const {
    return { into_quadrant(0), into_quadrant(1),
             into_quadrant(2), into_quadrant(3) };
}


/*  Method to determine if a given node has any subnodes
 *  in other words: if the given node is a leaf or a branch (doesnt distinguish leaf from empty
 *      inputs:     a node
 *      outputs:    a truth value
 *      side effects: none
 */
bool Node::has_children() {
    if(children == 0) // apparent
        return false;
    else {
        return true;
    }
}
// This one might actually be self_documenting
bool Node::is_empty() {
    if (mass == 0) {
        return true;
    } else {
        return false;
    }
}

// that is to say, a node with one body
bool Node::is_leaf() {
    return (children == 0);
}

/*  Method to insert a body into the structure of the quadtree
 *      currently incomplete
 *
 */
void Quadtree::insert(vec2 body_pos, double body_mass) {
    // traverse down the tree until reaching the leaf node containing the given position

    // we're storing indexes here
    uint32_t node = 0; // start at the root node
    while (nodes[node].has_children()) {
        int quadrant = nodes[node].quad.find_quadrant(body_pos);
        node = nodes[node].children + quadrant;
            // this traverses down the tree. it is a little weird though because i still dont think any children exist
            // probably that all happens in the subdivide method
    }

    // if the node has nothing in it, then the body we're inserting is the only body in the node
    // so the center of mass and total mass is the same as the position and mass of the body itself
    // and we exit
    if (nodes[node].is_empty()) {
        nodes[node].centm = body_pos;
        nodes[node].mass = body_mass;
        return;
    }

    // if the position of the body we're trying to insert is the same as the existing center of mass
    // add the mass of the body to the total mass of the node.
    // this is an edge case check to simplify calculations and avoid weird infinite subdivisions


    if (body_pos == nodes[node].centm) {
        nodes[node].mass += body_mass;
        return;
    }


    // subdivide until the center of mass of the current node and the position of the body
    // are no longer in the same leaf node
    // then set their positions and masses

    auto bp = body_pos; // = pos
    auto np = nodes[node].centm; // == p
    while(true) {
        // children == index of first newly created child node
        auto children = this->subdivide(node);
        // pos == body_pos
        // p ==  centm of the current node
        // this makes sense because we're getting quadrant NUMBERS back
        // not objects
        auto q1 = nodes[node].quad.find_quadrant(np); // node quad
        auto q2 = nodes[node].quad.find_quadrant(bp); // body quad

        // if theyre in the same subquadrant:
        //      update current node to the first subchild (i.e. NW)
        if (q1 == q2){
            node = children + q1;
        } else {
            auto n1 = children + q1;
            auto n2 = children + q2;

            nodes[n1].centm = nodes[node].centm;
            nodes[n1].mass = nodes[node].mass;
            nodes[n2].centm = body_pos;
            nodes[n2].mass = body_mass;
            return;
        }
    }

}


/*  Method to remove all nodes from the tree, and rebase with a given root node
 *  this is called at the beginning of every sim step
 *      inputs:         a tree, a root quad
 *      outputs:        none
 *      side effects:   none
 */
void Quadtree::reset(Quad root) {
    nodes.clear();
    parents.clear(); //maybe? we dont have that yet
    nodes.push_back(Node(root));
}

/*  Method to subdivide. the. tree?
 *  input: the index of the node to subdivide
 */
std::size_t Quadtree::subdivide(std::size_t node) {
    // 1. Record the parent of the new children
    parents.push_back(node);
    // 2. Index where the new children will start
    std::size_t children = nodes.size(); // preSUMABLY this returns len
    nodes[node].children = children;

    std::array<std::size_t, 4> nexts = {
            children + 1,
            children + 2,
            children + 3,
            nodes[node].next
    };
    // 5. Split this node’s quad into four smaller quads
    auto quads = nodes[node].quad.subdivide_quad();
    for (int i = 0; i < 4; i++) {
        // create new node based on each child
        nodes.push_back(Node(quads[i], nexts[i]));
    }
    // return index of first child
    return children;
}

void Quadtree::propogate() {
    for (auto& node : std::vector<std::size_t>(parents.rbegin(), parents.rend())) {
        // node is a value
        auto i = nodes[node].children;

        nodes[node].centm = nodes[i].centm * nodes[i].mass
                + nodes[i + 1].centm * nodes[i + 1].mass
                + nodes[i + 2].centm * nodes[i + 2].mass
                + nodes[i + 3].centm * nodes[i + 3].mass;

        nodes[node].mass = nodes[i].mass
                           + nodes[i + 1].mass
                           + nodes[i + 2].mass
                           + nodes[i + 3].mass;

        nodes[node].centm = (nodes[node].centm)/(nodes[node].mass);
    }
}

vec2 Quadtree::accel(vec2& body_pos) {
    vec2 accel(0,0);
    std::size_t node = 0; //node index -- starts at 0, i.e. root
    auto theta_sq = THETA*THETA;
    auto epsil_sq = EPSILON*EPSILON;

    while (true) {
       //printf("calculating acceleration for node %zu \n", node);
       Node& n = nodes[node];

       // distance to the center of mass
       vec2 dist(n.centm.x - body_pos.x, n.centm.y - body_pos.y);
        //TODO: add distance to center of charge
       // distance squared
       double dist_sq = dist.mag_sq();

       // "treat this as a single body" test (the barnes-hut approximation secret sauce):
       // leaf OR (size^2 < d^2 * t_sq)  <=> (size/d < theta)
       if (n.is_leaf() || (n.quad.length * n.quad.length) < dist_sq * theta_sq) {
           // sqrt(dist_sq) might cause problems?
            auto denom = (dist_sq + epsil_sq) * sqrt(dist_sq);
            // this actually makes me want to throw up it's so ugly. i hate c++
            // prevents infinite forces

            accel = accel + (dist * std::min(G * n.mass/denom, std::numeric_limits<double>::max()));
            //accel = (dist * (G * n.mass / denom));

            // if there is no next node -- i.e. if we have reached the end of the tree -- break
            // otherwise, set the node to the next node and loop
            if (n.next == 0) { break; } else { node = n.next; }
            
       // if we can't treat the node as a single body:
       //   move to the first child and loop
       } else {
           node = n.children;
       }
    }
    return accel;
}

