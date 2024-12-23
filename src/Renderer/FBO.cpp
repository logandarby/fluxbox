#include "FBO.h"
#include "core/GLCore.h"

FBOTex::FBOTex(const FBOSpec& spec) :
    m_width(spec.width), m_height(spec.height) {
    // Generate FBO
    GL_CALL(glGenFramebuffers(1, &m_fboId));
    bindFBO();

    // Generate color buffer
    GL_CALL(glGenTextures(1, &m_textureId));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));

    GL_CALL(glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL::getMagFilter(spec.minSampler)
    ));
    GL_CALL(glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL::getMagFilter(spec.magSampler)
    ));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
    );
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
    );

    GL_CALL(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, m_width, m_height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, nullptr
    ));

    GL_CALL(glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureId, 0
    ));

    // Generate depth buffer
    GL_CALL(glGenRenderbuffers(1, &m_depthBufferId));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferId));
    GL_CALL(glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height
    ));
    // Bind depth renderbuffer
    GL_CALL(glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
        m_depthBufferId
    ));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_GL_ERR("Framebuffer not complete");
    }

    unbind();
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

FBOTex::~FBOTex() {
    unbind();
    GL_CALL(glDeleteRenderbuffers(1, &m_depthBufferId));
    GL_CALL(glDeleteFramebuffers(1, &m_fboId));
    GL_CALL(glDeleteTextures(1, &m_textureId));
}

void FBOTex::bindFBO() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_fboId));
    GL_CALL(glViewport(0, 0, m_width, m_height));
}

void FBOTex::bindTexture(const unsigned int slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));
}

void FBOTex::unbind() const {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FBOTex::setViewport() const {
    GL_CALL(glViewport(0, 0, m_width, m_height));
}

int FBOTex::readPixel(int x, int y) {
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}
