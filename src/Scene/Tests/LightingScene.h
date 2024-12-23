#pragma once

#include "ApplicationSpec.h"
#include "Core/Core.h"
#include "Events/Event.h"
#include "Math/MatrixUtils.h"
#include "Model/CubeModel.h"
#include "Model/NormalCubeModel.h"
#include "Model/RectangleModel.h"
#include "Renderer/FBO.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Scene/CameraController.h"
#include "Scene/Light.h"
#include "Scene/Material.h"
#include "Scene/Scene.h"
#include "Texture/CubemapTexture.h"
#include "Texture/Texture2D.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include <imgui.h>

static const glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)
};

static const CameraControllerSpec DEFAULT_CAMERA_CONTROLLER_SPEC{
    .moveSpeed = 0.1f,
    .rotSpeed = 0.08f,
};
static const unsigned int DOWNSAMPLE = 4;
static const GL::Sampler2D TEXTURE_SLOT = 0;
static const GL::Sampler2D SPECULAR_SLOT = 1;
static const GL::Sampler2D SKYBOX_SLOT = 2;
static const GL::Sampler2D FBO_TEX_SLOT = 3;

class LightingScene : public Scene {
private:
    CameraController m_cameraController;
    const ApplicationSpec& m_spec;
    bool m_isMenuOpen = false;

    // Models (Points, VAO, IBs)
    // Rect
    RectangleModel2D rect;
    VertexArray rectVAO = VertexArray::withBuffer(
        std::make_shared<VertexBuffer>(
            rect.m_points.data(), sizeof(rect.m_points)
        ),
        std::make_shared<BufferLayout>(
            BufferLayout().withLayout<PositionWithTexture2D>()
        )
    );
    IndexBuffer rectIb{ rect.m_indices.data(),
                        static_cast<unsigned int>(rect.m_indices.size()) };

    NormalCubeModel normalCube;
    VertexArray cubeVao = VertexArray::withBuffer(
        std::make_shared<VertexBuffer>(
            normalCube.m_points.data(), sizeof(normalCube.m_points)
        ),
        std::make_shared<BufferLayout>(
            BufferLayout().withLayout<PosNormalTexture3D>()
        )
    );
    IndexBuffer cubeIb{ normalCube.m_indices.data(),
                        static_cast<unsigned int>(normalCube.m_indices.size()
                        ) };

    CubeModel lightingCube;
    VertexArray lightVAO = VertexArray::withBuffer(
        std::make_shared<VertexBuffer>(
            lightingCube.m_points.data(), sizeof(lightingCube.m_points)
        ),
        std::make_shared<BufferLayout>(
            BufferLayout().withLayout<PositionWithTexture3D>()
        )
    );
    IndexBuffer lightingIb{
        lightingCube.m_indices.data(),
        static_cast<unsigned int>(lightingCube.m_indices.size())
    };

    CubeModel skyboxCube;
    VertexArray skyboxVAO = VertexArray::withBuffer(
        std::make_shared<VertexBuffer>(
            skyboxCube.m_points.data(), sizeof(skyboxCube.m_points)
        ),
        std::make_shared<BufferLayout>(
            BufferLayout().withLayout<PositionWithTexture3D>()
        )
    );
    IndexBuffer skyboxIb{ skyboxCube.m_indices.data(),
                          static_cast<unsigned int>(skyboxCube.m_indices.size()
                          ) };

    // Textures
    FBOTex fboTex;
    Texture2D tex{ RESOURCES_PATH "/textures/box/box_texture.png" };
    Texture2D specularTex{ RESOURCES_PATH "/textures/box/box_specular.png" };
    CubemapTexture skyboxTex{ CubemapSource{
        .right = RESOURCES_PATH "/textures/skybox2/right.jpg",
        .left = RESOURCES_PATH "/textures/skybox2/left.jpg",
        .top = RESOURCES_PATH "/textures/skybox2/top.jpg",
        .bottom = RESOURCES_PATH "/textures/skybox2/bottom.jpg",
        .back = RESOURCES_PATH "/textures/skybox2/back.jpg",
        .front = RESOURCES_PATH "/textures/skybox2/front.jpg" } };
    Shader litTextureShader{ RESOURCES_PATH "/shaders/specTexture.glsl" };
    Shader colorShader{ RESOURCES_PATH "/shaders/color3d.glsl" };
    Shader skyboxShader{ RESOURCES_PATH "/shaders/skyboxShader.glsl" };
    Shader fboShader{ RESOURCES_PATH "/shaders/dither.glsl" };
    // Light
    DirectionalLight light{
        .direction = lightingCube.modelMatrix * glm::vec4(0, 0, 0, 1.0f),
        .ambient = glm::vec3(0.2f, 0.2f, 0.2f),
        .diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
        .specular = glm::vec3(1.0f, 1.0f, 1.0f),
    };
    PointLight ptLight{
        .position = lightingCube.modelMatrix * glm::vec4(0, 0, 0, 1.0f),
        .ambient = glm::vec3(0.2f, 0.2f, 0.2f),
        .diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
        .specular = glm::vec3(1.0f, 1.0f, 1.0f),
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f,
    };
    SpecMaterial material{
        .specular = SPECULAR_SLOT,
        .texture = TEXTURE_SLOT,
        .shininess = 32,
    };

