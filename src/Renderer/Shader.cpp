#include "Shader.h"
#include "renderer.h"
#include "core/GLCore.h"
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& fileName) : m_rendererId(0) {
    LOG_CORE_INFO("Parsing shader file: {}", fileName);
    ShaderProgramSource source = parseShader(fileName);
    m_rendererId = createShader(source);
}

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    LOG_CORE_INFO(
        "Parsing shader files: Vertex: {}, Fragment: {}", vertexSrc, fragmentSrc
    );
    ShaderProgramSource source = parseShader(vertexSrc, fragmentSrc);
    m_rendererId = createShader(source);
}

Shader::~Shader() {
    GL_CALL(glDeleteProgram(m_rendererId))
}

void Shader::bind() const {
    GL_CALL(glUseProgram(m_rendererId));
}

void Shader::unbind() const {
    GL_CALL(glUseProgram(0));
}

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

unsigned int Shader::compileShader(
    const unsigned int shaderType, const std::string& source
) {
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
        LOG_GL_ERR(
            "Failed to Compile {} shader: {}",
            (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"), message
        );
    }

    return id;
}

ShaderProgramSource Shader::parseShader(const std::string& fileName) {
    using namespace std;
    fstream stream(fileName);

    if (stream.fail()) {
        throw ShaderException(fmt::format(
            "Shader file {} does not exist or could not be opened", fileName
        ));
    }

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    string line;
    stringstream ss[2];
    ShaderType shaderType = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find(SHADER_DRECTIVE) != string::npos) {
            if (line.find(VERTEX_DELIMITER) != string::npos) {
                shaderType = ShaderType::VERTEX;
            }
            if (line.find(FRAGMENT_DELIMITER) != string::npos) {
                shaderType = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)shaderType] << line << endl;
        }
    }
    const size_t size = sizeof(ss) / sizeof(ss[0]);
    if (size != 2) {
        throw ShaderException(fmt::format(
            "Shader file {} does not contain both vertex and fragment shaders",
            fileName
        ));
    }
    return { ss[0].str(), ss[1].str() };
}

ShaderProgramSource Shader::parseShader(
    const std::string& vertexSrc, const std::string& fragmentSrc
) {
    using namespace std;
    stringstream ss[2];
    string line;
    fstream vertexStream(vertexSrc);
    if (vertexStream.fail()) {
        throw ShaderException(fmt::format(
            "Vertex shader file {} does not exist or could not be opened",
            vertexSrc
        ));
    }
    fstream fragmentStream(fragmentSrc);
    if (fragmentStream.fail()) {
        throw ShaderException(fmt::format(
            "Fragment shader file {} does not exist or could not be opened",
            fragmentSrc
        ));
    }
    while (getline(vertexStream, line)) {
        ss[0] << line << endl;
    }
    while (getline(fragmentStream, line)) {
        ss[1] << line << endl;
    }
    ShaderProgramSource src{ ss[0].str(), ss[1].str() };
    if (src.vertexSource.empty() || src.fragmentSource.empty()) {
        throw ShaderException(fmt::format(
            "Shader files {} and {} do not contain both vertex and fragment shaders",
            vertexSrc, fragmentSrc
        ));
    }
    return src;
}

Shader& Shader::setUniform4f(const std::string& name, const glm::vec4& v) {
    setUniform4f(name, v[0], v[1], v[2], v[3]);
    return *this;
}

Shader& Shader::setUniform4f(
    const std::string& name, float v0, float v1, float v2, float v3
) {
    const int location = getUniformLocation(name);
    GL_CALL(glUniform4f(location, v0, v1, v2, v3));
    return *this;
}

Shader& Shader::setUniform3f(const std::string& name, const glm::vec3& v) {
    setUniform3f(name, v.x, v.y, v.z);
    return *this;
}

Shader& Shader::setUniform3f(
    const std::string& name, float v0, float v1, float v2
) {
    const int location = getUniformLocation(name);
    GL_CALL(glUniform3f(location, v0, v1, v2));
    return *this;
}

Shader& Shader::setUniform2f(const std::string& name, const glm::vec2& v) {
    const int location = getUniformLocation(name);
    GL_CALL(glUniform2f(location, v.x, v.y));
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

Shader& Shader::setUniformMat4(const std::string& name, const glm::mat4& mat) {
    GL_CALL(
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0])
    );
    return *this;
}

Shader& Shader::setUniformMat3(const std::string& name, const glm::mat3& mat) {
    GL_CALL(
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0])
    );
    return *this;
}

Shader& Shader::useTexture(
    const std::string& name, Texture2D& texture, unsigned int slot
) {
    texture.bind(slot);
    setUniform1i(name, slot);
    return *this;
}

int Shader::getUniformLocation(const std::string& name) {
    if (m_locationMap.find(name) != m_locationMap.end()) {
        return m_locationMap[name];
    }
    GL_CALL(
        const int location = glGetUniformLocation(m_rendererId, name.c_str())
    );
    m_locationMap[name] = location;
    return location;
}
