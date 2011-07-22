#include "fbo.h"

FBO::FBO() {
    glGenFramebuffers(1,&fbo_id);
}

FBO::~FBO() {
    glDeleteFramebuffers(1,&fbo_id);
}

bool FBO::iscomplete() {
    bind();
    GLenum completion = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(completion != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    } else {
        return true;
    }
    unbind();
}

void FBO::bind(FBO_Type type) {
    glBindFramebuffer(type,fbo_id);
}

void FBO::unbind() {
    glBindFramebuffer(FBO_BOTH,0);
}

void FBO::attach_texture(Texture *tex,FBO_Attachment attachpoint) {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER,attachpoint,GL_TEXTURE_2D,tex->id(),0);
    unbind();
}

