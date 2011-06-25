#include "controls.h"

Controls::Controls() : quit(false) {
}

void Controls::refresh() {
    SDL_Event event;
    while(SDL_PollEvent(&event)==1) {

        switch(event.type) {
        case SDL_QUIT:
            quit=true;
            break;
        }

    }
}
