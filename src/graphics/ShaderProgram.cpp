
#include "ShaderProgram.h"

#include <src/engine/Engine.h>

ShaderProgram::ShaderProgram(ShaderAssetID _assetID)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    std::string shaderName = Assets::GetShaderAssetName(_assetID);

    std::pair<std::string,std::string> shaders = ParseShader(Globals::FREEPLANET_ASSET_PATH + "shaders/" + shaderName + ".shader");

    if(shaders.first.empty() || shaders.second.empty())
    {
        LogError(shaderName + " couldn't be parsed correctly.");
        return;
    }

    // Create and compile the vertex shader
    m_VertexShaderHandle = CompileShader(shaderName, shaders.first, ShaderType::Vertex);

    // Create and compile the fragment shader
    m_FragmentShaderHandle = CompileShader(shaderName, shaders.second, ShaderType::Fragment);

    // Link the vertex and fragment shader into a shader program
    m_ShaderProgramHandle = glCreateProgram();
    Globals::ReportGLError("glCreateProgram");

    glAttachShader(m_ShaderProgramHandle, m_VertexShaderHandle);
    glAttachShader(m_ShaderProgramHandle, m_FragmentShaderHandle);
    Globals::ReportGLError("glAttachShader");

    glBindAttribLocation(m_ShaderProgramHandle, 0, "frplPosition");
    glBindAttribLocation(m_ShaderProgramHandle, 1, "frplNormal");
    glBindAttribLocation(m_ShaderProgramHandle, 2, "frplTexcoord");
    Globals::ReportGLError("glBindAttribLocation");

    glBindFragDataLocation(m_ShaderProgramHandle, 0, "outColor");
    Globals::ReportGLError("glBindFragDataLocation");
    glLinkProgram(m_ShaderProgramHandle);
    Globals::ReportGLError("glLinkProgram");


    Use();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_ShaderProgramHandle);
    glDeleteShader(m_FragmentShaderHandle);
    glDeleteShader(m_VertexShaderHandle);
    std::cout << "Destroyed ShaderProgram" << std::endl;
}

GLuint ShaderProgram::CompileShader(std::string name, std::string shaderSource, ShaderType _shaderType)
{
    LogMessage("Compiling shader: \"" + name +"\"");

    GLenum shaderTypeId = 0;
    switch(_shaderType)
    {
        case ShaderType::Vertex:
            shaderTypeId = GL_VERTEX_SHADER;
            break;
        case ShaderType::Fragment:
            shaderTypeId = GL_FRAGMENT_SHADER;
            break;
        case ShaderType::None:
        default:
            LogError("Invalid ShaderType specified!");
            break;
    }

    Globals::ReportGLError("Compiling shader " + name);

    GLuint id = glCreateShader(shaderTypeId);

    const GLchar* sourceCStrings[] = { shaderSource.c_str() };

    glShaderSource(id, 1, sourceCStrings, NULL);
    glCompileShader(id);
    Globals::ReportGLError("glCompileShader " + name);

    // Check shader compilation
    GLint params;
    glGetShaderiv(id, GL_COMPILE_STATUS, &params);
    if(params == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog;
        infoLog.resize(infoLogLength + 1);
        glGetShaderInfoLog(id, infoLogLength, NULL, infoLog.data());
        std::string infoLogString = std::string(std::begin(infoLog), std::end(infoLog));

        std::string shaderType = _shaderType == ShaderType::Vertex ? "Vertex" : "Fragment";
        LogError(shaderType + " Shader: \"" + name + "\" compile failed. Log: \n" + infoLogString);

        if(Engine::GetInstance().GetCommandLineArgs().m_ShaderDebug)
        {
            assert(false);
        }
    }
    else
    {
        Globals::log("ShaderProgram", "Compilation complete");
    }
    //TODO: Decide what to do here if compilation fails, currently we continue after printing error

    return id;
}

std::pair<std::string, std::string> ShaderProgram::ParseShader(std::string filename)
{
    std::string currentLine;
    std::pair<std::string, std::string> result;
    std::fstream infile;
    infile.open(filename);

    // Checks if file is opened.
    if (!infile)
    {
        LogError("File \"" + filename + "\" cannot be opened.");
        return result;
    }

    ShaderType currentType = ShaderType::None;

    while(!infile.eof())
    {
        std::getline(infile, currentLine);

        if(currentLine[0] == '@')
        {
            if(currentLine == "@Vertex")
                currentType = ShaderType::Vertex;
            else if(currentLine == "@Fragment")
                currentType = ShaderType::Fragment;
            else
            {
                LogError("Shader parsing error. Unsupported identifier " + currentLine);
                currentType = ShaderType::None;
            }

            continue;
        }

        switch(currentType)
        {
            case ShaderType::None:
            {
                break;
            }
            case ShaderType::Vertex:
            {
                result.first = result.first + currentLine + "\n";
                break;
            }
            case ShaderType::Fragment:
            {
                result.second = result.second + currentLine + "\n";
                break;
            }
        }

    }
    infile.close();

    return result;
}

//TODO: Allow shader manipulation internally and do not expose GL details outside the class
GLuint ShaderProgram::GetProgramHandle() const
{
    return m_ShaderProgramHandle;
}

void ShaderProgram::SetUniformFloat3(std::string name, float r, float g, float b)
{
    Use();
    GLint uniform = glGetUniformLocation(m_ShaderProgramHandle, name.c_str());
    glUniform3f(uniform, r, g, b);
    Globals::ReportGLError("Setting uniform float3 \'" + name + "\'");

}

void ShaderProgram::SetUniformFloat3(std::string name, glm::vec3 vector)
{
    Use();
    GLint uniform = glGetUniformLocation(m_ShaderProgramHandle, name.c_str());
    glUniform3f(uniform, vector.x, vector.y, vector.z);
    Globals::ReportGLError("Setting uniform float3 \'" + name + "\'");

}

void ShaderProgram::SetUniformMat4(std::string name, glm::mat4 matrix)
{
    Use();
    GLint uniform = glGetUniformLocation(m_ShaderProgramHandle, name.c_str());
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
    Globals::ReportGLError("Setting uniform mat4 \'" + name + "\'");
}

void ShaderProgram::SetUniformFloat(std::string name, float value)
{
    Use();
    GLint uniform = glGetUniformLocation(m_ShaderProgramHandle, name.c_str());
    glUniform1f(uniform, value);
    Globals::ReportGLError("Setting uniform float \'" + name + "\'");
}

void ShaderProgram::Use() const
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glUseProgram(m_ShaderProgramHandle);
    Globals::ReportGLError("glUseProgram shaderProgram");
}

void ShaderProgram::SetUniformInt(std::string name, int value)
{
    Use();
    GLint uniform = glGetUniformLocation(m_ShaderProgramHandle, name.c_str());
    glUniform1i(uniform, value);
    Globals::ReportGLError("Setting uniform int \'" + name + "\'");
}
