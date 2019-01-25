# DungeonGenerator
My take on a random dungeon generator

Run: `sudo apt-get install libsfml-dev`
followed by `make`
Commands: 
G - Generate a new dungeon. Will clear any Delaunay or MST present
D - Will perform Delaunay Triangulation assuming a dungeon has been generated
M - Will create a Minimum Spanning Tree assuming the Delaunay triangulation has been performed
H - Will create hallways connecting rooms according to the minimum spanning tree

To change parameters such as room count, dungeon scale, degrees of freedom for the room generation or to activate debug mode simply adjust the #defines at the top of main.cpp and recompile the project. 

# DEBUG
Setting it to 0 will turn off debug messages while 1 turns it on
# DOF
Refers to the k value used in the chi-squared dstribution which is how the program generates the dimensions of its rooms (more information on chi-squared sitribution can be found here https://en.wikipedia.org/wiki/Chi-squared_distribution)
# DUNGSCALE
The multiplier used to make the rooms bigger since the chi-squared distribution only provides a float between 0-10
# RADIUS determines the size of the circle the rooms are initially placed in
# SEED allows you to manually set the seed for the dungeon, setting it to 0 allows the program to choose it randomly.

Dungeon generation pseudo code has been lifted from the following reddit post but all implementation is my own:
https://www.reddit.com/r/gamedev/comments/1dlwc4/procedural_dungeon_generation_algorithm_explained/

The Delaunay triangulation library I use here is developed by the Github user Bl4ckb0ne and the open source liscence is as follows:

Copyright (c) 2015 Simon Zeni (simonzeni@gmail.com)


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

This copyright applies to all source files found in the delaunay folder
