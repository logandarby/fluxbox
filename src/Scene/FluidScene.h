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
static const int BUFFER_SLOT = 3;

struct SimulationVars {
    float dissipation;
    float gridScale;
    float jacobiIterations;
}

static const DEFAULT_SIMULATION_VARS =
    SimulationVars{.dissipation = 1, .gridScale = 1.0f, .jacobiIterations = 10};

static const float DEFAULT_VELOCITY = 1.0f;

class FluidScene : public Scene {
public:
    FluidScene(
        const ApplicationSpec& spec,
        SimulationVars vars = DEFAULT_SIMULATION_VARS
    ) :
        Scene("FluxLab"),
        m_spec{spec},
        m_vars{vars},
        m_stride{1.0f / spec.width, 1.0f / spec.height} {};

    virtual void onInit() override {
        // Set Uniform Functions for all Shaders
        // m_advectionShader.setUniforms([&](Shader & sh) {
        //     sh.setUniform1i("u_field", BUFFER_SLOT)
        //         .setUniform2f("u_stride", m_stride)
        //         .setUniform1i("u_velocity", VELOCITY_SLOT)
        //         .setUniform1f("u_deltaT", timestep)
        //         .setUniform1f("u_dissipation", m_vars.dissipation)
        //         .setUniform1f("u_gridScale", 1.0f / m_vars.gridScale);
        // });

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
            .setUniform3f("u_color", DEFAULT_VELOCITY, 0.0f, 0.0f);
        drawToCurrentFBO(m_colorFragmentShader);

        m_colorFragmentShader
            .setUniformMat4(
                "u_model", glm::translate(
                               glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)),
                               glm::vec3(5.0f, -5.0f, 0.0f)
                           )
            )
            .setUniform3f("u_color", 0.0f, DEFAULT_VELOCITY, 0.0f);
        drawToCurrentFBO(m_colorFragmentShader);

        m_colorFragmentShader
            .setUniformMat4(
                "u_model", glm::translate(
                               glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)),
                               glm::vec3(-5.0f, 5.0f, 0.0f)
                           )
            )
            .setUniform3f("u_color", DEFAULT_VELOCITY, DEFAULT_VELOCITY, 0.0f);
        drawToCurrentFBO(m_colorFragmentShader);
    };
    virtual void onDetach() override {};
    virtual void onRender(const float timestep, const size_t frameCount)
        override {
        m_timestep = timestep;
        Renderer::bindDefaultFramebuffer();
        Renderer::clear();
        if (true) {
            // Advect
            copyToBuffer(
                m_velocityFBO, VELOCITY_SLOT, m_bufferFBO, BUFFER_SLOT
            );
            goto skipAdvection;
            m_velocityFBO.bindFBOandTexture(VELOCITY_SLOT);
            m_advectionShader.bind();
            m_advectionShader
                .useTexture("u_field", m_velocityFBO, VELOCITY_SLOT)
                .useTexture("u_velocity", m_bufferFBO, BUFFER_SLOT)
                .setUniform1f("u_deltaT", timestep)
                .setUniform1f("u_dissipation", m_vars.dissipation)
                .setUniform1f("u_gridScale", 1.0f / m_vars.gridScale);
            drawToCurrentFBO(m_advectionShader);
            m_velocityFBO.unbind();
        skipAdvection:

            // diffuse
            // add forces (mouse)

            // Compute Pressure -- Velocity Divergence
            m_divergenceFBO.bindFBOandTexture(DIVERGENCE_SLOT);
            Renderer::clear();
            m_divergenceShader.bind();
            m_divergenceShader
                .useTexture("u_field", m_velocityFBO, VELOCITY_SLOT)
                .setUniform2f("u_stride", m_stride)
                .setUniform1f("u_gridScale", 1.0f / m_vars.gridScale);
            drawToCurrentFBO(m_divergenceShader);

            // Compute Pressure -- Solve for Pressure in Poisson Equation
            const float alpha = 1.0 / timestep;
            const float beta = alpha + 4.0f;
            LOG_CORE_INFO("Alpha: {}, Beta: {}", alpha, 1 / beta);
            m_bufferFBO.bindFBOandTexture(BUFFER_SLOT);
            Renderer::clear();
            m_pressureFBO.bindFBOandTexture(PRESSURE_SLOT);
            Renderer::clear();
            for (size_t i = 0; i < m_vars.jacobiIterations; i++) {
                // Calculate Pressure Iteration
                m_bufferFBO.bindFBOandTexture(BUFFER_SLOT);
                m_poissonShader.bind();
                m_poissonShader.setUniform1f("u_alpha", alpha)
                    .setUniform2f("u_stride", m_stride)
                    .setUniform1f("u_rBeta", 1.0f / beta)
                    .useTexture(
                        "u_centerValue", m_divergenceFBO, DIVERGENCE_SLOT
                    )
                    .useTexture("u_x", m_pressureFBO, PRESSURE_SLOT);
                drawToCurrentFBO(m_poissonShader);

                // Copy Pressure
                copyToBuffer(
                    m_bufferFBO, BUFFER_SLOT, m_pressureFBO, PRESSURE_SLOT
                );
            }

            // copyToBuffer(
            //     m_velocityFBO, VELOCITY_SLOT, m_bufferFBO, BUFFER_SLOT
            // );

            copyToBuffer(
                m_pressureFBO, PRESSURE_SLOT, m_bufferFBO, BUFFER_SLOT
            );

            // Compute Pressure --Subtract pressure gradient from poisson
            m_velocityFBO.bindFBOandTexture(VELOCITY_SLOT);
            m_gradientSubtractShader.bind();
            m_gradientSubtractShader
                .useTexture("u_pressure", m_pressureFBO, PRESSURE_SLOT)
                .useTexture("u_velocity", m_bufferFBO, BUFFER_SLOT)
                .setUniform2f("u_stride", m_stride)
                .setUniform1f("u_gridScale", 1 / m_vars.gridScale);
            drawToCurrentFBO(m_gradientSubtractShader);
            // m_enabled = false;
        }
    skipPressure:
        Renderer::bindDefaultFramebuffer();
        Renderer::setDepthTest(false);
        m_framebufferShader.bind();
        m_framebufferShader
            .useTexture("u_texture", m_pressureFBO, PRESSURE_SLOT)
            .setUniform2f("u_stride", m_stride);
        drawToCurrentFBO(m_framebufferShader);
    };
    virtual void onEvent(Event& event) override {
        EventDispatcher dispathcer(event);
        dispathcer.dispatch<KeyPressEvent>([&](KeyPressEvent& e) {
            if (e.getKeyCode() == GL::KeyCode::SPACE) {
                m_enabled = !m_enabled;
                return true;
            }
            return false;
        });
    };

