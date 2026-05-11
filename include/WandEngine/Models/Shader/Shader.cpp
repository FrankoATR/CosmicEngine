#include "Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace WandEngine
{

    Shader::Shader()
    {

    }

    Shader::~Shader()
    {
        if (ID != 0) {
            glUseProgram(0);
            glDetachShader(this->ID, sVertex);
            glDeleteShader(sVertex);
            glDetachShader(this->ID, sFragment);
            glDeleteShader(sFragment);
            if (gShader != 0 )
            {
                glDetachShader(this->ID, gShader);
                glDeleteShader(gShader);
            }

            glDeleteShader(ID);
            ID = 0;
        }
    }

    void Shader::Compile(const char* vertexCode, const char* fragmentCode, const char* geometryCode)
    {
        sVertex = 0, sFragment = 0, gShader = 0;

        sVertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(sVertex, 1, &vertexCode, nullptr);
        glCompileShader(sVertex);
        checkCompileErrors(sVertex, "VERTEX");

        sFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(sFragment, 1, &fragmentCode, nullptr);
        glCompileShader(sFragment);
        checkCompileErrors(sFragment, "FRAGMENT");

        if (geometryCode != nullptr )
        {
            gShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gShader, 1, &geometryCode, nullptr);
            glCompileShader(gShader);
            checkCompileErrors(gShader, "GEOMETRY");
        }

        this->ID = glCreateProgram();
        glAttachShader(this->ID, sVertex);
        glAttachShader(this->ID, sFragment);
        if (geometryCode != nullptr )
            glAttachShader(this->ID, gShader);
        glLinkProgram(this->ID);
        checkCompileErrors(this->ID, "PROGRAM");



        std::cout << "Shader " << ID << " created" << std::endl;

    }

    void Shader::use()
    {
        glUseProgram(ID);
    }

    void Shader::endUse()
    {
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Shader::setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::setVec4(const std::string &name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec4(const std::string &name, float value1, float value2, float value3, float value4) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);

    }

    void Shader::setVec3(const std::string &name, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec3(const std::string &name, float value1, float value2, float value3) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
    }


    void Shader::setVec2(const std::string &name, glm::vec2 value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec2(const std::string &name, float value1, float value2) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
    }


    void Shader::setMatrix4(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
}