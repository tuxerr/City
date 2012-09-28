#include "controls.hpp"

Controls::Controls(sf::Window *app) : quit(false), up(false), down(false), right(false), left(false), m(false), s(false), d(false), e(false), app(app) {
}

void Controls::refresh() {
    sf::Event event;

    while(app->pollEvent(event)) {
        bool keypressed=false;

        switch(event.type) { 

        case sf::Event::Closed:
            quit=true;
            break;

        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
            if(event.type==sf::Event::KeyPressed) {
                keypressed=true;
            }

            switch(event.key.code) {
            case sf::Keyboard::Up :
                up=keypressed;
                break;

            case sf::Keyboard::Down :
                down=keypressed;
                break;

            case sf::Keyboard::Right :
                right=keypressed;
                break;

            case sf::Keyboard::Left :
                left=keypressed;
                break;

            case sf::Keyboard::M :
                m=keypressed;
                break;

            case sf::Keyboard::S :
                s=keypressed;
                break;

            case sf::Keyboard::D :
                d=keypressed;
                break;

            case sf::Keyboard::E :
                e=keypressed;
                break;
                
            default:
                break;
            }
            break;
            
        default:
            break;
            
        }
    }

}
