#ifndef DEF_CONTROLS
#define DEF_CONTROLS

#include <iostream>
#include <SFML/Window.hpp>

class Controls {
public:
    Controls(sf::Window *app);
    void refresh();
    bool quit;
    bool up;
    bool down;
    bool right;
    bool left;
    bool m;
    bool s;
    bool d;
    bool e;
    
private:
    sf::Window *app;
};

#endif
