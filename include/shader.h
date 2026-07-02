#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    // The program ID assigned by OpenGL
    unsigned int ID;

    // Constructor reads and builds the shader from file paths
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Activate the shader for drawing
    void use() const;

    // Uniform location
    GLint getLocation(const std::string &name) const;

    // Utility uniform setter functions
    void setVec3(const std::string& name, const glm::vec3& vec, const unsigned int location = 0) const;
    void setMat4(const std::string& name, const glm::mat4& mat, const unsigned int location = 0) const;

private:
    // Utility function for checking shader compilation/linking errors
    void checkCompileErrors(unsigned int shader, std::string type);
};