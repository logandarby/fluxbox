#pragma once

#include "IndexBuffer.h"
#include "Model/Model.h"
#include "RendererTypes.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer {
   public:
    static void clear();
    static void draw(const VertexArray& vao, const IndexBuffer& ib, const Shader& shader);
    static void getViewport(int x, int y, int width, int height);
    static void setDepthTest(bool enabled);
    static unsigned int getMaxTextureSlots();
};
