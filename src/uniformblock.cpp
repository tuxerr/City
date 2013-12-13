#include "uniformblock.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

UniformBlock::UniformBlock(std::string name,int attachpoint) : 
    complete_name(name), attach_name(name), size(-1), iscreated(false), attachpoint(attachpoint)
{
   /* if(complete_name.find('[')==std::string::npos) {
        complete_name+="_";
    } else {
        complete_name.insert(complete_name.find('['),"_");
    } */
}

UniformBlock::UniformBlock(std::string name,std::string attach_name, int attachpoint) :
complete_name(name), attach_name(attach_name), size(-1), iscreated(false), attachpoint(attachpoint)
{
    /* if(complete_name.find('[')==std::string::npos) {
     complete_name+="_";
     } else {
     complete_name.insert(complete_name.find('['),"_");
     } */
}

void UniformBlock::create() {
    if(size==-1) {
        Logger::log()<<"Invalid UBO size"<<std::endl;
        return;
    } else {
        glGenBuffers(1,&ubo);
        glBindBuffer(GL_UNIFORM_BUFFER,ubo);
        glBufferData(GL_UNIFORM_BUFFER,size,NULL,GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER,0);
        iscreated=true;
    }
}

void UniformBlock::destroy() {
    if(iscreated) {
        glDeleteBuffers(1,&ubo);
        iscreated=false;
    }
}

int UniformBlock::get_attach_point() {
    return attachpoint;
}

void UniformBlock::bind_to_attach_point(GLuint program_id) {
    // loc is a good value, since it has already been computed in the program stage of this function
    this->program_id=program_id;
    ubo_loc_in_program = glGetUniformBlockIndex(program_id,attach_name.c_str());
    int ubo_size=-1;
    glGetActiveUniformBlockiv(program_id,ubo_loc_in_program,GL_UNIFORM_BLOCK_DATA_SIZE,&ubo_size);

    if(!iscreated) {
        size=ubo_size;
        create();
    } else if(size!=ubo_size) {
        Logger::log(LOG_ERROR)<<"Size mismatch for UBO "<<get_ubo()<<" : UBO size is "<<size
                 <<" and needed size is "<<ubo_size<<std::endl;
    }
    glBindBuffer(GL_UNIFORM_BUFFER,ubo);
    glBindBufferRange(GL_UNIFORM_BUFFER,attachpoint,ubo,0,size);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}

std::string UniformBlock::get_name() {
    return complete_name;
}

std::string UniformBlock::get_attach_name() {
    return attach_name;
}


GLuint UniformBlock::get_ubo() {
    return ubo;
}

GLint UniformBlock::get_value_from_pname(std::string sub_name,GLenum pname) {
    GLuint indice;
    std::string real_name=complete_name;
    real_name+=".";
    real_name+=sub_name;
    
    const char *str_ptr = real_name.c_str();

    glGetUniformIndices(program_id,1,&str_ptr,&indice);
    if(indice==GL_INVALID_INDEX) {
        Logger::log(LOG_ERROR)<<"Subuniform "<<real_name<<" doesn't exist in program "<<program_id<<std::endl;
        return -1;
    } else {
        GLint value;
        glGetActiveUniformsiv(program_id,1,&indice,pname,&value);
        return value;
    }
}

GLint UniformBlock::get_offset(std::string sub_name) {
    if(offsets.find(sub_name)==offsets.end()) {
        GLint offset= get_value_from_pname(sub_name,GL_UNIFORM_OFFSET);

        if(offset==-1) {
            Logger::log(LOG_ERROR)<<sub_name<<" has a negative offset in the uniformblock "<<complete_name<<std::endl;            
        } else {
            std::map<std::string,GLint>::iterator it=offsets.begin();
            for(;it!=offsets.end();it++) {
                if(offset==it->second) {
                    Logger::log(LOG_WARNING)<<sub_name<<" has the same offset as "<<it->first<<" in uniformblock "<<complete_name<<std::endl;
                }
            }
        }

        offsets[sub_name]=offset;
        return offset;
    } else {
        return offsets[sub_name];
    }
}

void UniformBlock::bind() {
    if(!isbinded) {
        glBindBuffer(GL_UNIFORM_BUFFER,ubo);
        isbinded=true;
    }
}

void UniformBlock::unbind() {
    if(isbinded) {
        glBindBuffer(GL_UNIFORM_BUFFER,0);
        isbinded=false;
    }
}

void UniformBlock::set_value(bool val,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        int realval;
        if(val) {
            realval=1;
        } else {
            realval=0;
        }
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(realval),&realval);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(realval),&realval);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
        
    }
}

void UniformBlock::set_value(int val,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(val),&val);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(val),&val);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
    }
}

void UniformBlock::set_value(float val,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(val),&val);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(val),&val);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
    }
}

void UniformBlock::set_value(Vec3<float> &v3 ,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(v3),&v3);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(v3),&v3);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
        
    }
}

void UniformBlock::set_value(Vec3<int> &i3 ,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(i3),&i3);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(i3),&i3);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
    }
}

void UniformBlock::set_value(Vec2<float> &v2 ,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(v2),&v2);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(v2),&v2);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
    }
}

void UniformBlock::set_value(Vec2<int> &i2 ,std::string sub_name) {
    int offset = get_offset(sub_name);
    if(offset!=-1) {
        if(isbinded) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(i2),&i2);
        } else {
            glBindBuffer(GL_UNIFORM_BUFFER,ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,offset,sizeof(i2),&i2);
            glBindBuffer(GL_UNIFORM_BUFFER,0);
        }
    }
}

void UniformBlock::set_value(Matrix4 &mat,std::string sub_name) {
    Matrix4 m = mat;
     m.transpose();

    int offset = get_offset(sub_name);
    if(offset!=-1) {
        //GLint mat_stride;
        //if(matrix_strides.find(sub_name)==matrix_strides.end()) {
        //    mat_stride = get_value_from_pname(sub_name,GL_UNIFORM_MATRIX_STRIDE);
        //    matrix_strides[sub_name] = mat_stride;
        //} else {
        //    mat_stride = matrix_strides[sub_name];
       //}
        
        int mat_stride=0;

        glBindBuffer(GL_UNIFORM_BUFFER,ubo);
        for(int i=0;i<4;i++) {
            glBufferSubData(GL_UNIFORM_BUFFER,offset+(sizeof(float)*4+mat_stride)*i,
                            sizeof(float)*4,&(m.val[i*4]));
        }
        glBindBuffer(GL_UNIFORM_BUFFER,0);
    }
}

void UniformBlock::print_contents() {
    glBindBuffer(GL_UNIFORM_BUFFER,ubo);
    float *map_ubo = (float*)glMapBuffer(GL_UNIFORM_BUFFER,GL_READ_ONLY);
    if(map_ubo==NULL) {
        Logger::log()<<"UBO contents are not accessible"<<std::endl;
    } else {
        for(int i=0;i<size/4;i++) {
            Logger::log()<<i<<" : "<<map_ubo[i]<<std::endl;
        }
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}
