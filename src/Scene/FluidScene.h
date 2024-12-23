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
static const int PRESSURE_SLOT = 1;
static const int DIVERGENCE_SLOT = 2;
static const int PRESSURE_COPY_SLOT = 3;

struct SimulationVars {
    float dissipation;
    float gridScale;
    float jacobiIterations;
}

static const DEFAULT_SIMULATION_VARS = SimulationVars{ .dissipation = 0.99f,
                                                       .gridScale = 0.1f,
                                                       .jacobiIterations = 80 };

class FluidScene : public Scene {
public:
    FluidScene(
        const ApplicationSpec& spec,
        SimulationVars vars = DEFAULT_SIMULATION_VARS
    ) :
        Scene("FluxLab"), m_spec{ spec }, m_vars{ vars } {};

    virtual void onInit() override {
        // Draw initial velocity rectangle
        m_velocityFBO.bindTexture(VELOCITY_SLOT);
        m_velocityFBO.bindFBO();
        Renderer::clear();
        Renderer::setDepthTest(true);
        m_colorFragmentShader.bind();
        m_colorFragmentShader
            .setUniformMat4(
                "u_model", glm::scale(glm::mat4(1.0f), glm::vec3(0.1f))
            )
            .setUniform3f("u_color", 1.0f, 1.0f, 1.0f);
        Renderer::draw(m_rectVAO, m_rectIb, m_colorFragmentShader);
    };
    virtual void onDetach() override {};
    virtual void onRender(const float timestep, const size_t frameCount)
        override {
        const glm::vec2 rdv{ 1.0f / m_spec.width, 1.0f / m_spec.height };
        Renderer::bindDefaultFramebuffer();
        Renderer::clear();
        if (frameCount == 0) {
            m_velocityFBO.bindTexture(VELOCITY_SLOT);
            m_velocityFBO.bindFBO();

            // Advect
            // m_velocity.bindTexture(VELOCITY_SLOT);
            // m_velocity.bindFBO();

            // m_advectionShader.bind();
            // m_advectionShader.setUniform1i("u_field", VELOCITY_SLOT)
            //     .setUniform1i("u_velocity", VELOCITY_SLOT)
            //     .setUniform1f("u_deltaT", timestep)
            //     .setUniform1f("u_dissipation", m_vars.dissipation)
            //     .setUniform1f("u_gridScale", m_vars.gridScale);
            // Renderer::draw(m_rectVAO, m_rectIb, m_advectionShader);
            m_velocityFBO.unbind();

            // diffuse
            // add forces (mouse)

            // Compute Pressure -- Velocity Divergence
            m_divergenceFBO.bindTexture(DIVERGENCE_SLOT);
            m_divergenceFBO.bindFBO();
            m_velocityFBO.bindTexture(VELOCITY_SLOT);
            Renderer::clear();
            m_divergenceShader.bind();
            m_divergenceShader.setUniform1i("u_field", VELOCITY_SLOT)
                .setUniform2f("u_stride", rdv)
                .setUniform1f("u_gridScale", 1 / m_vars.gridScale);
            Renderer::draw(m_rectVAO, m_rectIb, m_divergenceShader);
            m_divergenceFBO.unbind();

            // Compute Pressure -- Solve for Pressure in Poisson Equation
            const float alpha =
                m_vars.gridScale * m_vars.gridScale / (timestep);
            const float beta = alpha + 4.0f;
            LOG_CORE_INFO(
                "alpha: {}, gridScale: {}, timestep: {}, beta: {}", alpha,
                m_vars.gridScale, timestep, 1 / beta
            );
            m_pressureFBO.bindTexture(PRESSURE_SLOT);
            m_divergenceFBO.bindTexture(DIVERGENCE_SLOT);
            m_pressureFBO.bindFBO();
            Renderer::clear();
            m_poissonShader.bind();
            for (size_t i = 0; i < m_vars.jacobiIterations; i++) {
                // Copy Pressure
                // m_bufferFBO.bindTexture(PRESSURE_COPY_SLOT);
                // m_bufferFBO.bindFBO();
                // m_pressureFBO.bindTexture(PRESSURE_SLOT);
                // m_copyShader.bind();
                // m_copyShader.setUniform1i("u_texture", PRESSURE_SLOT);
                // Renderer::draw(m_rectVAO, m_rectIb, m_copyShader);

                // Calculate Pressure
                m_pressureFBO.bindTexture(PRESSURE_SLOT);
                m_pressureFBO.bindFBO();
                m_bufferFBO.bindTexture(PRESSURE_COPY_SLOT);
                m_divergenceFBO.bindTexture(DIVERGENCE_SLOT);
                m_poissonShader.bind();
                m_poissonShader.setUniform1f("u_alpha", alpha)
                    .setUniform2f("u_stride", rdv)
                    .setUniform1f("u_rBeta", 1 / beta)
                    .setUniform1i("u_b", DIVERGENCE_SLOT)
                    .setUniform1i("u_x", PRESSURE_COPY_SLOT);
                Renderer::draw(m_rectVAO, m_rectIb, m_poissonShader);
            }
            m_pressureFBO.unbind();
            m_divergenceFBO.unbind();

            // Compute Pressure --Subtract pressure gradient from poisson result
        }
        Renderer::bindDefaultFramebuffer();
        Renderer::setDepthTest(false);
        // m_velocityFBO.bindTexture(VELOCITY_SLOT);
        // m_divergenceFBO.bindTexture(DIVERGENCE_SLOT);
        m_pressureFBO.bindTexture(PRESSURE_SLOT);
        m_framebufferShader.bind();
        // m_framebufferShader.setUniform1i("u_texture", PRESSURE_SLOT);
        m_framebufferShader.setUniform1i("u_texture", PRESSURE_SLOT);

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
    Shader m_divergenceShader{ RESOURCES_PATH "shaders/commonVert.vert",
                               RESOURCES_PATH "shaders/divergence.frag" };
    Shader m_poissonShader{ RESOURCES_PATH "shaders/commonVert.vert",
                            RESOURCES_PATH "shaders/solveJacobi.frag" };
    Shader m_copyShader{ RESOURCES_PATH "shaders/copy.glsl" };
    FBOTex m_velocityFBO{ {
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    } };
    FBOTex m_pressureFBO{ {
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    } };
    FBOTex m_divergenceFBO{ {
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    } };
    FBOTex m_bufferFBO{ {
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    } };
};