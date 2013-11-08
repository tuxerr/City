#include "controls.hpp"

Controls* Controls::control_ptr;

void keycallback(GLFWwindow *win, int key,int scancode,int action,int mods) {
    Controls::control_ptr->key_callback(win,key,scancode,action,mods);
}

Controls::Controls(GLFWwindow *app) : app(app) {
    glfwSetKeyCallback(app, keycallback);
    Controls::control_ptr=this;
}

std::string Controls::control_keys_to_str(CONTROL_KEYS key) {
    switch(key) {
        case CT_RIGHT:
            return "RIGHT";
        case CT_LEFT:
            return "LEFT";
        case CT_DOWN:
            return "DOWN";
        case CT_UP:
            return "UP";
        case CT_ESCAPE:
            return "ESCAPE";
    }
}

std::string Controls::glfw_control_keys_to_str(int key) {
    if(key>=GLFW_KEY_A && key<=GLFW_KEY_Z) {
        char c=(char)key;
        std::string str(1,c);
        return str;
    } else {
        switch(key) {
            case GLFW_KEY_RIGHT:
                return "RIGHT";
            case GLFW_KEY_LEFT:
                return "LEFT";
            case GLFW_KEY_DOWN:
                return "DOWN";
            case GLFW_KEY_UP:
                return "UP";
            case GLFW_KEY_ESCAPE:
                return "ESCAPE";
            default:
                return "UNSUPPORTED";
        }
    }
}

bool Controls::is_pressed(char c) {
    std::string str(1,c);
    std::map<std::string,bool>::const_iterator it = keypress.find(str);
    if(it!=keypress.end()) {
        return keypress[str];
    } else {
        return false;
    }
}


bool Controls::is_pressed(CONTROL_KEYS key) {
    std::string str = control_keys_to_str(key);
    std::map<std::string,bool>::const_iterator it = keypress.find(str);
    if(it!=keypress.end()) {
        return keypress[str];
    } else {
        return false;
    }
}

bool Controls::quit_program() {
    return (is_pressed(CT_ESCAPE) || glfwWindowShouldClose(app));
}

void Controls::key_callback(GLFWwindow *win, int key,int scancode,int action,int mods) {
    std::string entry = glfw_control_keys_to_str(key);
    if(action==GLFW_PRESS) {
        keypress[entry]=true;
    } else {
        keypress[entry]=false;
    }
}

void Controls::refresh() {
    glfwPollEvents();
    
    /*while(app->pollEvent(event)) {
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
    }*/

}
