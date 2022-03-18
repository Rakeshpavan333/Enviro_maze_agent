#include "maze_gen.h"

//! This file implements the member functions of the MAZE class 
//! In particular, we generate a Maze using a randomized divide & conquer technique
//! The core idea is to generate random vertical and horizontal axes 
//! to recursively divide the space into 4 sub-spaces 

using namespace std;
using nlohmann::json; 

//! The random number generator using current time as the seed
//! We leave a gap of 2*_width on both sides for ease of implementation 
//! as this would let us do away the tricky edge cases 
int MAZE::random(int len){
	rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	int low = 2*_width;
	int high = len - 2*_width-1;
	return std::uniform_int_distribution<int>(low, high)(rng);
}

//! This method makes the first call to sub_maze(), which then recursively calls itself
void MAZE::generate_random_maze() {
	sub_maze(_corners, true);
}

//! Punching a "hole" in a static agent (wall) to connect the two sub-spaces it seperates
//! Orientation indicates if it is a vertical axis or a horizontal axis 
void MAZE::create_hole(vector<point> wall, bool orientation){
	//! The length calculation from the co-ordinates depends on the orientation 
	int len = abs(orientation? wall[2].y() - wall[0].y() : wall[1].x() - wall[0].x());

	//! In case the wall is already smaller than a threshold, we don't proceed
	if(len <= 6*_width){
		return;
	}

	//! The point to open a hole/gap in the static agent is chosen randomly 
	int hole_point = random(len);
	vector<point> new_wall1, new_wall2;

	//! Depending on the orientation (horizontal/vertical) of the wall we handle cases to 
	//! ensure that the two subspaces get connected
	if(orientation){
		point temp_a(wall[1].x(), wall[1].y() + hole_point - 2*_width);
		point temp_b(wall[0].x(), wall[0].y() + hole_point - 2*_width);
		point temp_c(wall[0].x(), wall[0].y() + hole_point + 2*_width);
		point temp_d(wall[1].x(), wall[1].y() + hole_point + 2*_width);

		new_wall1 = {wall[0], wall[1], temp_a, temp_b};
		new_wall2 = {temp_c, temp_d, wall[3], wall[2]};
	} else {
		point temp_a(wall[0].x() + hole_point - 2*_width, wall[0].y());
		point temp_b(wall[0].x() + hole_point + 2*_width, wall[0].y());
		point temp_c(wall[0].x() + hole_point + 2*_width, wall[2].y());
		point temp_d(wall[0].x() + hole_point - 2*_width, wall[2].y());

		new_wall1 = {wall[0], temp_a, temp_d, wall[3]};
		new_wall2 = {temp_b, wall[1], wall[2], temp_c};
	}

	//! Append the newly constructed static agents (walls) to a class member
	_walls.push_back(new_wall1);
	_walls.push_back(new_wall2);

	return;
}

//! Recursively construct the maze by dividing the space into smaller sub-spaces
void MAZE::sub_maze(vector<point> corners, bool orientation){
	//! Base/termination cases to stop when the size of the sub-space is smaller than a threshold
	if(abs(corners[0].x() - corners[1].x()) <= 7*_width 
		|| abs(corners[0].y() - corners[3].y()) <= 7*_width){

		return;
	}

	//! Again, the length calculation to pass to the RNG depends on if the space is cut vertically or horizontally 
	int len = abs(orientation? corners[0].x() - corners[1].x() : corners[0].y() - corners[3].y());
	if(len <= 6*_width){
		return;
	}

	//! We sample a random point. The space will be divided by an axis through this random point
	int divide_point = random(len);

	vector<point> wall;

	//! We define static element (wall) as the axis that separates the different sub-spaces
	if(orientation) {
		point temp_a(corners[0].x() + divide_point - _width, corners[0].y());
		point temp_b(corners[0].x() + divide_point + _width, corners[0].y());
		point temp_c(corners[0].x() + divide_point - _width, corners[3].y());
		point temp_d(corners[0].x() + divide_point + _width, corners[3].y());

		wall = {temp_a, temp_b, temp_c, temp_d};

		//! Recursively calling sub_maze for the smaller sub-spaces 
		sub_maze( {corners[0], temp_a, temp_d, corners[3]} , !orientation);
		sub_maze( {temp_b, corners[1], corners[2], temp_c} , !orientation);
	} else {
		point temp_a(corners[0].x(), corners[0].y() + divide_point - _width);
		point temp_b(corners[1].x(), corners[0].y() + divide_point - _width);
		point temp_c(corners[1].x(), corners[0].y() + divide_point + _width);
		point temp_d(corners[0].x(), corners[0].y() + divide_point + _width);

		wall = {temp_a, temp_b, temp_c, temp_d};	

		//! Recursively calling sub_maze for the smaller sub-spaces
		sub_maze( {corners[0], corners[1], temp_b, temp_a} , !orientation);
		sub_maze( {temp_d, temp_c, corners[2], corners[3]} , !orientation);
	}

	//! Now that we have the static agent (wall) that acts as an axis of separation, we 
	//! should create one single hole/gap to connect both the spaces it seperates. 
	create_hole(wall, orientation);

	return;
}


//! Method to update the 'config' json file in the format required by ENVIRO
void MAZE::update_enviro_config() {

	//! Read the file through input stream
	std::ifstream inp("config.json");
	json maze;
	inp >> maze;

	//! We will read in and leave the borders unchanged, but clear out rest of the static members in the config
	auto wall_style = maze["statics"][0]["style"];
	auto border0 = maze["statics"][0];
	auto border1 = maze["statics"][1];
	auto border2 = maze["statics"][2];
	auto border3 = maze["statics"][3];	

	//! This is done to make room for a new random maze, i.e a new set of static walls
	maze["statics"].clear();

	//! We leave the borders unchanged for convenience
	maze["statics"].push_back(border0); 
	maze["statics"].push_back(border1);
	maze["statics"].push_back(border2);
	maze["statics"].push_back(border3);

	// reverse(_walls.begin(), _walls.end());

	//! Iterate through the class data member that stores the constructed static agents
	//! Convert to json object and preserve the structure expected by ENVIRO 
	for(auto wall : _walls){
		json wall_temp;
		for(point p : wall){
			wall_temp["shape"].push_back(p.to_json());
		}

		wall_temp["style"] = wall_style;

		maze["statics"].push_back(wall_temp);
	}

	//! Write back to the config file
	std::ofstream out("config.json");
	out << std::setw(4) << maze << std::endl;

	return;
}

int main(){

	//! The border co-ordinates. This is hardcoded for convenience
	point c0(-340, -190), c1(340, -190), c2(340, 190), c3(-340, 190);

	//! Instantiating a new MAZE object with a width of 10. 
	//! Using a lower width value with lower thresholds for 
	//! base recursion cases in sub_maze can lead to arbitrarily complex mazes.
	//! However that would require more design considerations, computations, and
	//! the Robot size will have to get smaller to fit in the small gaps/holes
	MAZE RandomMaze(c0, c1, c2, c3, 10);

	//! Populating the object members with a new randomly generated maze, using the aforementioned 
	//! recursive methods. 
	RandomMaze.generate_random_maze();

	//! Updating the configurations json file so that the generated maze gets rendered on screen
	RandomMaze.update_enviro_config();
	
	return 0;
}
