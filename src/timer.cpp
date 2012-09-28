#include "timer.hpp"

Timer::Timer(int fps) : clock(), fps(fps) , act_fps(fps) {
    needed_time=1000/fps;
}

void Timer::reset() {
    clock.restart();
}

void Timer::wait() {
    // slows the program to avoid using too much CPU
    int elapsed_time = clock.getElapsedTime().asMilliseconds();
    
    if(elapsed_time<needed_time) {
        // if the new image is too quick
        sf::sleep(sf::milliseconds(needed_time-elapsed_time));
    } else {
        Logger::log(LOG_WARNING)
            <<"Performance issues : program cannot keep up to "<<fps
            <<" fps (act "<<elapsed_time<<" ms or "<<1000/(elapsed_time)
            <<" fps)"<<endl;
    } 

    reset();

    // calculates real FPS
    if(elapsed_time!=0)
        act_fps=1000/(elapsed_time);

}

int Timer::get_act_fps() {
    return act_fps;
}
