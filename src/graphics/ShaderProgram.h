
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <src/tools/globals.h>
#include <src/assets/ShaderAssets.h>

class ShaderProgram
{
    enum class ShaderType
    {
        None,
        Vertex,
        Fragment
    };

public:
    ShaderProgram() = default;
    ~ShaderProgram() = default;

    void AcquireResources(ShaderAssetID _asset);
    void ReleaseResources();

    GLuint GetProgramHandle() const;

    void SetUniformFloat3(std::string name, float r, float g, float b);
    void SetUniformMat4(std::string name, glm::mat4 matrix);

    // Make this the active shader.
    void Use() const;

    void SetUniformFloat(std::string name, float value);
    void SetUniformInt(std::string name, int value);

    void SetUniformFloat3(std::string name, glm::vec3 vector);

    static bool ValidateShader(std::string _fileName);

protected:
    static std::pair<std::string, std::string> ParseShader(std::string filename);
    static GLuint CompileShader(std::string name, std::string shaderSource, ShaderType _shaderType);

private:
    GLuint m_VertexShaderHandle;
    GLuint m_FragmentShaderHandle;
    GLuint m_ShaderProgramHandle;
};
