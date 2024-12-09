#pragma once

// FBO with texture & depth attatchment
class FBOTex {
   public:
    FBOTex(unsigned int width, unsigned int height);
    ~FBOTex();

    void bindFBO() const;
    void bindTexture(unsigned int slot) const;
    void unbind() const;
    int readPixel(int x, int y);

    inline unsigned int getTextureId() { return m_textureId; }

   private:
    unsigned int m_fboId;
    unsigned int m_textureId;
    unsigned int m_depthBufferId;
    unsigned int m_width, m_height;
};
