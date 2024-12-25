#pragma once

#include "IndexBuffer.h"
#include "RendererTypes.h"
#include "VertexArray.h"

class Shader;

class Renderer {
public:
    static void clear();
    static void clearDepthBuffer();
    static void draw(
        const VertexArray& vao, const IndexBuffer& ib, const Shader& shader
    );
    static void getViewport(int x, int y, int width, int height);
    static void setDepthTest(bool enabled);
    static unsigned int getMaxTextureSlots();
    static void bindDefaultFramebuffer();
};
