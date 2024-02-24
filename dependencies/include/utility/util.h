#ifndef UTIL_H
#define UTIL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>
#include <glm/glm.hpp>

void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
					 GLsizei length, const GLchar* message, const void* userParam);

GLFWwindow* initialize(int windowWidth, int windowHeight, std::string_view title, int majorVersion, int minorVersion);

unsigned int compileShader(unsigned int type, std::string_view path);
unsigned int createShaderProgram(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);

void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec3& vec3);
void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec4& vec4);
void setUniform(unsigned int shaderID, std::string_view fieldName, float flt);
void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat4& mat4);

#endif