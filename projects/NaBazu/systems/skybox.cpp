#include "skybox.hpp"

#include <CosmicEngine/models/shader/shader.hpp>

#include <CosmicEngine/interfaces/definitions.hpp>
#include CAMERAMANAGER_HEADER

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace
{
    const char *kSkyboxVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vDir;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vDir = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
)GLSL";

    // Twinkling starfield + two drifting nebula/galaxy blobs, entirely procedural
    // (hash + value-noise fbm evaluated on the view direction) -- no textures, so
    // there is no fixed resolution to run out of, and no seams between cube faces
    // since everything is a function of the 3D direction rather than per-face UVs.
    const char *kSkyboxFragmentSource = R"GLSL(
#version 330 core
in vec3 vDir;
out vec4 FragColor;

uniform float uTime;

float hash13(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return fract((p.x + p.y) * p.z);
}

float valueNoise3D(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float n000 = hash13(i + vec3(0.0, 0.0, 0.0));
    float n100 = hash13(i + vec3(1.0, 0.0, 0.0));
    float n010 = hash13(i + vec3(0.0, 1.0, 0.0));
    float n110 = hash13(i + vec3(1.0, 1.0, 0.0));
    float n001 = hash13(i + vec3(0.0, 0.0, 1.0));
    float n101 = hash13(i + vec3(1.0, 0.0, 1.0));
    float n011 = hash13(i + vec3(0.0, 1.0, 1.0));
    float n111 = hash13(i + vec3(1.0, 1.0, 1.0));

    float nx00 = mix(n000, n100, f.x);
    float nx10 = mix(n010, n110, f.x);
    float nx01 = mix(n001, n101, f.x);
    float nx11 = mix(n011, n111, f.x);

    float nxy0 = mix(nx00, nx10, f.y);
    float nxy1 = mix(nx01, nx11, f.y);

    return mix(nxy0, nxy1, f.z);
}

float fbm(vec3 p)
{
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; ++i)
    {
        value += amplitude * valueNoise3D(p);
        p *= 2.02;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    vec3 dir = normalize(vDir);

    vec3 baseColor = mix(vec3(0.010, 0.010, 0.028), vec3(0.018, 0.018, 0.050), dir.y * 0.5 + 0.5);

    // Nebula A (purple/blue), drifts slowly with uTime.
    vec3 nebulaP = dir * 3.0 + vec3(7.0, 3.0, 11.0) + vec3(uTime * 0.003, 0.0, uTime * 0.002);
    float n1 = fbm(nebulaP);
    float n2 = fbm(nebulaP * 1.7 + vec3(31.0, 5.0, 9.0));
    float nebulaMask = smoothstep(0.55, 0.85, n1) * smoothstep(0.30, 0.90, n2);
    vec3 nebulaColorA = vec3(0.35, 0.15, 0.55);
    vec3 nebulaColorB = vec3(0.10, 0.30, 0.55);
    vec3 nebulaColor = mix(nebulaColorA, nebulaColorB, n2) * nebulaMask * 0.9;

    // Nebula B (amber/gold), fixed patch elsewhere in the sky -- a second "galaxy".
    vec3 nebulaP2 = dir * 4.0 + vec3(-19.0, 41.0, -6.0);
    float n3 = fbm(nebulaP2);
    float nebulaMask2 = smoothstep(0.60, 0.90, n3);
    vec3 nebulaColor2 = vec3(0.55, 0.35, 0.20) * nebulaMask2 * 0.7;

    // Sparse stars: one hashed candidate per direction-space cell, each with its
    // own twinkle frequency/phase/brightness so they don't blink in lockstep.
    vec3 starCell = dir * 220.0;
    vec3 cellId = floor(starCell);
    float starChance = hash13(cellId);
    vec3 starColor = vec3(0.0);
    if (starChance > 0.986)
    {
        vec3 cellFrac = fract(starCell) - 0.5;
        float d = length(cellFrac);
        float starCore = smoothstep(0.10, 0.0, d);
        float twinklePhase = hash13(cellId + 91.7) * 6.2831853;
        float twinkleFreq = 1.5 + hash13(cellId + 3.1) * 2.0;
        float twinkle = 0.55 + 0.45 * sin(uTime * twinkleFreq + twinklePhase);
        float brightness = hash13(cellId + 5.5) * 0.6 + 0.4;
        starColor = vec3(starCore * brightness * twinkle);
        starColor *= mix(vec3(0.8, 0.85, 1.0), vec3(1.0, 0.85, 0.7), hash13(cellId + 8.2));
    }

    vec3 color = baseColor + nebulaColor + nebulaColor2 + starColor;
    FragColor = vec4(color, 1.0);
}
)GLSL";

    // Unit cube (-0.5..0.5), position-only, 36 verts (6 faces x 2 tris x 3 verts) --
    // matches the vertex count Get_Static_VAO("COSMIC_Parallelepiped") uses internally,
    // but we build our own since that one interleaves normal/texcoord data our shader
    // doesn't use and Get_Static_VAO is a private ResourceManager helper anyway.
    const float kSkyboxVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
    };
}

namespace NaBazu
{
    Skybox::Skybox()
        : shader_(new CosmicEngine::Shader(kSkyboxVertexSource, kSkyboxFragmentSource)),
          vao_(0),
          vbo_(0)
    {
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kSkyboxVertices), kSkyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(0));

        glBindVertexArray(0);
    }

    Skybox::~Skybox()
    {
        delete shader_;
        if (vbo_ != 0)
        {
            glDeleteBuffers(1, &vbo_);
        }
        if (vao_ != 0)
        {
            glDeleteVertexArrays(1, &vao_);
        }
    }

    void Skybox::Draw(float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        // Depth buffer is cleared to 1.0 (far) every frame and the default depth
        // func is GL_LESS, so our xyww-forced depth of exactly 1.0 would never pass
        // ("1.0 < 1.0" is false) -- LEQUAL is required for the skybox to render at
        // all. Scene::DrawManagers() never restores depth state itself and calls
        // ObjectManager::draw() (ship/enemies/bullets) right after this, so both
        // glDepthFunc and glDepthMask must be restored before returning.
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        // Strip translation from the view matrix (cast to mat3 and back) so the
        // skybox rotates with the camera but never translates with it -- the
        // standard "infinite distance" skybox technique.
        const glm::mat4 rotationOnlyView = glm::mat4(glm::mat3(CAM_MN.GetViewMatrix()));

        shader_->Use();
        shader_->SetMatrix4("view", rotationOnlyView);
        shader_->SetMatrix4("projection", CAM_MN.GetProjectionMatrix());
        shader_->SetFloat("uTime", elapsedTime);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        shader_->EndUse();

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }
}
