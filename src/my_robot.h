#ifndef __MY_ROBOT_AGENT__H
#define __MY_ROBOT_AGENT__H 

#include "enviro.h"
#include <bits/stdc++.h>

using namespace enviro;

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

class MyRobotController : public Process, public AgentInterface {

    public:
    MyRobotController() : Process(), AgentInterface() {
        _position_init = false;
        _step_size = 4;
    }

    void init(){
        watch("screen_click", [this](Event& e) {
            if(!this->_position_init) {
                _rec_stack = std::stack<std::pair<int, int>>();
                _visited.clear();
                _rec_stack.push(std::make_pair(e.value()["x"], e.value()["y"]));
            }
            e.stop_propagation();
            this->_position_init = true;
        });
    }

    void start() {
    }

    void update() {
        if(!_position_init){
            return;
        }

        if(_rec_stack.empty()) {
            return;
        }

        auto node = _rec_stack.top(); _rec_stack.pop();

        teleport(node.first, node.second, 0.0);

        if(_visited[node] == true){
            return;
        }

        if(outside(x(), y())){
            _position_init = false;
            return;
        }

        node = std::make_pair((int)x(), (int)y());

        _visited[node] = true;

        for(int i=0; i<4; ++i){
            auto d = "RULD"[i];
            auto next = get_next_pos(node, d);
            if(_visited[next]){
                continue;
            }
            if(std::min(sensor_value(2*i), sensor_value(2*i+1)) <= 4+_step_size){
                continue;
            }
            _rec_stack.push(node);
            _rec_stack.push(next);
        }

        return;
    }

    void stop() {
    }

    bool outside(int x, int y){
        if(!_position_init){
            return false;
        }
        return x<=-350 || x>=350 || y<=-200 || y>=200;
    }

    std::pair<int,int> get_next_pos(std::pair<int, int> cur_pos, char dir){
        Direction d(dir, _step_size);
        int new_x = cur_pos.first + d.dx();
        int new_y = cur_pos.second + d.dy();
        return std::make_pair(new_x, new_y);        
    }

    private:
    int _step_size;
    bool _position_init;
    std::map<std::pair<int, int>, bool> _visited;
    std::stack<std::pair<int, int>> _rec_stack;
};

class MyRobot : public Agent {
    public:
    MyRobot(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    MyRobotController c;
};

DECLARE_INTERFACE(MyRobot)

#endif