    float spread = 0.05f;
    bool isDither = true;

public:
    LightingScene(const ApplicationSpec& spec) :
        Scene("Lighting Scene"),
        m_spec(spec),
        m_cameraController(
            CameraSpecification{ .viewportWidth = spec.width,
                                 .viewportHeight = spec.height },
            DEFAULT_CAMERA_CONTROLLER_SPEC
        ),
        fboTex({
            .width = static_cast<unsigned int>(m_spec.width) / DOWNSAMPLE,
            .height = static_cast<unsigned int>(m_spec.height) / DOWNSAMPLE,
            .magSampler = GL::MagFilter::Nearest,
            .minSampler = GL::MagFilter::Nearest,
        }) {};

    virtual void onInit() override {
        lightingCube.modelMatrix = glm::translate(
            lightingCube.modelMatrix, glm::vec3(2.0f, 2.0f, -2.0f)
        );
        lightingCube.modelMatrix =
            glm::scale(lightingCube.modelMatrix, glm::vec3(0.6f, 0.6f, 0.6f));

        tex.bind(TEXTURE_SLOT);
        specularTex.bind(SPECULAR_SLOT);
        skyboxTex.bind(SKYBOX_SLOT);
        fboTex.bindTexture(FBO_TEX_SLOT);
    };

    virtual void onRender(const float timestep, const float frameCount)
        override {
        if (m_isMenuOpen) {
            onImguiRender();
        }

        const Camera& camera = m_cameraController.getCamera();
        fboTex.bindFBO();

        Renderer::clear();
        Renderer::setDepthTest(true);

        normalCube.modelMatrix = glm::rotate(
            normalCube.modelMatrix, glm::radians(2.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
        m_cameraController.onUpdate();

        litTextureShader.bind();
        litTextureShader.setUniform1i("u_texture", 0)
            .setUniform3f("u_viewPos", camera.getPos())
            .setMVP(
                "u_mvp", normalCube.modelMatrix, camera.getViewMatrix(),
                camera.getProjectionMatrix()
            )
            .setLight("u_pointLight", ptLight)
            .setLight("u_dirLight", light)
            .setMaterial("u_material", material);

        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(
                model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)
            );
            if (i != 0) {
                litTextureShader.setUniformMat4("u_mvp.model", model)
                    .setUniformMat3(
                        "u_mvp.normal", Math::getNormalMatrix(model)
                    );
            }

            Renderer::draw(cubeVao, cubeIb, litTextureShader);
        }

        colorShader.bind();
        colorShader.setUniform4f("u_color", glm::vec4(light.diffuse, 1.0f))
            .setUniformMat4("u_model", lightingCube.modelMatrix)
            .setUniformMat4("u_view", camera.getViewMatrix())
            .setUniformMat4("u_projection", camera.getProjectionMatrix());

        Renderer::draw(lightVAO, lightingIb, colorShader);

        skyboxShader.bind();
        skyboxShader
            .setMVP(
                "u_mvp", glm::mat4(1.0f),
                glm::mat4(glm::mat3(camera.getViewMatrix())),
                camera.getProjectionMatrix()
            )
            .setUniform1i("u_skybox", SKYBOX_SLOT);

        Renderer::draw(skyboxVAO, skyboxIb, skyboxShader);

        Renderer::setDepthTest(false);

        fboTex.unbind();
        fboTex.bindTexture(FBO_TEX_SLOT);

        fboShader.bind();
        fboShader.setUniform1i("u_texture", FBO_TEX_SLOT)
            .setUniform1i("u_enable", isDither)
            .setUniform1f("u_spread", spread)
            .setUniform2f(
                "u_resolution",
                glm::vec2(m_spec.width / DOWNSAMPLE, m_spec.height / DOWNSAMPLE)
            );

        Renderer::getViewport(0, 0, m_spec.width, m_spec.height);

        Renderer::draw(rectVAO, rectIb, fboShader);
    }

    void onImguiRender() {
        ImGui::Begin("Debug");
        ImGui::Text(
            fmt::format("Framerate {}", ImGui::GetIO().Framerate).c_str()
        );
        ImGui::Text("Dither");
        ImGui::SliderFloat("Spread", &spread, 0.0f, 0.4f);
        ImGui::Checkbox("Enable Dither", &isDither);
        ImGui::Text("Light");
        ImGui::SliderFloat3("Light Diffuse", &light.diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Specular", &light.specular[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Ambient", &light.ambient[0], 0.0f, 1.0f);
        ImGui::Text("Material");
        ImGui::SliderFloat(
            "Material Shininess", &material.shininess, 0.0f, 100.0f
        );
        ImGui::End();
    }

    virtual void onEvent(Event& event) override {
        m_cameraController.onEvent(event);
    };
};