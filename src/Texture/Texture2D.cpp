#include "Texture2D.h"
#include "../core/GLCore.h"
#include "../core/Image.h"

Texture2D::Texture2D(const std::string& filePath) :
    Texture{}, m_rendererId(0), m_width(0), m_height(0), m_BPP(0) {
    Image image(filePath, 4);
    const auto& imageData = image.getImageData();
    m_width = imageData.width;
    m_height = imageData.height;

    init();

    GL_CALL(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, imageData.imageData.get()
    ));
    unbind();
}

Texture2D::Texture2D(const TextureSpecification& spec) :
    m_width{spec.width}, m_height{spec.height} {
    init();
    std::vector<GLubyte> data(m_width * m_height * 4, 0);
    GL_CALL(glTexImage2D(
        GL_TEXTURE_2D, 0, spec.internalFormat, m_width, m_height, 0,
        spec.dataFormat, GL_UNSIGNED_BYTE, data.data()
    ));
}

void Texture2D::init() {
    GL_CALL(glGenTextures(1, &m_rendererId));
    LOG_CORE_INFO("texture renderer id {}", m_rendererId);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_rendererId));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
    );
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
}

Texture2D::~Texture2D() {
    unbind();
    LOG_CORE_INFO("Deleting texture {}", m_rendererId);
    glDeleteTextures(1, &m_rendererId);
}

void Texture2D::bind(const unsigned int slot) const {
    CORE_ASSERT(slot < MAX_TEXTURE_SLOTS);
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_rendererId))
}

void Texture2D::unbind() const {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
