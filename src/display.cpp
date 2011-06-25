#include "display.h"

Display::Display(int width,int height,bool fullscreen) : 
    width(width), height(height), fullscreen(fullscreen)
{
}

Display::~Display() {
    // destructing all shader programs.
    programs.clear();

    std::vector<Uniform*>::iterator it=uniforms.begin();
    for(;it!=uniforms.end();it++) {
        delete (*it);
    }

    SDL_Quit();
}

void Display::init() {
    SDL_Init(SDL_INIT_VIDEO);
    
    if(fullscreen) {
        SDL_SetVideoMode(width,height,32,SDL_OPENGL | SDL_FULLSCREEN);
    } else {
        SDL_SetVideoMode(width,height,32,SDL_OPENGL);
    }

    SDL_WM_SetCaption(TITLE,NULL);
    
    glewInit();
    
    glEnable(GL_DEPTH_TEST);
}

void Display::perspective(float angle,float near,float far,Uniform *perspective) {
    Matrix4 tmp;
    float f = 1.0 / tan(angle * M_PI / 360); 
    tmp.val[0]=f/((float)width/height);
    tmp.val[5]=f;
    tmp.val[10]=(far + near)/(near-far);
    tmp.val[11]=2*far*near/(near-far);
    tmp.val[14]=-1;

    perspective->set_value(tmp);
}

void Display::new_program(const char *vertex_shader_path,const char *fragment_shader_path,std::string name) {
    // if a program with this name already exists, rewrite it.
    if(programs.find(name)!=programs.end()) {
        programs.erase(programs.find(name));
    }
    
    // creates the new program with both vertex and fragment shaders.
    programs[name].load_shaders(vertex_shader_path,fragment_shader_path);
}

Uniform* Display::new_uniform(std::string uniform_name,Uniform_Type type) {
    Uniform *uni=new Uniform(uniform_name,type);
    uniforms.push_back(uni);
    return uni;
}

void Display::link_program_to_uniform(std::string program_name,Uniform *uni) {
    if(programs.find(program_name)!=programs.end()) {
      // subscribe the program to the uniform
        programs[program_name].subscribe_to_uniform(uni);
    } else {
	std::cout<<"Program "<<program_name<<" does not exist"<<std::endl;
    }
}

void Display::new_draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::refresh() {
    SDL_GL_SwapBuffers();
}

void Display::set_modelview_uniform(Uniform *uni) {
    modelview=uni;
}

void Display::draw_object(Object &o) {
    if(o.enable_draw()) {
        std::string program=o.get_program();
        // if the object's shader doesn't exist, use default one.
        if(programs.find(program)==programs.end()) {
            program="default";
            std::cout<<"Object program "<<program<<" does not exist : using default"<<std::endl;
        }

        modelview->set_value(o.modelview_matrix());

        programs[program].use();

        o.draw();

        programs[program].unuse();
    }
}

void Display::draw_scene(Scene &sce) {
    // drawing all objects
    std::map<int,Object*>::iterator it;
    for(it=(sce.objects).begin();it!=(sce.objects).end();it++) {
        draw_object(*(it->second));
    }
}
