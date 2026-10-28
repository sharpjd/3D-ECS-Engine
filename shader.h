#pragma once

#include "libs/glad/glad.h" // include glad to get all the required OpenGL headers

#include <string>
#include <type_traits>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "loader.h"
#include "engineContext.h"
#include "debug.h"
#include <cassert>

/*
* Base class for all shader programs. It compiles and links the program, owns the GL object,
* and nothing else. Concrete shaders (see shaders.h) derive from it and declare their
* uniforms as Uniform<T> members:
*
*   struct MyShader : MCShader {
*       Uniform<glm::mat4> model{ *this, "model" };
*       MyShader(ShaderManager& sm) : MCShader(ShaderId::My, sm) {}
*   };
*
* The base class is fully constructed (program linked) before the derived members are, so each
* Uniform can look up its location right in its own constructor. No virtual calls are involved:
* after construction, setting a uniform is one inline call straight to glUniform*.
*/
class MCShader
{
public:
    MCShader(const MCShader&) = delete;
    MCShader& operator=(const MCShader&) = delete;

    virtual ~MCShader() {
        if (gl_programID != 0) glDeleteProgram(gl_programID);
    }

    void use() const { glUseProgram(gl_programID); }

    bool isCompiled() const { return compiled; }

    unsigned int gl_programID = 0; //the linked OpenGL program ID

protected:
    //abstract: only derived shaders can be created
    MCShader(ShaderId vertShader, ShaderId fragShader, ShaderManager& loader) { compile(vertShader, fragShader, loader); }
    MCShader(ShaderId shaderID, ShaderManager& loader) : MCShader(shaderID, shaderID, loader) {}

private:
    bool compiled = false;

    //returns 0 on failure
    static unsigned int compileStage(GLenum type, const char* source, const char* label) {
        unsigned int id = glCreateShader(type);
        glShaderSource(id, 1, &source, NULL);
        glCompileShader(id);

        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(id, sizeof(infoLog), NULL, infoLog);
            Debug::log(std::string("ERROR::SHADER::") + label + "::COMPILATION_FAILED\n" + infoLog);
            glDeleteShader(id);
            return 0;
        }
        return id;
    }

    void compile(ShaderId vertShader, ShaderId fragShader, ShaderManager& loader) {
        auto& vLoader = loader.vert.getLoadable(vertShader);
        auto& fLoader = loader.frag.getLoadable(fragShader);

        vLoader.load();
        fLoader.load();
        unsigned int vertID = compileStage(GL_VERTEX_SHADER, vLoader.get().c_str(), "VERTEX");
        unsigned int fragID = compileStage(GL_FRAGMENT_SHADER, fLoader.get().c_str(), "FRAGMENT");
        vLoader.unload();
        fLoader.unload();

        if (!vertID || !fragID) {
            glDeleteShader(vertID); //deleting 0 is silently ignored
            glDeleteShader(fragID);
            return;
        }

        gl_programID = glCreateProgram();
        glAttachShader(gl_programID, vertID);
        glAttachShader(gl_programID, fragID);
        glLinkProgram(gl_programID);

        int success;
        glGetProgramiv(gl_programID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(gl_programID, sizeof(infoLog), NULL, infoLog);
            Debug::log(std::string("ERROR::SHADER_PROGRAM::LINKING_FAILED\n") + infoLog);
        }

        //we don't need the vert/frag shader after we've linked them
        glDeleteShader(vertID);
        glDeleteShader(fragID);

        compiled = success;
    }
};

namespace detail {
    inline void uploadUniform(GLint loc, int v) { glUniform1i(loc, v); }
    inline void uploadUniform(GLint loc, unsigned int v) { glUniform1ui(loc, v); }
    inline void uploadUniform(GLint loc, bool v) { glUniform1i(loc, (int)v); }
    inline void uploadUniform(GLint loc, float v) { glUniform1f(loc, v); }
    inline void uploadUniform(GLint loc, const glm::vec2& v) { glUniform2fv(loc, 1, glm::value_ptr(v)); }
    inline void uploadUniform(GLint loc, const glm::vec3& v) { glUniform3fv(loc, 1, glm::value_ptr(v)); }
    inline void uploadUniform(GLint loc, const glm::vec4& v) { glUniform4fv(loc, 1, glm::value_ptr(v)); }
    inline void uploadUniform(GLint loc, const glm::mat3& v) { glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(v)); }
    inline void uploadUniform(GLint loc, const glm::mat4& v) { glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(v)); }
}

/*
* A typed handle to one uniform of one shader program: just the GL location (4 bytes).
* set() requires the owning shader to be bound (MCShader::use()), like any glUniform* call.
* An unsupported T fails to compile; a uniform missing from the GLSL (or optimized out)
* is logged once at construction and set() on it is a harmless no-op in GL (location -1).
*/
#ifdef _DEBUG
template <typename T>
class Uniform
{
public:
    Uniform(const MCShader& shader, const char* name)
        : location(glGetUniformLocation(shader.gl_programID, name)), 
            shader(shader)
    {
        if (location == -1) Debug::log(std::string("WARNING::SHADER::UNIFORM_NOT_FOUND ") + name);
    }

    void set(const T& v) const { 
        /*
        * this check costs quite a bit of performance but it'll save some headaches
        */
        GLint activeProgramID = -1;
        glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgramID);
        if (shader.gl_programID != activeProgramID) {
            Debug::log("Tried to set uniform for a program that isn't currently active");
            assert(false);
        }
        detail::uploadUniform(location, v); 
    }

    GLint loc() const { return location; }

private:
    GLint location;
    const MCShader& shader;
};
#else 
template <typename T>
class Uniform
{
public:
    Uniform(const MCShader& shader, const char* name)
        : location(glGetUniformLocation(shader.gl_programID, name))
    {
        if (location == -1) Debug::log(std::string("WARNING::SHADER::UNIFORM_NOT_FOUND ") + name);
    }

    void set(const T& v) const { detail::uploadUniform(location, v); }

    GLint loc() const { return location; }

private:
    GLint location;
};
#endif