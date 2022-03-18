# Maze Generation & Solving 
- A randomized maze generator using a divide & conquer algorithmic paradigm (recursive division).
- A dynamic agent (robot) that uses effective backtracking + depth-first-search (DFS) to solve the maze. 

## Key challenges & highlights:
1. (Algorithm & design) DFS + Backtracking using map and stack:
   1. Visualizing the grid co-ordinates as graph nodes and use graph based search techniques like Depth-First Search (DFS) and Backtracking, without actually constructing the graph or edges explicitly. Assuming the the robot is performing a DFS traversal through the maze. 
   2. Designing a DFS algorithm without using recursion: Given how event management and process scheduling works in elma and enviro it is difficult to code a recursive DFS inside during() or any class method. Solved this efficiently by using the first-in-last-out data-structure (STL `stack`). The stack tracked the order in which nodes (i.e the robot position co-ordinates) get accessed and traversed in the maze. 
   3. Keeping tracking of the cells visited by the robot in the maze using a STL `map<>` container. 
   4. Effectively backtracking through dead ends (i.e leaf nodes) in linear time by using `stack` to track the order of entry of cells.  
2. (Algorithm & design) Randomized divided & conquer based maze generation:
   1. Recursively dividing the empty region into smaller sub-regions and processing each through a similar recursive subroutine. 
   2. Using randomly generated vertical and horizontal axes to divide the region. Interpreting the axes as static agents (or walls). 
   3. Creating a randomized gap in each of the walls (or axis) to connect the divided sub-regions, to form a maze. 
3. Implementation: 
   1. Working with integer aproximations of the robot locations. Storing and handling the state space (in maps and stacks) would be too large to handle if we consider all the possible floating point values too. 
   2. Calculating closed polygon points inside the divide & conquer to represent static agents in a format the enviro expects. 
   3. Integrating DFS+Backtracking and Divide & Conquer paradigms as processes in enviro. 
   4. Converting the constructed maze to json and writing to the `config.json` file that enviro renders on screen. 

I did not get the time to catalogue all the bugs and issues I face during the implementation phase. 

## File structure & hierarchy:
Briefly highlighting some important files:
- I have define and implement various modules used for generating the randomized maze in the files `maze_gen.h` and  `maze_gen.cc`. The maze is then converted to json and written into the `config.json` file using the `update_enviro_config()` method in the MAZE class.
- `run.sh` is used to start the project 
- `src/my_robot.h` used to implement the Robot controller (DFS + Backtracking). 


## Starting the project: 

``` 
bash run.sh 
```

The bash script performs the following:
- Compiles and runs `maze_gen`. 
- Compiles the robot source codes through `make`
- Starts Enviro 

## Usage:


## References:
All the code is original and written from scratch by me. I don't use any pre-built libraries or third-party code. However, the general algorithmic principles used are from popular literature:- 
1. Recursive Division for Maze Generation [link](https://weblog.jamisbuck.org/2011/1/12/maze-generation-recursive-division-algorithm)
2. DFS and BFS using Stacks and Queues [PDF](http://web.cs.unlv.edu/larmore/Courses/CSC477/bfsDfs.pdf)

