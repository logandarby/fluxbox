#pragma once

#include <glm/glm.hpp>
#include <spdlog/fmt/fmt.h>
#include <memory>
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

class Shader {
public:
    Shader(const std::string& fileName);
    ~Shader();

    void bind() const;
    void unbind() const;

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

    // void setUniform(std::shared_ptr<Uniform> uniform);
    unsigned int getRenderer() {
        return m_rendererId;
    }

private:
    unsigned int m_rendererId;
    const std::string m_filePath;
    std::unordered_map<std::string, int> m_locationMap;

    static unsigned int createShader(const ShaderProgramSource& source);
    static unsigned int compileShader(
        const unsigned int shaderType, const std::string& source
    );
    static ShaderProgramSource parseShader(const std::string& fileName);
    int getUniformLocation(const std::string& name);
};
