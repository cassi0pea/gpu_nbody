# Cassie's N-Body Simulation Project
An n-body gravitational simulation implementing the Barnes-Hut quadtree approximation algorithm 

## Acknowledgments

This project features a great deal of code heavily translated from, inspired by, and straight up cribbed from two similar projects:
- **Core algorithm**: Adapted from [DeadlockCode](https://github.com/DeadlockCode/barnes-hut)
- **Rendering system and constants**: Adapted from [Peter Whidden](https://github.com/PWhiddy/Nbody-Gravity)
## Requirements
For the python wrapper, the only dependencies are Tkinter and pillow. For the underlying c++ code, you'll need the latest version of cmake and ffmpeg.

## Installation and running

To install, clone the repository. To run, simply run the python wrapper. Should be self-explanatory from there

## Algorithm Details
As mentioned, this project is an implementation of the Barnes-Hut algorithm, which is an optimization algorithm for an n-body gravitational simulation utilizing a quadtree. On the face of things, an n-body sim is really really easy: simply have a list of all bodies in the sim, and on each step, go through the list and apply a gravitational attraction calculation between each pair of bodies. In practice though, this very quickly becomes insanely expensive to compute due to exponential growth, so most simulators use some kind of approximation algorithm such as this one. 

The algorithm works by using a quadtree to detect when a given body is far away from a cluster of other bodies, such that those clustered bodies can be treated as a singular body at their center of mass for the purposes of calculating gravitational attraction. This cuts down massively on the number of calculations which must be done, allowing the simulation to run in approximately nlogn time
