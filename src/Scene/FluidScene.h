#pragma once

#include "Scene.h"
#include "Core/Core.h"
#include "Model/RectangleModel.h"
#include "Renderer/FBO.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

static const int FRAMEBUFFER_SLOT = 1;

class FluidScene : public Scene {
public:
    FluidScene(const ApplicationSpec& spec) :
        Scene("FluxLab"), m_spec{ spec } {};

    virtual void onInit() override {
        m_fboTex.bindTexture(FRAMEBUFFER_SLOT);
    };
    virtual void onDetach() override {};
    virtual void onRender(const float timestep) override {
        m_fboTex.bindFBO();
        Renderer::clear();

        // Render stuff

        Renderer::bindDefaultFramebuffer();
        Renderer::draw(m_rectVAO, m_rectIb, m_framebufferShader);
    };
    virtual void onEvent(Event& event) override {};

private:
    ApplicationSpec m_spec;
    RectangleModel2D m_screenQuad;
    VertexArray m_rectVAO = VertexArray::withBuffer(
        std::make_shared<VertexBuffer>(
            m_screenQuad.m_points.data(), sizeof(m_screenQuad.m_points)
        ),
        std::make_shared<BufferLayout>(
            BufferLayout().withLayout<PositionWithTexture2D>()
        )
    );
    IndexBuffer m_rectIb{ m_screenQuad.m_indices.data(),
                          static_cast<unsigned int>(m_screenQuad.m_indices.size(
                          )) };
    Shader m_framebufferShader{ RESOURCES_PATH "shaders/framebuffer.glsl" };
    FBOTex m_fboTex{ {
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    } };
};