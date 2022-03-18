#ifndef __MAZE_GEN__H
#define __MAZE_GEN__H 

//! This file declares the classes used to define the maze generator and the maze structure. 

#include <bits/stdc++.h>
#include <json/json.h>

using nlohmann::json; 

//! points are the fundamental building blocks to define more complex structures
class point {
public:
	//! Constructors
	point() {
		_x = 0; _y = 0;
	};
	point(int x, int y){
		_x = x; _y = y;
	};

	//! getters for the point co-ordinates
	//! \return the X and Y co-ordinates of the point
	int x() { return _x; };
	int y() { return _y; };

	//! setter to set the point values
	//! \param the X and Y co-ordinates 
	void set(int x, int y) { _x = x; _y = y; };

	//! a helper function to convert the point to a json object
	//! \return json object containing the X and Y co-ordinates 
	//! in the format expected by the Enviro config file
	json to_json() { json p = {{"x", _x}, {"y", _y}}; return p; };
private:
	int _x, _y;
};

//! Maze class defines the structure of the maze as a composition of points
class MAZE {
public: 
	//! Constructor 
	//! we also seed the random number generator
	MAZE(point a, point b, point c, point d, int width){
		_corners.push_back(a); _corners.push_back(b);
		_corners.push_back(c); _corners.push_back(d);
		_width = width;
		rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	};

	//! The Divide & Conquer paradigm will be implemented in this sub_maze module
	//! \param the corners of the sub-space to construct the maze, 
	//! \param Divide orientation: True value represents a vertical division of the space and False represents horizontal
	void sub_maze(std::vector<point> corners, bool orientation);

	//! A module to punch a hole in the static element (wall) to connect the two divided sub-spaces 
	//! \param The static elements defined as a closed polygon (wall)
	//! \param The orientation of the static element (horizontal/vertical)
	void create_hole(std::vector<point> wall, bool orientation);

	//! starts the random maze generation process
	void generate_random_maze();

	//! Handles converting the class members to a json object and writing them to 'config.json'
	void update_enviro_config();

	//! The random number generator. Generates an integer in a closed interval based on the input.
	//! \param len used to calculate the closed interval [low, high] for random number generation
	int random(int len);
	
private:
	std::vector<point> _corners;
	std::vector<std::vector<point>> _walls;
	int _width;
	std::mt19937 rng;
};

#endif
