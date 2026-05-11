#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>

namespace WandEngine
{

    enum class ViewType{
        Ortho = 0,
        Projection = 1,
        UI = 2
    };

    class Shader
    {
    private:
        unsigned int sVertex, sFragment, gShader;
        unsigned int ID;
        void CheckCompileErrors(unsigned int shader, std::string type);
        void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource);

    public:
        Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
        ~Shader();

        void Use();
        void EndUse();

        unsigned int GetID();

        void SetModel(const std::string &name, glm::vec3 position, glm::vec3 size, glm::vec3 rotation) const;
        void SetModel(const std::string &name, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 pivot) const;
        void SetProjection(const std::string &name, ViewType viewType) const;

        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetFloat(const std::string &name, float value) const;
        
        void SetVec4(const std::string &name, glm::vec4 value) const;
        void SetVec4(const std::string &name, float value1, float value2, float value3, float value4) const;

        void SetVec3(const std::string &name, glm::vec3 value) const;
        void SetVec3(const std::string &name, float value1, float value2, float value3) const;

        void SetVec2(const std::string &name, glm::vec2 value) const;
        void SetVec2(const std::string &name, float value1, float value2) const;

        void SetMatrix4(const std::string &name, glm::mat4 value) const;
    };
}

#endif