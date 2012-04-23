#ifndef DEF_TIMER
#define DEF_TIMER

#include <iostream>
#include <SDL/SDL.h>

using namespace std;

#define DEFAULT_TIMER_FPS 40

class Timer {
public:
    Timer(int fps=DEFAULT_TIMER_FPS);
    void init();
    void reset();
    void wait();
    int get_act_fps();

private:
    int fps;
    int last_time;
    int act_fps;
};

#endif
