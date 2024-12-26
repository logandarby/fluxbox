#include "GLCore.h"
#include "Logger.h"
#include <unordered_map>

void GLCore::glClearError() {
    while (glGetError() != GL_NO_ERROR);
}

template <typename T>
std::string convertToHex(T i) {
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(sizeof(T)) << std::hex
           << i;
    return stream.str();
}

std::unordered_map<GLenum, std::string> errorMsgMap = {
    {GL_INVALID_ENUM, "Invalid enum"},
    {GL_INVALID_VALUE, "Invalid value"},
    {GL_INVALID_OPERATION, "Invalid operation"},
    {GL_STACK_OVERFLOW, "Stack overflow"},
    {GL_STACK_UNDERFLOW, "Stack underflow"},
    {GL_OUT_OF_MEMORY, "Out of memory"},
};

bool GLCore::glLogError(
    const char* function, const char* file, const int line
) {
    while (GLenum error = glGetError()) {
        const std::string hexError = convertToHex(error);
        const std::string errorMsg = errorMsgMap[error];
        LOG_GL_ERR(
            "({} {}): {} {} {}", hexError, errorMsg, function, file, line
        );
        return false;
    }
    return true;
}
