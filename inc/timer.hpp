#ifndef DEF_TIMER
#define DEF_TIMER

#include <iostream>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <unistd.h>
#include "logger.hpp"

using namespace std;

#define DEFAULT_TIMER_FPS 40

class Timer {
public:
    Timer(int fps=DEFAULT_TIMER_FPS);
    void wait();
    int get_act_fps();

private:
    int fps;
    int act_fps;
    double old_time;
    double needed_time;
};

#endif
