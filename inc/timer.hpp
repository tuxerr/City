#ifndef DEF_TIMER
#define DEF_TIMER

#include <iostream>
#include <SFML/System.hpp>
#include "logger.hpp"

using namespace std;

#define DEFAULT_TIMER_FPS 40

class Timer {
public:
    Timer(int fps=DEFAULT_TIMER_FPS);
    void reset();
    void wait();
    int get_act_fps();

private:
    sf::Clock clock;
    int fps;
    int act_fps;
    int needed_time;
    
};

#endif
