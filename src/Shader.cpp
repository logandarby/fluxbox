#include "Shader.h"

#include <fstream>
#include <sstream>

#include "core/GLCore.h"
#include "renderer.h"

Shader::Shader(const std::string& fileName)
    : m_rendererId(0), m_filePath(fileName) {
    ShaderProgramSource source = parseShader(fileName);
    m_rendererId = createShader(source);
}

Shader::~Shader() { GL_CALL(glDeleteProgram(m_rendererId)) }

void Shader::bind() const { GL_CALL(glUseProgram(m_rendererId)); }

void Shader::unbind() const { GL_CALL(glUseProgram(0)); }

unsigned int Shader::createShader(const ShaderProgramSource& source) {
    GLuint program = glCreateProgram();
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, source.vertexSource);
    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, source.fragmentSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned int Shader::compileShader(const unsigned int shaderType,
                                   const std::string& source) {
    GL_CALL(GLuint id = glCreateShader(shaderType));
    const char* src = source.c_str();
    GL_CALL(glShaderSource(id, 1, &src, nullptr));
    GL_CALL(glCompileShader(id));

    int result;
    GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GL_CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GL_CALL(glGetShaderInfoLog(id, length, &length, message));
        LOG_GL_ERR("Failed to Compile {} shader: {}",
                   (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"),
                   message);
    }

    return id;
}

ShaderProgramSource Shader::parseShader(const std::string& fileName) {
    using namespace std;
    fstream stream(fileName);

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    string line;
    stringstream ss[2];
    ShaderType shaderType = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != string::npos) {
            if (line.find("vertex") != string::npos) {
                shaderType = ShaderType::VERTEX;
            }
            if (line.find("fragment") != string::npos) {
                shaderType = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)shaderType] << line << endl;
        }
    }
    const size_t size = sizeof(ss) / sizeof(ss[0]);
    if (size != 2) {
        throw std::runtime_error(fmt::format(
            "Shader file {} does not contain both vertex and fragment shaders",
            fileName));
    }
    return {ss[0].str(), ss[1].str()};
}

Shader& Shader::setUniform4f(const std::string& name, const glm::vec4& v) {
    setUniform4f(name, v[0], v[1], v[2], v[3]);
    return *this;
}

Shader& Shader::setUniform4f(const std::string& name, float v0, float v1,
                             float v2, float v3) {
    const int location = getUniformLocation(name);
    GL_CALL(glUniform4f(location, v0, v1, v2, v3));
    return *this;
}

Shader& Shader::setUniform3f(const std::string& name, const glm::vec3& v) {
    setUniform3f(name, v.x, v.y, v.z);
    return *this;
}

Shader& Shader::setUniform3f(const std::string& name, float v0, float v1,
                             float v2) {
    const int location = getUniformLocation(name);
    GL_CALL(glUniform3f(location, v0, v1, v2));
    return *this;
}

Shader& Shader::setUniform1i(const std::string& name, const int i) {
    const int location = getUniformLocation(name);
    GL_CALL(glUniform1i(location, i));
    return *this;
}

Shader& Shader::setUniform1f(const std::string& name, const float i) {
    GL_CALL(glUniform1f(getUniformLocation(name), i));
    return *this;
}

Shader& Shader::setLight(const std::string& name,
                         const DirectionalLight& light) {
    setUniform3f(fmt::format("{}.direction", name), light.direction);
    setUniform3f(fmt::format("{}.ambient", name), light.ambient);
    setUniform3f(fmt::format("{}.diffuse", name), light.diffuse);
    setUniform3f(fmt::format("{}.specular", name), light.specular);
    return *this;
}

Shader& Shader::setLight(const std::string& name, const PointLight& light) {
    setUniform3f(fmt::format("{}.position", name), light.position);
    setUniform3f(fmt::format("{}.ambient", name), light.ambient);
    setUniform3f(fmt::format("{}.diffuse", name), light.diffuse);
    setUniform3f(fmt::format("{}.specular", name), light.specular);
    setUniform1f(fmt::format("{}.constant", name), light.constant);
    setUniform1f(fmt::format("{}.linear", name), light.linear);
    setUniform1f(fmt::format("{}.quadratic", name), light.quadratic);
    return *this;
}

template <typename T>
Shader& Shader::setMaterial(const std::string& name, const T& material) {
    throw std::runtime_error(fmt::format(
        "Shader does not have implementation for 'setMaterial' for type {}",
        typeid(T).name()));
    return *this;
}

template <>
Shader& Shader::setMaterial<Material>(const std::string& name,
                                      const Material& material) {
    setUniform1i(fmt::format("{}.diffuse", name), material.texture);
    setUniform3f(fmt::format("{}.specular", name), material.specular);
    setUniform1f(fmt::format("{}.shininess", name), material.shininess);
    return *this;
}

template <>
Shader& Shader::setMaterial<SpecMaterial>(const std::string& name,
                                          const SpecMaterial& material) {
    setUniform1i(fmt::format("{}.diffuse", name), material.texture);
    setUniform1i(fmt::format("{}.specular", name), material.specular);
    setUniform1f(fmt::format("{}.shininess", name), material.shininess);
    return *this;
}

Shader& Shader::setMVP(const std::string& name, const glm::mat4& model,
                       const glm::mat4& view, const glm::mat4& projection) {
    const glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));
    setUniformMat4(fmt::format("{}.model", name), model);
    setUniformMat4(fmt::format("{}.view", name), view);
    setUniformMat4(fmt::format("{}.projection", name), projection);
    setUniformMat3(fmt::format("{}.normal", name), normal);
    return *this;
}

Shader& Shader::setUniformMat4(const std::string& name, const glm::mat4& mat) {
    GL_CALL(
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]));
    return *this;
}

Shader& Shader::setUniformMat3(const std::string& name, const glm::mat3& mat) {
    GL_CALL(
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]));
    return *this;
}

// void Shader::setUniform(const std::shared_ptr<Uniform> uniform) {
//	const int location = getUniformLocation(uniform->getName());
//	uniform->bind(location);
// }

int Shader::getUniformLocation(const std::string& name) {
    if (m_locationMap.find(name) != m_locationMap.end()) {
        return m_locationMap[name];
    }
    GL_CALL(const int location =
                glGetUniformLocation(m_rendererId, name.c_str()));
    m_locationMap[name] = location;
    return location;
}
