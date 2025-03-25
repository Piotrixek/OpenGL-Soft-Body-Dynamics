// Shader.cpp
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Globals.h"


std::string readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR::FILE::READ_FAILED: " << path << "\n";
        logger.addLog("[ERROR] Failed to load: " + std::string(path));
        return "";
    }
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    logger.addLog("Loaded shader: " + std::string(path));
    return stream.str();
}
unsigned int compileShader(GLenum type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "SHADER COMPILE ERROR (" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
            << "):\n" << infoLog << "\n";
        logger.addLog(std::string("Shader compile error: ") + infoLog);
        return 0;
    }
    return shader;
}
unsigned int loadShader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    if (vertexCode.empty() || fragmentCode.empty()) return 0;
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    if (!vertex || !fragment) return 0;
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "SHADER LINK ERROR:\n" << infoLog << "\n";
        logger.addLog(std::string("Shader link error: ") + infoLog);
        glDeleteProgram(program);
        return 0;
    }
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "SHADER VALIDATION ERROR:\n" << infoLog << "\n";
        logger.addLog(std::string("Shader validation error: ") + infoLog);
        glDeleteProgram(program);
        return 0;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}
