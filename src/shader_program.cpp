#include "shader_program.hpp"

Program::Program() : binded(false) { 
}

Program::~Program() {
    glDeleteProgram(program_id);
}

void Program::load_shaders(const char *vertex_shader_path,const char *fragment_shader_path,const char *tessellation_control_shader_path,const char *tessellation_evaluator_shader_path,const char *geometry_shader_path) {

    program_id=glCreateProgram();

    if(vertex_shader_path!=NULL) {
        GLuint vertex_shader=compile_shader(vertex_shader_path,GL_VERTEX_SHADER);
        if(vertex_shader!=0) {
            glAttachShader(program_id,vertex_shader);
        }
        glDeleteShader(vertex_shader);
    }

    if(fragment_shader_path!=NULL) {
        GLuint fragment_shader=compile_shader(fragment_shader_path,GL_FRAGMENT_SHADER);
        if(fragment_shader!=0) {
            glAttachShader(program_id,fragment_shader);
        }
        glDeleteShader(fragment_shader);
    }

    if(tessellation_control_shader_path!=NULL) {
        GLuint tesscontrol_shader=compile_shader(tessellation_control_shader_path, GL_TESS_CONTROL_SHADER);
        if(tesscontrol_shader!=0) {
            glAttachShader(program_id,tesscontrol_shader);
        }
        glDeleteShader(tesscontrol_shader);
    }

    if(tessellation_evaluator_shader_path!=NULL) {
        GLuint tesseval_shader=compile_shader(tessellation_evaluator_shader_path,GL_TESS_EVALUATION_SHADER);
        if(tesseval_shader!=0) {
            glAttachShader(program_id,tesseval_shader);
        }
        glDeleteShader(tesseval_shader);
    }

    if(geometry_shader_path!=NULL) {
        GLuint geometry_shader=compile_shader(geometry_shader_path,GL_GEOMETRY_SHADER);
        if(geometry_shader!=0) {
            glAttachShader(program_id,geometry_shader);
        }
        glDeleteShader(geometry_shader);
    }
    
    // linking attributes
    glBindAttribLocation(program_id,SHADER_VERTEX_ATTRIB,"in_Vertex");
    glBindAttribLocation(program_id,SHADER_COLOR_ATTRIB,"in_Color");
    glBindAttribLocation(program_id,SHADER_TEXTURE_ATTRIB,"in_TexCoord");
    glBindAttribLocation(program_id,SHADER_NORMAL_ATTRIB,"in_Normal");

    glLinkProgram(program_id);

    GLint link_status = GL_TRUE;
    glGetProgramiv(program_id,GL_LINK_STATUS,&link_status);

    if(link_status != GL_TRUE) {
        GLsizei logsize = 0;
        glGetProgramiv(program_id,GL_INFO_LOG_LENGTH,&logsize);
            
        char *log=new char[logsize+1];
        memset(log,'\0',logsize+1);
        glGetProgramInfoLog(program_id,logsize,&logsize,log);
        std::cout<<"Program linking failed : \n"<<log<<std::endl;
        delete log;
    }

}

GLuint Program::compile_shader(const char *path,GLenum shader_type) {
    GLuint shader=0;
    if(shader_type==GL_FRAGMENT_SHADER || shader_type==GL_VERTEX_SHADER || shader_type==GL_TESS_EVALUATION_SHADER || shader_type==GL_TESS_CONTROL_SHADER || shader_type==GL_GEOMETRY_SHADER) {
        shader = glCreateShader(shader_type);
    } else {
        std::cout<<"Wrong shader type"<<std::endl;
    }

    FILE *fp = NULL;
    fp = fopen(path,"r");
    if(fp!=NULL) {
        fseek(fp,0,SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        GLchar *src=NULL;
        src=new char[size+1];

        for(int i=0;i<size;i++) {
            src[i]=fgetc(fp);
        }
        src[size]='\0';
        fclose(fp);

        // loading the shader source code
        glShaderSource(shader,1,(const GLchar**)&src,NULL);

        // compiling shader
        glCompileShader(shader);

        delete[] src;
        GLint compile_status = GL_TRUE;
        
        glGetShaderiv(shader,GL_COMPILE_STATUS,&compile_status);

        if(compile_status != GL_TRUE) {
            // an error occured
            GLsizei logsize = 0;
            glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&logsize);
            
            char *log=new char[logsize+1];
            memset(log,'\0',logsize+1);
            glGetShaderInfoLog(shader,logsize,&logsize,log);
            std::cout<<"Shader "<<path<<" compilation failed : \n"<<log<<std::endl;
            delete[] log;
            glDeleteShader(shader);
            return 0;

        } 
        
    } else {
        std::cout<<"File "<<path<<" couldn't be opened"<<std::endl;
    }
    return shader;
}

void Program::subscribe_to_uniform(Uniform *uni) {
    if(uni->get_type()==UNIFORM_SAMPLER) {
        int max_size;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&max_size);
        if((int)textures.size()==max_size) {
            std::cout<<max_size<<" Textures are already binded to program "<<id()<<std::endl;
        } else {
            textures.push_back(NULL);
            uni->add_texture(&textures.back(),this,int(textures.size())-1);
        }
    } else {
        uniforms[uni]=false;
        uni->add_subscriber(&(uniforms[uni]),program_id,&binded);
    }
}

void Program::subscribe_to_uniformblock(UniformBlock *uni) {
    GLuint loc = glGetUniformBlockIndex(id(),uni->get_attach_name().c_str());
    if(loc==GL_INVALID_INDEX) {
        std::cout<<"Uniform "<<uni->get_attach_name()<<" does not exist in program "<<id()<<std::endl;
    } else {
        uni->bind_to_attach_point(id());      
        glUniformBlockBinding(id(), loc, uni->get_attach_point());
        /*if(glGetError()==GL_INVALID_VALUE) {
            std::cout<<"Error while binding block uniform "<<uni->get_name()<<"(program "<<id()
                     <<") to attach point "<<uni->get_attach_point()<<" with location "<<loc<<std::endl;
        } */
    }
}

void Program::use() {
    if(!binded) {
        std::list<Texture*>::iterator it = textures.begin();
        int tex_index=0;
        for(;it!=textures.end();it++) {
            if( (*it) != NULL) {
                (*it)->bind(tex_index);
            }
            tex_index++;
        }

        glUseProgram(program_id);

        std::map<Uniform*,bool>::iterator uni_it=uniforms.begin();
        for(;uni_it!=uniforms.end();uni_it++) {
            // if the uniform has been modified since the last time
            if(uni_it->second==false) {
                (uni_it->first)->send_value(id());
                uni_it->second=true;
            }
        }
    
        binded=true;
    }
}


void Program::bind_texture(Texture* tex) {
    // function is used for texture hotswapping while the same program is being used. if the program is nob binded, his texture array will anyway be changed by the set_value Uniform function and texture will be changed when the program will be used.

    if(binded) {
    
        std::list<Texture*>::iterator it = textures.begin();
        int tex_index=0;
        for(;it!=textures.end();it++) {
            if( (*it) == tex ) {
                tex->bind(tex_index);
                break;
            }
            tex_index++;
        }

    }
}

void Program::unuse() {
    if(binded) {
        binded=false;
    }
}

bool Program::isBinded() {
    return binded;
}

GLuint Program::id() {
    return program_id;
}

