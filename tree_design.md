# Tree Design Document

## Tree Design
For my tree I selected a quadtree. I picked such because quadtrees are very useful for speeding up gravitational simulations, which i had a pre-existing interest in; the program I wrote for this assignment runs in O(nlogn) time compared to a list-based simulation which would have run in O(n^2) time. This tree is characterized by its property of always splitting into four equally sized child nodes when it branches. It is usually used to represent physical space, with each node being some bounding box, and each child of a branch node being 1/4th of that box.

I elected not to use an interface for this project, as the usual benefits of modularity are irrelevant here -- only a quadtree would work, as the algorithm I chose to implement (the Barnes-Hut gravitational approximation algorithm
