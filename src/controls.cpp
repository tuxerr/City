#include "controls.hpp"

Controls::Controls() : quit(false), up(false), down(false), right(false), left(false), m(false), s(false), d(false), e(false) {
}

void Controls::refresh() {
    SDL_Event event;
    bool keydown=true;
    while(SDL_PollEvent(&event)==1) {

        switch(event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if(event.type==SDL_KEYUP) {
                keydown=false;
            } else {
                keydown=true;
            }

            switch(event.key.keysym.sym) {
            case SDLK_UP:
                up=keydown;
                break;

            case SDLK_DOWN:
                down=keydown;
                break;

            case SDLK_RIGHT:
                right=keydown;
                break;

            case SDLK_LEFT:
                left=keydown;
                break;

            case SDLK_m:
                m=keydown;
                break;

            case SDLK_s:
                s=keydown;
                break;

            case SDLK_d:
                d=keydown;
                break;

            case SDLK_e:
                e=keydown;
                break;
            }
            
            break;

        }

        switch(event.window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            quit=true;
            break;
            
        }

    }
}
