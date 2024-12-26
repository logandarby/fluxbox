#pragma once

#include "FBO.h"
#include "Texture/Texture2D.h"
#include <glm/glm.hpp>
#include <spdlog/fmt/fmt.h>
#include <memory>
#include <string>
#include <unordered_map>

static const std::string SHADER_DRECTIVE = "#shader";
static const std::string VERTEX_DELIMITER = "vertex";
static const std::string FRAGMENT_DELIMITER = "fragment";

struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

class ShaderException : public std::runtime_error {
public:
    ShaderException(const std::string& message) : std::runtime_error(message) {}
};

class Shader {
public:
    Shader(const std::string& fileName);
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    void bind();
    void unbind() const;

    // Set a uniform function that sets on every bind
    Shader& setUniforms(std::function<void(Shader&)> setUniforms);

    // Set primitive Uniforms
    Shader& setUniform4f(
        const std::string& name, float v0, float v1, float v2, float v3
    );
    Shader& setUniform4f(const std::string& name, const glm::vec4& v);
    Shader& setUniform3f(const std::string& name, float v0, float v1, float v2);
    Shader& setUniform3f(const std::string& name, const glm::vec3& v);
    Shader& setUniform2f(const std::string& name, const glm::vec2& v);

    Shader& setUniform1i(const std::string& name, int i);
    Shader& setUniform1f(const std::string& name, float i);

    Shader& setUniformMat4(const std::string& name, const glm::mat4& mat);
    Shader& setUniformMat3(const std::string& name, const glm::mat3& mat);

    Shader& useTexture(
        const std::string& name, Texture2D& texture, unsigned int slot
    );
    Shader& useTexture(
        const std::string& name, FBOTex& texture, unsigned int slot
    );

    // void setUniform(std::shared_ptr<Uniform> uniform);
    unsigned int getRenderer() {
        return m_rendererId;
    }

private:
    unsigned int m_rendererId;
    std::string m_filePath;
    std::unordered_map<std::string, int> m_locationMap;
    std::function<void(Shader&)> m_setUniforms;

private:
    static unsigned int createShader(const ShaderProgramSource& source);
    static unsigned int compileShader(
        const unsigned int shaderType, const std::string& source
    );
    static ShaderProgramSource parseShader(const std::string& fileName);
    static ShaderProgramSource parseShader(
        const std::string& vertexSrc, const std::string& fragmentSrc
    );
    int getUniformLocation(const std::string& name);
};
