#include "display.hpp"

Display::Display(int width,int height,bool fullscreen,bool aa) : 
    current_program(NULL), width(width), height(height), fullscreen(fullscreen), antialiasing(aa)
{
}

Display::~Display() {
    // destructing all shader programs.
    programs.clear();

    std::set<Uniform*>::iterator itu=uniforms.begin();
    for(;itu!=uniforms.end();itu++) {
        delete (*itu);
    }

    std::set<UniformBlock*>::iterator itub=uniformblocks.begin();
    for(;itub!=uniformblocks.end();itub++) {
        (*itub)->destroy();
        delete (*itub);
    }

    delete window; // removes the SFML rendering context
}

void Display::init() {

    sf::ContextSettings winsettings;
    winsettings.depthBits         = 24; // Request a 24 bits depth buffer
    winsettings.antialiasingLevel = 2;  // Request 2 levels of antialiasing
    winsettings.majorVersion = 4;
    winsettings.minorVersion = 0;
    

    if(fullscreen) {
        window = new sf::Window(sf::VideoMode(width,height,32),TITLE,sf::Style::Fullscreen,winsettings);
    } else {
        window = new sf::Window(sf::VideoMode(width,height,32),TITLE,sf::Style::Close,winsettings);
    }

    glewInit();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D_ARRAY);

    glPatchParameteri(GL_PATCH_VERTICES,4); // patches are made up from 4 vertices
}

sf::Window* Display::getWindow() {
    return window;
}

int Display::get_width() {
    return width;
}

int Display::get_height() {
    return height;
}

float Display::get_ratio() {
    return (float)width/(float)height;
}

void Display::viewport(int new_width,int new_height) {
    if(new_width==-1) {
        new_width=width;
    }

    if(new_height==-1) {
        new_height=height;
    }

    glViewport(0,0,new_width,new_height);
}

void Display::new_program(const char *vertex_shader_path,const char *fragment_shader_path,const char *tessellation_control_shader_path,const char *tessellation_evaluator_shader_path,const char *geometry_shader_path,std::string name) {
    // if a program with this name already exists, rewrite it.
    if(programs.find(name)!=programs.end()) {
        programs.erase(programs.find(name));
    }
    
    // creates the new program with both vertex and fragment shaders.
    programs[name].load_shaders(vertex_shader_path,fragment_shader_path,tessellation_control_shader_path,tessellation_evaluator_shader_path,geometry_shader_path);
    Logger::log()<<"Program "<<name<<" was created"<<std::endl;
}

bool Display::has_program(std::string name) {
    if(programs.find(name)==programs.end()) {
        return false;
    } else {
        return true;
    }
}

Program *Display::get_program(std::string name) {
    if(programs.find(name)==programs.end()) {
        return &programs["default"];
    } else {
        return &programs[name];
    }
}

void Display::use_program(std::string name) {
    Program *p = get_program(name);
    if(current_program!=p) {
        if(current_program!=NULL) {
            current_program->unuse();
        }
        p->use();
        current_program=p;
    }
}

Uniform* Display::new_uniform(std::string uniform_name,Uniform_Type type) {
    Uniform *uni=new Uniform(uniform_name,type);
    uniforms.insert(uni);
    return uni;
}

UniformBlock* Display::new_uniformblock(std::string uniformblock_name) {
    int max_uniforms;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS,&max_uniforms);
    if(uniformblocks.size()==(unsigned int)max_uniforms) {
        Logger::log()<<"MAX_UNIFORM_BLOCKS is "<<max_uniforms<<" : you can't create anymore UniformBlocks"<<std::endl;
        return NULL;
    } else {
        UniformBlock *uni=new UniformBlock(uniformblock_name,uniformblocks.size()+1);
        uniformblocks.insert(uni);
        return uni;
    }
}

void Display::link_program_to_uniform(std::string program_name,Uniform *uni) {
    if(programs.find(program_name)!=programs.end()) {
      // subscribe the program to the uniform
        programs[program_name].subscribe_to_uniform(uni);
    } else {
	Logger::log()<<"Program "<<program_name<<" does not exist"<<std::endl;
    }
}

void Display::link_program_to_uniformblock(std::string program_name,UniformBlock *uni) {
    if(programs.find(program_name)!=programs.end()) {
      // subscribe the program to the uniform
        programs[program_name].subscribe_to_uniformblock(uni);
    } else {
	Logger::log()<<"Program "<<program_name<<" does not exist"<<std::endl;
    }
}

void Display::new_draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
}

void Display::refresh() {
    window->display();
}
