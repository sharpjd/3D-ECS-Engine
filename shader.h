#pragma once

#include "libs/glad/glad.h" // include glad to get all the required OpenGL headers

#include "loader.h"
#include "engineContext.h"
#include "debug.h"

class Shader
{
private:
    const char* vertexShaderSource = nullptr;
    const char* fragShaderSource = nullptr;
public:
    unsigned int vertID; //the compiled OpenGL vertex shader ID
    unsigned int fragID; //the compiled OpenGL fragment shader ID
    unsigned int ID; //the compiled OpenGL shader ID

    ShaderId vertShader;
    ShaderId fragShader;
    ShaderFileLoader& loader;

    Shader(ShaderId vertShader, ShaderId fragShader, ShaderFileLoader& loader)
        : vertShader(vertShader), fragShader(fragShader), loader(loader)
    {
        compile();
    }

    Shader(ShaderId shaderID, ShaderFileLoader& loader) 
        : vertShader(shaderID), fragShader(shaderID), loader(loader)
    {
        compile();
    }


    void compile() {
        loader.vert.load(vertShader);
        loader.frag.load(fragShader);
        vertexShaderSource = loader.vert.getData(vertShader);
        fragShaderSource = loader.frag.getData(fragShader);


        vertID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertID, 1, &vertexShaderSource, NULL);
        glCompileShader(vertID);

        int  success;
        char infoLog[512];
        glGetShaderiv(vertID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertID, 512, NULL, infoLog);
            Debug::log("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
        }


        fragID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragID, 1, &fragShaderSource, NULL);
        glCompileShader(fragID);

        glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragID, 512, NULL, infoLog);
            Debug::log("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
        }


        ID = glCreateProgram();
        glAttachShader(ID, vertID);
        glAttachShader(ID, fragID);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            Debug::log("ERROR::SHADER_PROGAM::LINKING_FAILED");
        }


        //we don't need the vert/frag shader after we've linked them
        glDeleteShader(vertID);
        glDeleteShader(fragID);

        loader.vert.unload(vertShader);
        loader.frag.unload(fragShader);
    }
    
    //void setBool(const std::string& name, bool value) const
    //{
    //    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    //}
    //void setInt(const std::string& name, int value) const
    //{
    //    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    //}
    //void setFloat(const std::string& name, float value) const
    //{
    //    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    //}
};