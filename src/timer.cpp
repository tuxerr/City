#include "timer.hpp"

Timer::Timer(int fps) : fps(fps) , act_fps(fps) {
    needed_time=1./fps;
    old_time = glfwGetTime();
}

void Timer::wait() {
    // slows the program to avoid using too much CPU
    double time = glfwGetTime();
    double elapsed = time-old_time;
    Logger::log()<<"w8"<<std::endl;
    if(elapsed<needed_time) {
        // if the new image is too quick
        usleep(int((needed_time-elapsed)*1000000));
    } else {
        Logger::log(LOG_WARNING)
            <<"Performance issues : program cannot keep up to "<<fps
            <<" fps (act "<<elapsed<<" s or "<<1/(elapsed)
            <<" fps)"<<endl;
    }

    // calculates real FPS
    if(elapsed!=0)
        act_fps=1/(elapsed);
    old_time=glfwGetTime();

}

int Timer::get_act_fps() {
    return act_fps;
}
