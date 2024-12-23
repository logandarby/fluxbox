#pragma once

#include "Scene.h"
#include "Core/Core.h"
#include "Model/RectangleModel.h"
#include "Renderer/FBO.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Texture/Texture2D.h"
#include "glm/gtc/matrix_transform.hpp"

static const int VELOCITY_SLOT = 0;

struct SimulationVars {
    float dissipation = 1.0f;
    float gridScale = 0.1f;
}

static const DEFAULT_SIMULATION_VARS = SimulationVars{
    .dissipation = 1.0f,
    .gridScale = 1.0f,
};

class FluidScene : public Scene {
public:
    FluidScene(
        const ApplicationSpec& spec,
        SimulationVars vars = DEFAULT_SIMULATION_VARS
    ) :
        Scene("FluxLab"), m_spec{ spec }, m_vars{ vars } {};

    virtual void onInit() override {
        m_velocity.bindTexture(VELOCITY_SLOT);
    };
    virtual void onDetach() override {};
    virtual void onRender(const float timestep) override {
        m_velocity.bindTexture(VELOCITY_SLOT);
        m_velocity.bindFBO();

        Renderer::clear();
        Renderer::setDepthTest(true);

        m_colorFragmentShader.bind();
        m_colorFragmentShader
            .setUniformMat4(
                "u_model", glm::scale(glm::mat4(1.0f), glm::vec3(0.1f))
            )
            .setUniform3f("u_color", 1.0f, 1.0f, 1.0f);
        Renderer::draw(m_rectVAO, m_rectIb, m_colorFragmentShader);

        // Render stuff

        // advect

        m_velocity.bindTexture(VELOCITY_SLOT);
        m_velocity.bindFBO();

        // Renderer::clear();
        // Renderer::setDepthTest(true);

        // m_advectionShader.bind();
        // m_advectionShader.setUniform1i("u_field", VELOCITY_SLOT)
        //     .setUniform1i("u_velocity", VELOCITY_SLOT)
        //     .setUniform1f("u_delta_t", timestep)
        //     .setUniform1f("u_dissipation", m_vars.dissipation)
        //     .setUniform1f("u_grid_scale", m_vars.gridScale);
        // Renderer::draw(m_rectVAO, m_rectIb, m_advectionShader);

        // diffuse
        // add forces (mouse)
        // compute pressure
        // subtract pressure gradient

        Renderer::bindDefaultFramebuffer();
        Renderer::setDepthTest(false);
        m_velocity.bindTexture(VELOCITY_SLOT);
        m_framebufferShader.bind();
        m_framebufferShader.setUniform1i("u_texture", VELOCITY_SLOT);

        // Renderer::getViewport(0, 0, m_spec.width, m_spec.height);

        Renderer::draw(m_rectVAO, m_rectIb, m_framebufferShader);
    };
    virtual void onEvent(Event& event) override {};

private:
    ApplicationSpec m_spec;
    SimulationVars m_vars;
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
    Shader m_framebufferShader{ RESOURCES_PATH "shaders/commonVert.vert",
                                RESOURCES_PATH "shaders/framebuffer.frag" };
    Shader m_colorFragmentShader{ RESOURCES_PATH "shaders/color.glsl" };
    Shader m_advectionShader{ RESOURCES_PATH "shaders/commonVert.vert",
                              RESOURCES_PATH "shaders/advect.frag" };
    FBOTex m_velocity{ {
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    } };
};