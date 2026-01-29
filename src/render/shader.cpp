#include <glad/gl.h>

#include "shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath,
               const std::string& fragmentPath) {
    std::string vsSrc = loadFile(vertexPath);
    std::string fsSrc = loadFile(fragmentPath);

    unsigned int vs = compile(GL_VERTEX_SHADER, vsSrc);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fsSrc);

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);

    int ok;
    glGetProgramiv(m_program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, log);
        std::cerr << "[Shader Link Error]\n" << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader() {
    glDeleteProgram(m_program);
}

void Shader::bind() const {
    glUseProgram(m_program);
}

void Shader::unbind() const {
    glUseProgram(0);
}

unsigned int Shader::compile(unsigned int type, const std::string& src) {
    unsigned int id = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(id, 1, &cstr, nullptr);
    glCompileShader(id);

    int ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(id, 1024, nullptr, log);
        std::cerr << "[Shader Compile Error]\n" << log << std::endl;
    }
    return id;
}

std::string Shader::loadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const {
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()),
                       1, GL_FALSE, &m[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()),
                 1, &v[0]);
}

void Shader::setFloat(const std::string& name, float v) const {
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), v);
}

