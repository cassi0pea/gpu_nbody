# Cassie's N-Body Simulation Project
A Barnes-Hut n-body gravitational simulation with GPU acceleration (currently features zero gpu acceleration)

## Acknowledgments

This project features a great deal of code heavily translated from, inspired by, and straight up cribbed from two similar projects:
- **Core algorithm**: Adapted from [DeadlockCode](https://github.com/DeadlockCode/barnes-hut)
- **Rendering system and constants**: Adapted from [Peter Whidden](https://github.com/PWhiddy/Nbody-Gravity)
## Requirements
I have no idea. Cmake, probably? This is my first time putting a project out like this, I have no idea how to make sure anyone else on the planet can run it.
FFMpeg if you wanna create a video at the end of the sim.

## Installation and running
clone the repo, go into the build folder, type "make" in the terminal,
and if you're lucky an executable will be generated. 
run that with a command line argument specifying the number
of bodies to be generated, and you're off to the races.