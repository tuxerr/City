#include "uniform.hpp"
#include "shader_program.hpp"

Uniform::Uniform(std::string name,Uniform_Type type) : uniform_name(name), type(type) {
}

void Uniform::add_subscriber(bool *uniform_sended,GLuint program_id, bool *state) {
    GLint loc=glGetUniformLocation(program_id,uniform_name.c_str());
    if(loc==-1) {
        std::cout<<"Uniform "<<uniform_name<<" wasn't found in program "<<program_id<<std::endl;
    } else {
        uniform_locations[program_id]=loc;
    }
    *uniform_sended=false;
    Program_Status status = { state, uniform_sended };
    programs_status[program_id]=status;
}

void Uniform::add_texture(Texture **tex,Program *shaderprogram,int index) {
    if(type==UNIFORM_SAMPLER) {
        tex_pointers[tex]=shaderprogram;
        GLuint program_id = shaderprogram->id();
        GLint loc=glGetUniformLocation(program_id,uniform_name.c_str());
        if(loc!=-1) {
            glUseProgram(program_id);
            glUniform1i(loc,index);
            glUseProgram(0);
        } else {
            std::cout<<"Uniform "<<uniform_name<<" wasn't found in program "<<program_id<<std::endl;
        }
    }
}

Uniform_Type Uniform::get_type() {
    return type;
}

void Uniform::set_value(bool val) {
    if(type==UNIFORM_BOOL) {
	bval=val;
    }
    reset_bools();
}

void Uniform::set_value(int val) {
    if(type==UNIFORM_INT) {
	ival=val;
    }
    reset_bools();
}

void Uniform::set_value(float val) {
    if(type==UNIFORM_FLOAT) {
	fval=val;
    }
    reset_bools();
}

void Uniform::set_value(Matrix4 &val) {
    if(type==UNIFORM_MAT4) {
	mval=val;
    }
    reset_bools();
}

void Uniform::set_value(Vec3<float> val) {
    if(type==UNIFORM_FLOAT3) {
	f3val=val;
    }
    reset_bools();
}

void Uniform::set_value(Vec2<float> val) {
    if(type==UNIFORM_FLOAT2) {
	f2val=val;
    }
    reset_bools();
}

void Uniform::set_value(Vec3<int> val) {
    if(type==UNIFORM_INT3) {
	i3val=val;
    }
    reset_bools();
}

void Uniform::set_value(Vec2<int> val) {
    if(type==UNIFORM_INT2) {
	i2val=val;
    }
    reset_bools();
}

void Uniform::set_value(Texture *tex) {
    std::map<Texture**,Program*>::iterator it=tex_pointers.begin();
    for(;it!=tex_pointers.end();it++) {
        *(it->first)=tex;
        (it->second)->bind_texture(tex);
    }
}

void Uniform::reset_bools() {
    std::map<GLuint,Program_Status>::iterator it=programs_status.begin();
    for(;it!=programs_status.end();it++) {
        if( *((it->second).binded) ) {
            send_value(it->first);
        } else {
            *((it->second).uniform_sended)=false;
        }
    }
}

void Uniform::send_value(GLuint program_id) {
    switch(type) {
    case UNIFORM_BOOL:
        if(bval) {
            glUniform1i(uniform_locations[program_id],1);            
        } else {
            glUniform1i(uniform_locations[program_id],0);            
        }
	break;
        
    case UNIFORM_INT:
	glUniform1i(uniform_locations[program_id],ival);
	break;

    case UNIFORM_FLOAT:
	glUniform1f(uniform_locations[program_id],fval);
	break;

    case UNIFORM_FLOAT3:
        glUniform3f(uniform_locations[program_id],f3val.x,f3val.y,f3val.z);
        break;

    case UNIFORM_FLOAT2:
        glUniform2f(uniform_locations[program_id],f2val.x,f2val.y);
        break;

    case UNIFORM_INT3:
        glUniform3i(uniform_locations[program_id],i3val.x,i3val.y,i3val.z);
        break;

    case UNIFORM_INT2:
        glUniform2i(uniform_locations[program_id],i2val.x,i2val.y);
        break;
	
    case UNIFORM_MAT4:
	glUniformMatrix4fv(uniform_locations[program_id],1,GL_TRUE,mval.adress());
	break;

    case UNIFORM_SAMPLER:
        break;
    }
}

std::string Uniform::name() {
    return uniform_name;
}
