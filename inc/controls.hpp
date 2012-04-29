#ifndef DEF_CONTROLS
#define DEF_CONTROLS

#include <iostream>
#include <SDL/SDL.h>

class Controls {
public:
    Controls();
    void refresh();
    bool quit;
    bool up;
    bool down;
    bool right;
    bool left;
    bool m;
    bool s;
    
private:
};

#endif
