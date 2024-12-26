#include "FBO.h"
#include "core/GLCore.h"

FBOTex::FBOTex(const FBOSpec& spec) :
    m_width(spec.width),
    m_height(spec.height),
    m_texture(TextureSpecification{
        .width = spec.width,
        .height = spec.height,
        .internalFormat = GL_RGBA32F,
        .dataFormat = GL_RGBA,
    }) {
    // Generate FBO
    GL_CALL(glGenFramebuffers(1, &m_fboId));
    bindFBO();

    // Generate color buffer
    m_texture.bind();

    GL_CALL(glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        m_texture.getTetxureId(), 0
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
    m_texture.unbind();
}

FBOTex::~FBOTex() {
    unbind();
    LOG_CORE_INFO("Deleting FBO: {}", m_fboId);
    GL_CALL(glDeleteRenderbuffers(1, &m_depthBufferId));
    GL_CALL(glDeleteFramebuffers(1, &m_fboId));
}

void FBOTex::bindFBO() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_fboId));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferId));
    GL_CALL(glViewport(0, 0, m_width, m_height));
}

void FBOTex::bindTexture(const unsigned int slot) const {
    m_texture.bind(slot);
}

void FBOTex::bindFBOandTexture(unsigned int slot) const {
    bindFBO();
    bindTexture(slot);
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
