# The main shell script to start the project. The order of execution is as follows:
# First we compile the maze generator, and run it. The maze generator module writes static elements to the 'config.json' file. 
# After that, we run the makefile which compiles the agent (Robot) code and start the enviro simulator. 

g++ -std=c++17 -Wno-psabi maze_gen.cc maze_gen.h -o run
./run 
make
enviro
