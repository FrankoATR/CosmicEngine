#include "Shader.hpp"

#include "../../Managers/Camera/CameraManager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace WandEngine
{

    Shader::Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
    {
        Compile(vertexSource, fragmentSource, geometrySource);
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

    void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
    {
        sVertex = 0, sFragment = 0, gShader = 0;

        sVertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(sVertex, 1, &vertexSource, nullptr);
        glCompileShader(sVertex);
        CheckCompileErrors(sVertex, "VERTEX");

        sFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(sFragment, 1, &fragmentSource, nullptr);
        glCompileShader(sFragment);
        CheckCompileErrors(sFragment, "FRAGMENT");

        if (geometrySource != nullptr )
        {
            std::cout << "GEO" << std::endl;
            gShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gShader, 1, &geometrySource, nullptr);
            glCompileShader(gShader);
            CheckCompileErrors(gShader, "GEOMETRY");
        }

        this->ID = glCreateProgram();
        glAttachShader(this->ID, sVertex);
        glAttachShader(this->ID, sFragment);
        if (geometrySource != nullptr )
            glAttachShader(this->ID, gShader);
        glLinkProgram(this->ID);
        CheckCompileErrors(this->ID, "PROGRAM");

        std::cout << "Shader " << ID << " created" << std::endl;

    }

    void Shader::Use()
    {
        glUseProgram(ID);
    }

    void Shader::EndUse()
    {
        glUseProgram(0);
    }

    unsigned int Shader::GetID()
    {
        return ID;
    }

    void Shader::SetModel(const std::string &name, glm::vec3 position, glm::vec3 size, glm::vec3 rotation) const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);

        model = glm::translate(model, glm::vec3(size.x * 0.5, size.y * 0.5, size.z * 0.5f));

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::translate(model, glm::vec3(-size.x * 0.5, -size.y * 0.5, -size.z * 0.5f));

        model = glm::scale(model, size);

        SetMatrix4(name, model);
    }

    void Shader::SetModel(const std::string &name, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 pivot) const
    {
        glm::mat4 model(1.0f);

        model = glm::translate(model, pivot);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, position - pivot);
        model = glm::scale(model, size);
        
        SetMatrix4(name, model);
        
    }


    void Shader::SetProjection(const std::string &name, ViewType viewType) const
    {
        glm::mat4 projection;
        if(viewType == ViewType::Ortho)
            projection = CameraManager::GetInstance().GetProjectionMatrix();
        else if(viewType == ViewType::Projection)
            projection = CameraManager::GetInstance().GetProjectionMatrix();
        else if(viewType == ViewType::UI)
            projection = CameraManager::GetInstance().Get_UI_ProjectionMatrix();
        
        SetMatrix4(name, projection);
    }

    void Shader::SetBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::SetInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::SetFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::SetVec4(const std::string &name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4(const std::string &name, float value1, float value2, float value3, float value4) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);

    }

    void Shader::SetVec3(const std::string &name, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetVec3(const std::string &name, float value1, float value2, float value3) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
    }


    void Shader::SetVec2(const std::string &name, glm::vec2 value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetVec2(const std::string &name, float value1, float value2) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
    }


    void Shader::SetMatrix4(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::CheckCompileErrors(unsigned int shader, std::string type)
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