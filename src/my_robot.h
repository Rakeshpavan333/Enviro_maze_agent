#ifndef __MY_ROBOT_AGENT__H
#define __MY_ROBOT_AGENT__H 

#include "enviro.h"
#include <bits/stdc++.h>

using namespace enviro;

//! A useful class for converting direction to integer co-ordinate steps
class Direction {
    public:
    Direction(char dir, int step_size){
        if (dir == 'R'){
            _dx = step_size; _dy = 0;
        } else if (dir == 'L' ){
            _dx = -step_size; _dy = 0;
        } else if (dir == 'D'){
            _dx = 0; _dy = -step_size;
        } else if (dir == 'U'){
            _dx = 0; _dy = step_size;
        } else {
            throw std::invalid_argument("Invalid Direction");
        }
    }
    int dx() { return _dx; }
    int dy() { return _dy; }

    private: 
    int _dx;
    int _dy;
};

//! Controlling the robot movement. The DFS + Backtracking is implemented as a member function
//! Inherits from Process and AgentInterace
class MyRobotController : public Process, public AgentInterface {

    public:
    //! Constructor assigning step size and a boolean indicating if robot position
    //! has been initialized (which is done through a event handler for screen_click)
    MyRobotController() : Process(), AgentInterface() {
        _position_init = false;
        _step_size = 4;
    }

    //! The required init() method for every process. 
    void init(){
        //! Watching for the screen click event to assign as the starting 
        //! position for the robot 
        //! \param Event reference which contains the clicked screen co-ordinates 
        watch("screen_click", [this](Event& e) {
            //! If the robot has not been assigned a starting position yet 
            //! we assign it now. This is to avoid re-assigning by subsequent clicks,
            //! accidental or otherwise 
            if(!this->_position_init) {
                //! We first initialize the recursion stack as a empty stack.
                //! A stack implements first-in-last-out (FILO) which we will use 
                //! for backtracking the robot position during the DFS case it gets stuck
                _rec_stack = std::stack<std::pair<int, int>>();

                //! A map of visited cells useful for the depth-first-search (DFS)
                _visited.clear();

                //! Pushing the clicked screen co-ordinate as the first element to the stack
                //! This will be the starting position for our robot
                _rec_stack.push(std::make_pair(e.value()["x"], e.value()["y"]));
            }
            e.stop_propagation();
            this->_position_init = true;
        });
    }

    //! Empty!
    void start() {
    }

    void update() {
        //! We only proceed with the update() method if the 
        //! robot has been assigned a starting position (trigger by click)
        if(!_position_init){
            return;
        }

        //! If the recursion stack is empty the robot has exhausted the feasible
        //! search regions and is stuck in a closed section. We end here
        if(_rec_stack.empty()) {
            return;
        }

        //! Pop the top element from the stack. Used for both the DFS and the backtracking
        auto node = _rec_stack.top(); _rec_stack.pop();

        //! Move the robot to the desired node (position)
        teleport(node.first, node.second, 0.0);

        //! If the node was previously visited we stop here. 
        if(_visited[node] == true){
            return;
        }

        //! If the robot has made it outside the grid successfully then we stop and 
        //! wait for another mouse click to assign a new startign point for the bot
        if(outside(x(), y())){
            _position_init = false;
            return;
        }

        node = std::make_pair((int)x(), (int)y());

        _visited[node] = true;

        // A depth-first-search through the neighboring cells  
        for(int i=0; i<4; ++i){
            auto d = "RULD"[i];
            auto next = get_next_pos(node, d);
            if(_visited[next]){
                continue;
            }
            //! We can detect static agents (i.e walls) through using multiple sensors
            if(std::min(sensor_value(2*i), sensor_value(2*i+1)) <= 4+_step_size){
                continue;
            }
            //! We push both the current node and the next node. If the robot exhausts 
            //! search in the sub-graph of 'next' then it would backtrack to current node 
            //! Each node gets visited a maximum of 4 times. Hence the time completxity 
            //! scales linearly with the area of the region (i.e the number of integer co-ordinates). 
            _rec_stack.push(node);
            _rec_stack.push(next);
        }

        return;
    }

    void stop() {
    }

    //! Helper method to check if the robot has successfully made it outside
    //! the maze
    //! \param the position co-ordinates
    bool outside(int x, int y){
        if(!_position_init){
            return false;
        }
        return x<=-350 || x>=350 || y<=-200 || y>=200;
    }

    //! Computing the next position for the robot based on current position and 
    //! intended direction of movement. Uses the Direction class
    //! \param the current position of the robot
    //! \param the intended direction of movement 'U', 'D', 'L', or 'R'
    std::pair<int,int> get_next_pos(std::pair<int, int> cur_pos, char dir){
        Direction d(dir, _step_size);
        int new_x = cur_pos.first + d.dx();
        int new_y = cur_pos.second + d.dy();
        return std::make_pair(new_x, new_y);        
    }

    private:
    int _step_size;
    bool _position_init;
    //! We use map from the standard template library to store which cells have 
    //! already been visited by the robot. This is needed for the DFS
    std::map<std::pair<int, int>, bool> _visited;

    //! A recursion stack the stores the order in which the positions 
    //! were accessed along the DFS. This helps in backtracking.
    std::stack<std::pair<int, int>> _rec_stack;
};

//! Main Robot class that inherits from the Agent class. The controller is a private 
//! data member of this class
class MyRobot : public Agent {
    public:
    //! Constructor used to add the controller process class to the robot 
    MyRobot(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    MyRobotController c;
};

DECLARE_INTERFACE(MyRobot)

#endif
