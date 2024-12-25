#pragma once

#include "Texture.h"
#include "../Core/GLCore.h"
#include "../core/Core.h"

struct TextureSpecification {
    unsigned int width, height;
    unsigned int internalFormat = GL_RGBA8, dataFormat = GL_RGBA;
};

class Texture2D : public Texture {
public:
    Texture2D(const std::string& filePath);
    Texture2D(const TextureSpecification& spec);
    ~Texture2D();

    virtual void bind(unsigned int slot = 0) const override;
    virtual void unbind() const override;

    inline unsigned int getWidth() const {
        return m_width;
    }
    inline unsigned int getHeight() const {
        return m_height;
    }
    inline unsigned int getTetxureId() const {
        return m_rendererId;
    }

private:
    unsigned int m_rendererId;
    size_t m_width, m_height, m_BPP;

private:
    void init();
};
