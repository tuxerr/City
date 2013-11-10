#include "fbo.hpp"

FBO::FBO() {
    glGenFramebuffers(1,&fbo_id);
}

FBO::~FBO() {
    glDeleteFramebuffers(1,&fbo_id);
}

bool FBO::iscomplete() {
    bool res;
    bind();
    GLenum completion = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(completion != GL_FRAMEBUFFER_COMPLETE) {
        switch(completion) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout<<"FBO attachment incomplete"<<std::endl;
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout<<"Missing a FBO attachment (at lest one required)"<<std::endl;
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout<<"FBO draw buffer incomplete"<<std::endl;
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout<<"FBO read buffer incomplete"<<std::endl;
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout<<"FBO multisampler incomplete"<<std::endl;
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            std::cout<<"FBO incomplete layer targets"<<std::endl;
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout<<"FBO unsupported"<<std::endl;
            break;

        case GL_FRAMEBUFFER_UNDEFINED:
            std::cout<<"FBO undefined"<<std::endl;
            break;
           
        default:
            break;
        }

        res =  false;
    } else {
        res = true;
    }

    unbind();
    return res;
}

void FBO::bind(FBO_Type type) {
    switch(type) {
    case FBO_DRAW:
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo_id);
        break;

    case FBO_READ:
        glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo_id);
        break;

    case FBO_BOTH:
        glBindFramebuffer(GL_FRAMEBUFFER,fbo_id);
        break;
    }

}

void FBO::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void FBO::attach_texture(Texture *tex,FBO_Attachment attachpoint,int array_layer) {
    bind();
    tex->bind();
    switch(tex->get_gl_texture_type()) {
    case GL_TEXTURE_2D:
        if(array_layer!=-1) {
            std::cout<<"You shouldn't specify a layer ("<<array_layer<<") for a 2-dimensional texture"<<std::endl;
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER,attachpoint,GL_TEXTURE_2D,tex->id(),0);
        break;

    case GL_TEXTURE_2D_ARRAY:
        if(array_layer==-1) {
            std::cout<<"You should specify a layer for the FBO binding of a 3-dimensional texture"<<std::endl;
        } else {
            glFramebufferTexture3D(GL_FRAMEBUFFER,attachpoint,GL_TEXTURE_2D_ARRAY,tex->id(),0,array_layer);
        }

        break;
    }

    tex->unbind();
    unbind();
}

