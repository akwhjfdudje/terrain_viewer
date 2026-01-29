#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string& vertexPath,
           const std::string& fragmentPath);
    ~Shader();

    void bind() const;
    void unbind() const;

    void setMat4(const std::string& name, const glm::mat4& m) const;
    void setVec3(const std::string& name, const glm::vec3& v) const;
    void setFloat(const std::string& name, float v) const;

private:
    unsigned int m_program = 0;

    std::string loadFile(const std::string& path);
    unsigned int compile(unsigned int type, const std::string& src);
};

