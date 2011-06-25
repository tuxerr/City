#include "uniform.h"

Uniform::Uniform(std::string name,Uniform_Type type) : uniform_name(name), type(type) {
}

void Uniform::add_subscriber(bool *sub,GLuint program_id) {
    GLint loc=glGetUniformLocation(program_id,uniform_name.c_str());
    if(loc==-1) {
        std::cout<<"Uniform "<<uniform_name<<" wasn't found in program "<<program_id<<std::endl;
    } else {
        uniform_locations[program_id]=loc;
    }
    sent_uniforms.push_back(sub);
}

void Uniform::set_value(int val) {
    if(type==UNIFORM_INT) {
	ival=val;
    }
    reset_bools();
}

void Uniform::set_value(float val) {
    if(type==UNIFORM_FLOAT) {
	ival=val;
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

void Uniform::reset_bools() {
    std::vector<bool*>::iterator it=sent_uniforms.begin();
    for(;it!=sent_uniforms.end();it++) {
        *(*it)=false;
    }
}

void Uniform::send_value(GLuint program_id) {
    switch(type) {
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
    }
}

std::string Uniform::name() {
    return uniform_name;
}
