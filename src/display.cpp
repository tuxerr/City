#include "display.hpp"

Display::Display(int width,int height,bool fullscreen,bool aa) : 
    width(width), height(height), fullscreen(fullscreen), antialiasing(aa)
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

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(screen);
    SDL_Quit();
}

void Display::init() {

    if(SDL_Init(SDL_INIT_VIDEO)<0) {
        std::cout<<"Error during SDL init : "<<SDL_GetError()<<std::endl;
        exit(1);
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if(antialiasing) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    }

    if(fullscreen) {
        screen=SDL_CreateWindow(TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                width,height,SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | 
                                SDL_WINDOW_FULLSCREEN);
    } else {
        screen=SDL_CreateWindow(TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                width,height,SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    }

    glcontext = SDL_GL_CreateContext(screen);

//    SDL_GL_SetSwapInterval(1);

    glewInit();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D_ARRAY);
}

int Display::get_width() {
    return width;
}

int Display::get_height() {
    return height;
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

void Display::new_program(const char *vertex_shader_path,const char *fragment_shader_path,std::string name) {
    // if a program with this name already exists, rewrite it.
    if(programs.find(name)!=programs.end()) {
        programs.erase(programs.find(name));
    }
    
    // creates the new program with both vertex and fragment shaders.
    programs[name].load_shaders(vertex_shader_path,fragment_shader_path);
    std::cout<<"Program "<<name<<" was created"<<std::endl;
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

Uniform* Display::new_uniform(std::string uniform_name,Uniform_Type type) {
    Uniform *uni=new Uniform(uniform_name,type);
    uniforms.insert(uni);
    return uni;
}

UniformBlock* Display::new_uniformblock(std::string uniformblock_name) {
    int max_uniforms;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS,&max_uniforms);
    if(uniformblocks.size()==(unsigned int)max_uniforms) {
        std::cout<<"MAX_UNIFORM_BLOCKS is "<<max_uniforms<<" : you can't create anymore UniformBlocks"<<std::endl;
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
	std::cout<<"Program "<<program_name<<" does not exist"<<std::endl;
    }
}

void Display::link_program_to_uniformblock(std::string program_name,UniformBlock *uni) {
    if(programs.find(program_name)!=programs.end()) {
      // subscribe the program to the uniform
        programs[program_name].subscribe_to_uniformblock(uni);
    } else {
	std::cout<<"Program "<<program_name<<" does not exist"<<std::endl;
    }
}

void Display::new_draw() {
    glClear(GL_COLOR_BUFFER_BIT); 
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Display::refresh() {
    SDL_GL_SwapWindow(screen);
}
