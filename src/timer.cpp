#include "timer.hpp"

Timer::Timer(int fps) : fps(fps) , act_fps(fps) {
}

void Timer::init() {
    // inits SDL timer system
    SDL_InitSubSystem(SDL_INIT_TIMER);
    reset();
}

void Timer::reset() {
    last_time=SDL_GetTicks();
}

void Timer::wait() {
    // slows the program to avoid using too much CPU
    int act_time=SDL_GetTicks();
    int needed_time=1000/fps;

    if(act_time-last_time<needed_time) {
        // if the new image is too quick
        SDL_Delay(needed_time-act_time+last_time);
    } else {
        cout<<"Performance issues : program cannot keep up to "<<fps
            <<" fps (act "<<act_time-last_time<<" ms or "<<act_fps
            <<" fps)"<<endl;
    } 

    act_time=SDL_GetTicks();

    // calculates real FPS
    if(act_time!=last_time)
        act_fps=1000/(act_time-last_time);

    last_time=act_time;
    
}

int Timer::get_act_fps() {
    return act_fps;
}
