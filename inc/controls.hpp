#ifndef DEF_CONTROLS
#define DEF_CONTROLS

#include <iostream>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "logger.hpp"
#include <map>

typedef enum CONTROL_KEYS {
    CT_RIGHT,CT_LEFT,CT_UP,CT_DOWN,CT_ESCAPE
} CONTROL_KEYS;

class Controls {
public:
    Controls(GLFWwindow *app);
    void refresh();
    bool is_pressed(char c);
    bool is_pressed(CONTROL_KEYS key);
    bool quit_program();
    void key_callback(GLFWwindow *win, int key,int scancode,int action,int mods);
    static Controls *control_ptr;
    
private:
    std::string control_keys_to_str(CONTROL_KEYS key);
    std::string glfw_control_keys_to_str(int key);
    std::map<std::string,bool> keypress;
    GLFWwindow *app;
};

#endif