private:
    void drawToCurrentFBO(Shader& shader) {
        Renderer::draw(m_screenVAO, m_screenIB, shader);
    }
    void copyToBuffer(
        FBOTex& from, unsigned int fromSlot, FBOTex& to, unsigned int toSlot
    ) {
        to.bindFBOandTexture(toSlot);
        Renderer::clear();
        m_copyShader.bind();
        m_copyShader.useTexture("u_texture", from, fromSlot);
        drawToCurrentFBO(m_copyShader);
    }

private:
    bool m_enabled = true;
    float m_timestep = 1.0f / 60.0f;
    glm::vec2 m_stride;
    ApplicationSpec m_spec;
    SimulationVars m_vars;
    RectangleModel2D m_screenQuad;
    VertexArray m_screenVAO = VertexArray::withBuffer(
        std::make_shared<VertexBuffer>(
            m_screenQuad.m_points.data(), sizeof(m_screenQuad.m_points)
        ),
        std::make_shared<BufferLayout>(
            BufferLayout().withLayout<PositionWithTexture2D>()
        )
    );
    IndexBuffer m_screenIB{
        m_screenQuad.m_indices.data(),
        static_cast<unsigned int>(m_screenQuad.m_indices.size())
    };
    Shader m_framebufferShader{
        RESOURCES_PATH "shaders/commonVert.vert",
        RESOURCES_PATH "shaders/framebuffer.frag"
    };
    Shader m_colorFragmentShader{RESOURCES_PATH "shaders/color.glsl"};
    Shader m_advectionShader{
        RESOURCES_PATH "shaders/commonVert.vert",
        RESOURCES_PATH "shaders/advect.frag"
    };
    Shader m_divergenceShader{
        RESOURCES_PATH "shaders/commonVert.vert",
        RESOURCES_PATH "shaders/divergence.frag"
    };
    Shader m_poissonShader{
        RESOURCES_PATH "shaders/commonVert.vert",
        RESOURCES_PATH "shaders/solveJacobi.frag"
    };
    Shader m_copyShader{
        RESOURCES_PATH "shaders/commonVert.vert",
        RESOURCES_PATH "shaders/copy.frag"
    };
    Shader m_gradientSubtractShader{
        RESOURCES_PATH "shaders/commonVert.vert",
        RESOURCES_PATH "shaders/gradientSubtraction.frag"
    };
    FBOTex m_velocityFBO{{
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    }};
    FBOTex m_pressureFBO{{
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    }};
    FBOTex m_divergenceFBO{{
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    }};
    FBOTex m_bufferFBO{{
        .width = static_cast<unsigned int>(m_spec.width),
        .height = static_cast<unsigned int>(m_spec.height),
    }};
};