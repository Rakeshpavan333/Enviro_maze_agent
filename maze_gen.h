#ifndef __MAZE_GEN__H
#define __MAZE_GEN__H 

#include <bits/stdc++.h>
#include <json/json.h>

using nlohmann::json; 

class point {
public:
	point() {
		_x = 0; _y = 0;
	};
	point(int x, int y){
		_x = x; _y = y;
	};
	int x() { return _x; };
	int y() { return _y; };
	void set(int x, int y) { _x = x; _y = y; };
	json to_json() { json p = {{"x", _x}, {"y", _y}}; return p; };
private:
	int _x, _y;
};

class MAZE {
public: 
	MAZE(point a, point b, point c, point d, int width){
		_corners.push_back(a); _corners.push_back(b);
		_corners.push_back(c); _corners.push_back(d);
		_width = width;
		rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	};

	void sub_maze(std::vector<point> corners, bool orientation);
	void create_hole(std::vector<point> wall, bool orientation);
	void generate_random_maze();
	void update_enviro_config();
	int random(int len);
	
private:
	std::vector<point> _corners;
	std::vector<std::vector<point>> _walls;
	int _width;
	std::mt19937 rng;
};

#endif