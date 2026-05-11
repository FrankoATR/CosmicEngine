#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>

namespace WandEngine
{

    class Shader
    {
    private:
        void checkCompileErrors(unsigned int shader, std::string type);

    public:
        unsigned int ID;
        unsigned int sVertex, sFragment, gShader;

        Shader();
        ~Shader();

        void use();
        void endUse();

        void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        
        void setVec4(const std::string &name, glm::vec4 value) const;
        void setVec4(const std::string &name, float value1, float value2, float value3, float value4) const;

        void setVec3(const std::string &name, glm::vec3 value) const;
        void setVec3(const std::string &name, float value1, float value2, float value3) const;

        void setVec2(const std::string &name, glm::vec2 value) const;
        void setVec2(const std::string &name, float value1, float value2) const;

        void setMatrix4(const std::string &name, glm::mat4 value) const;
    };
}

#endif