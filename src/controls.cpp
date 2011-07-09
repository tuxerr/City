#include "controls.h"

Controls::Controls() : quit(false) {
}

void Controls::refresh() {
    SDL_Event event;
    while(SDL_PollEvent(&event)==1) {

        switch(event.window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            quit=true;
            break;
        }

    }
}
