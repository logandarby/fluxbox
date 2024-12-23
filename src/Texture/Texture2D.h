#pragma once

#include "Texture.h"
#include "../core/Core.h"

class Texture2D : public Texture {
public:
    Texture2D(const std::string& filePath);
    Texture2D(const unsigned int width, const unsigned int height);
    ~Texture2D();

    virtual void bind(unsigned int slot = 0) const override;
    virtual void unbind() const override;

    inline unsigned int getWidth() const {
        return m_width;
    }
    inline unsigned int getHeight() const {
        return m_height;
    }

private:
    unsigned int m_rendererId;
    size_t m_width, m_height, m_BPP;

private:
    void init();
};
