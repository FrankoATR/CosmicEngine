#ifndef COSMIC_SHADER_HPP
#define COSMIC_SHADER_HPP

/**
 * @file shader.hpp
 * @brief Declares the shader wrapper used by the engine renderer.
 */

#include <glm/glm.hpp>
#include <string>

namespace CosmicEngine
{

    /**
     * @brief Identifies which engine projection context should be sent to a shader.
     */
    enum class ViewType{
        Ortho = 0,
        Projection = 1,
        UI = 2
    };

    /**
     * @brief Wraps an OpenGL shader program and its uniform helper functions.
     *
     * Shader encapsulates compiling, linking, and using an OpenGL shader program.
     * The engine creates and manages shaders internally through the ResourceManager;
     * game code typically does not instantiate Shader directly.  Use the ViewType
     * enum with SetProjection() to select between orthographic, perspective, and
     * UI projections.
     *
    * @par Example - Shader lifecycle (internal to ResourceManager)
     * @code
     * // Shaders are loaded internally from master_shaders.hpp source strings:
     * auto* shader = new CosmicEngine::Shader(vertexSrc, fragmentSrc);
     * shader->Use();
     * shader->SetProjection("projection", CosmicEngine::ViewType::Ortho);
     * shader->SetVec3("spriteColor", glm::vec3(1.0f));
     * // ... draw calls ...
     * shader->EndUse();
     * @endcode
     */
    class Shader
    {
    private:
        unsigned int sVertex, sFragment, gShader;
        unsigned int ID;
        void CheckCompileErrors(unsigned int shader, std::string type);
        void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource);

    public:
        /**
         * @brief Creates and compiles a shader program.
         * @param vertexSource Vertex shader source code.
         * @param fragmentSource Fragment shader source code.
         * @param geometrySource Optional geometry shader source code.
         * @throws std::runtime_error When shader compilation or program linking fails.
         */
        Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
        /** @brief Releases the shader program and owned shader objects. */
        ~Shader();

        /** @brief Binds the shader program for subsequent draw calls. */
        void Use();
        /** @brief Unbinds the current shader program. */
        void EndUse();

        /** @brief Returns the OpenGL program identifier. */
        unsigned int GetID();

        /**
         * @brief Sets a model matrix uniform from transform components.
         * @param name     Uniform variable name in the shader program.
         * @param position World-space translation applied to the model.
         * @param size     Per-axis scale applied to the model.
         * @param rotation Euler rotation in degrees.
         */
        void SetModel(const std::string &name, glm::vec3 position, glm::vec3 size, glm::vec3 rotation) const;
        /**
         * @brief Sets a model matrix uniform from transform components and an explicit pivot.
         * @param name     Uniform variable name in the shader program.
         * @param position World-space translation applied to the model.
         * @param size     Per-axis scale applied to the model.
         * @param rotation Euler rotation in degrees.
         * @param pivot    Local-space pivot used as the rotation/scale center.
         */
        void SetModel(const std::string &name, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 pivot) const;
        /**
         * @brief Sets a projection matrix uniform using one of the engine view types.
         * @param name     Uniform variable name in the shader program.
         * @param viewType One of the engine ViewType values (perspective / orthographic / etc.).
         */
        void SetProjection(const std::string &name, ViewType viewType) const;

        /**
         * @brief Sets a boolean uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Boolean value to upload.
         */
        void SetBool(const std::string &name, bool value) const;
        /**
         * @brief Sets an integer uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Integer value to upload.
         */
        void SetInt(const std::string &name, int value) const;
        /**
         * @brief Sets a floating-point uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Floating-point value to upload.
         */
        void SetFloat(const std::string &name, float value) const;
        
        /**
         * @brief Sets a four-component vector uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Vector value to upload.
         */
        void SetVec4(const std::string &name, glm::vec4 value) const;
        /**
         * @brief Sets a four-component vector uniform from scalar values.
         * @param name   Uniform variable name in the shader program.
         * @param value1 X component.
         * @param value2 Y component.
         * @param value3 Z component.
         * @param value4 W component.
         */
        void SetVec4(const std::string &name, float value1, float value2, float value3, float value4) const;

        /**
         * @brief Sets a three-component vector uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Vector value to upload.
         */
        void SetVec3(const std::string &name, glm::vec3 value) const;
        /**
         * @brief Sets a three-component vector uniform from scalar values.
         * @param name   Uniform variable name in the shader program.
         * @param value1 X component.
         * @param value2 Y component.
         * @param value3 Z component.
         */
        void SetVec3(const std::string &name, float value1, float value2, float value3) const;

        /**
         * @brief Sets a two-component vector uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Vector value to upload.
         */
        void SetVec2(const std::string &name, glm::vec2 value) const;
        /**
         * @brief Sets a two-component vector uniform from scalar values.
         * @param name   Uniform variable name in the shader program.
         * @param value1 X component.
         * @param value2 Y component.
         */
        void SetVec2(const std::string &name, float value1, float value2) const;

        /**
         * @brief Sets a 4x4 matrix uniform.
         * @param name  Uniform variable name in the shader program.
         * @param value Matrix value to upload.
         */
        void SetMatrix4(const std::string &name, glm::mat4 value) const;
    };
}

#endif // COSMIC_SHADER_HPP
