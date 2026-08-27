#include "glow_mesh_renderer.hpp"

#include "../utilities/mesh_gen.hpp"

#include <CosmicEngine/models/shader/shader.hpp>

#include <CosmicEngine/interfaces/definitions.hpp>
#include CAMERAMANAGER_HEADER

#include <glad/glad.h>

namespace
{
    unsigned int BuildPositionOnlyVao(const std::vector<float> &vertices, unsigned int &outVbo)
    {
        unsigned int vao = 0;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &outVbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, outVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(0));

        glBindVertexArray(0);
        return vao;
    }

    // ---- Bolt (player/enemy bullets): radial glow from the shape's own axis ----

    const char *kBoltVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vLocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLocalPos = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)GLSL";

    const char *kBoltFragmentSource = R"GLSL(
#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform float uTime;

void main()
{
    float axisDist = clamp(length(vLocalPos.xy) / 0.22, 0.0, 1.0);
    float glowCore = pow(1.0 - axisDist, 1.6);
    float shimmer = 0.85 + 0.15 * sin(uTime * 18.0 + vLocalPos.z * 12.0);

    vec3 hotColor = mix(uColor, vec3(1.0), glowCore * 0.6);
    vec3 color = hotColor * (0.55 + 0.65 * glowCore) * shimmer;

    FragColor = vec4(color, 1.0);
}
)GLSL";

    // ---- Orb (missile): fresnel rim glow + slow pulse ----

    const char *kOrbVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vLocalPos;
out vec3 vWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLocalPos = aPos;
    vec4 worldPos = model * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}
)GLSL";

    const char *kOrbFragmentSource = R"GLSL(
#version 330 core
in vec3 vLocalPos;
in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCameraPos;
uniform float uTime;

void main()
{
    vec3 normal = normalize(vLocalPos);
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    float fresnel = pow(clamp(1.0 - max(dot(normal, viewDir), 0.0), 0.0, 1.0), 2.2);
    float pulse = 0.85 + 0.15 * sin(uTime * 6.0);

    vec3 core = mix(uColor, vec3(1.0), 0.5);
    vec3 rim = vec3(1.0, 0.95, 0.85);
    vec3 color = mix(core, rim, fresnel) * pulse * 1.3;

    FragColor = vec4(color, 1.0);
}
)GLSL";

    // ---- Star pickups: iridescent tip-to-center glow with a shifting shimmer ----

    const char *kStarVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vLocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLocalPos = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)GLSL";

    const char *kStarFragmentSource = R"GLSL(
#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform float uTime;

void main()
{
    // 0 at the star's center, ~1 at the tips (outer radius is 0.5 in local space).
    float radial = clamp(length(vLocalPos.xy) / 0.5, 0.0, 1.0);

    // Bright hot core fading toward the saturated base color at the tips.
    vec3 core = mix(vec3(1.0), uColor, smoothstep(0.0, 0.75, radial));

    // Angular shimmer sweeping around the star plus an overall pulse, so it visibly
    // sparkles while spinning instead of looking like flat plastic.
    float angle = atan(vLocalPos.y, vLocalPos.x);
    float sparkle = 0.5 + 0.5 * sin(angle * 3.0 + uTime * 5.0);
    float pulse = 0.85 + 0.15 * sin(uTime * 4.0);

    vec3 color = core * (0.75 + 0.55 * sparkle) * pulse;
    FragColor = vec4(color, 1.0);
}
)GLSL";

    // ---- Ring pickup: glowing torus with an energy band travelling around it ----

    const char *kRingVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vLocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLocalPos = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)GLSL";

    const char *kRingFragmentSource = R"GLSL(
#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform float uTime;

void main()
{
    // Angle around the ring drives a bright band that sweeps continuously,
    // reading as energy circulating through the ring.
    float angle = atan(vLocalPos.y, vLocalPos.x);
    float band = pow(0.5 + 0.5 * sin(angle * 2.0 - uTime * 3.5), 3.0);

    // Distance from the tube's center circle: 0 on the tube axis, 1 at its surface.
    // Brightens the tube's outer surface so the ring keeps a solid volumetric look.
    float ringRadius = 0.39;
    float radialDist = length(vLocalPos.xy) - ringRadius;
    float tubeDepth = clamp(length(vec2(radialDist, vLocalPos.z)) / 0.11, 0.0, 1.0);

    float pulse = 0.85 + 0.15 * sin(uTime * 3.0);
    vec3 base = mix(uColor, vec3(1.0, 0.85, 0.85), 0.35 * tubeDepth);
    vec3 color = base * (0.7 + 0.9 * band) * pulse;

    FragColor = vec4(color, 1.0);
}
)GLSL";

    // ---- Crosshair (aim reticle): pulsing glow, hotter/faster when locked on ----

    const char *kCrosshairVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vLocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLocalPos = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)GLSL";

    const char *kCrosshairFragmentSource = R"GLSL(
#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform float uTime;
uniform float uLockOn;

void main()
{
    float pulseFreq = mix(3.0, 10.0, uLockOn);
    float pulse = 0.75 + 0.25 * sin(uTime * pulseFreq);

    vec3 lockedColor = vec3(1.0, 0.25, 0.2);
    vec3 color = mix(uColor, lockedColor, uLockOn) * (0.9 + 0.5 * uLockOn) * pulse;

    FragColor = vec4(color, 1.0);
}
)GLSL";

    // ---- Explosion: turbulent-noise fireball, cools and fades over lifeFraction ----

    const char *kExplosionVertexSource = R"GLSL(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vLocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLocalPos = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)GLSL";

    const char *kExplosionFragmentSource = R"GLSL(
#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform float uTime;
uniform float uLifeFraction;

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
    for (int i = 0; i < 4; ++i)
    {
        value += amplitude * valueNoise3D(p);
        p *= 2.1;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    vec3 n = normalize(vLocalPos);
    float turbulence = fbm(n * 3.0 + vec3(uTime * 1.6, uTime * 1.1, -uTime * 1.3));
    float shell = smoothstep(0.15, 0.55, turbulence + (1.0 - uLifeFraction) * 0.35);

    vec3 hot = vec3(1.0, 0.95, 0.75);
    vec3 mid = vec3(1.0, 0.45, 0.1);
    vec3 cool = vec3(0.35, 0.08, 0.05);

    vec3 color = mix(cool, mid, smoothstep(0.0, 0.6, turbulence));
    color = mix(color, hot, smoothstep(0.55, 0.95, turbulence) * (1.0 - uLifeFraction));

    float alpha = (1.0 - uLifeFraction) * (0.45 + 0.55 * shell);
    FragColor = vec4(color, clamp(alpha, 0.0, 1.0));
}
)GLSL";
}

namespace NaBazu
{
    // ---- BoltGlowRenderer ----

    BoltGlowRenderer &BoltGlowRenderer::GetInstance()
    {
        static BoltGlowRenderer instance;
        return instance;
    }

    BoltGlowRenderer::BoltGlowRenderer()
        : shader_(new CosmicEngine::Shader(kBoltVertexSource, kBoltFragmentSource)),
          vao_(0),
          vbo_(0),
          vertexCount_(0)
    {
        const std::vector<float> vertices = BuildUnitBoltVertices();
        vertexCount_ = static_cast<int>(vertices.size() / 3);
        vao_ = BuildPositionOnlyVao(vertices, vbo_);
    }

    BoltGlowRenderer::~BoltGlowRenderer()
    {
        delete shader_;
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void BoltGlowRenderer::Draw(glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 color, float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        shader_->Use();
        shader_->SetModel("model", position, size, rotation, position);
        shader_->SetMatrix4("view", CosmicEngine::CameraManager::GetInstance().GetViewMatrix());
        shader_->SetMatrix4("projection", CosmicEngine::CameraManager::GetInstance().GetProjectionMatrix());
        shader_->SetVec3("uColor", color);
        shader_->SetFloat("uTime", elapsedTime);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
        glBindVertexArray(0);

        shader_->EndUse();
    }

    // ---- OrbGlowRenderer ----

    OrbGlowRenderer &OrbGlowRenderer::GetInstance()
    {
        static OrbGlowRenderer instance;
        return instance;
    }

    OrbGlowRenderer::OrbGlowRenderer()
        : shader_(new CosmicEngine::Shader(kOrbVertexSource, kOrbFragmentSource)),
          vao_(0),
          vbo_(0),
          vertexCount_(0)
    {
        const std::vector<float> vertices = BuildUnitSphereVertices();
        vertexCount_ = static_cast<int>(vertices.size() / 3);
        vao_ = BuildPositionOnlyVao(vertices, vbo_);
    }

    OrbGlowRenderer::~OrbGlowRenderer()
    {
        delete shader_;
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void OrbGlowRenderer::Draw(glm::vec3 position, glm::vec3 size, glm::vec3 color, float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        auto &camera = CosmicEngine::CameraManager::GetInstance();

        shader_->Use();
        shader_->SetModel("model", position, size, glm::vec3(0.0f), position);
        shader_->SetMatrix4("view", camera.GetViewMatrix());
        shader_->SetMatrix4("projection", camera.GetProjectionMatrix());
        shader_->SetVec3("uColor", color);
        shader_->SetVec3("uCameraPos", camera.GetPosition());
        shader_->SetFloat("uTime", elapsedTime);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
        glBindVertexArray(0);

        shader_->EndUse();
    }

    // ---- StarPickupRenderer ----

    StarPickupRenderer &StarPickupRenderer::GetInstance()
    {
        static StarPickupRenderer instance;
        return instance;
    }

    StarPickupRenderer::StarPickupRenderer()
        : shader_(new CosmicEngine::Shader(kStarVertexSource, kStarFragmentSource)),
          vaoStar3_(0),
          vboStar3_(0),
          vertexCountStar3_(0),
          vaoStar5_(0),
          vboStar5_(0),
          vertexCountStar5_(0)
    {
        // A 3-pointed star needs a deeper valley (smaller innerRatio) to still read as
        // a star rather than a rounded triangle; 5 points look right at the default.
        const std::vector<float> star3 = BuildUnitStarVertices(3, 0.30f);
        vertexCountStar3_ = static_cast<int>(star3.size() / 3);
        vaoStar3_ = BuildPositionOnlyVao(star3, vboStar3_);

        const std::vector<float> star5 = BuildUnitStarVertices(5, 0.42f);
        vertexCountStar5_ = static_cast<int>(star5.size() / 3);
        vaoStar5_ = BuildPositionOnlyVao(star5, vboStar5_);
    }

    StarPickupRenderer::~StarPickupRenderer()
    {
        delete shader_;
        if (vboStar3_ != 0) glDeleteBuffers(1, &vboStar3_);
        if (vaoStar3_ != 0) glDeleteVertexArrays(1, &vaoStar3_);
        if (vboStar5_ != 0) glDeleteBuffers(1, &vboStar5_);
        if (vaoStar5_ != 0) glDeleteVertexArrays(1, &vaoStar5_);
    }

    void StarPickupRenderer::Draw(int pointCount, glm::vec3 position, glm::vec3 size, glm::vec3 rotation,
                                   glm::vec3 color, float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        const bool useThreePoint = (pointCount == 3);
        const unsigned int vao = useThreePoint ? vaoStar3_ : vaoStar5_;
        const int vertexCount = useThreePoint ? vertexCountStar3_ : vertexCountStar5_;

        shader_->Use();
        shader_->SetModel("model", position, size, rotation, position);
        shader_->SetMatrix4("view", CosmicEngine::CameraManager::GetInstance().GetViewMatrix());
        shader_->SetMatrix4("projection", CosmicEngine::CameraManager::GetInstance().GetProjectionMatrix());
        shader_->SetVec3("uColor", color);
        shader_->SetFloat("uTime", elapsedTime);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);

        shader_->EndUse();
    }

    // ---- RingPickupRenderer ----

    RingPickupRenderer &RingPickupRenderer::GetInstance()
    {
        static RingPickupRenderer instance;
        return instance;
    }

    RingPickupRenderer::RingPickupRenderer()
        : shader_(new CosmicEngine::Shader(kRingVertexSource, kRingFragmentSource)),
          vao_(0),
          vbo_(0),
          vertexCount_(0)
    {
        const std::vector<float> vertices = BuildUnitRingVertices();
        vertexCount_ = static_cast<int>(vertices.size() / 3);
        vao_ = BuildPositionOnlyVao(vertices, vbo_);
    }

    RingPickupRenderer::~RingPickupRenderer()
    {
        delete shader_;
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void RingPickupRenderer::Draw(glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 color, float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        shader_->Use();
        shader_->SetModel("model", position, size, rotation, position);
        shader_->SetMatrix4("view", CosmicEngine::CameraManager::GetInstance().GetViewMatrix());
        shader_->SetMatrix4("projection", CosmicEngine::CameraManager::GetInstance().GetProjectionMatrix());
        shader_->SetVec3("uColor", color);
        shader_->SetFloat("uTime", elapsedTime);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
        glBindVertexArray(0);

        shader_->EndUse();
    }

    // ---- CrosshairRenderer ----

    CrosshairRenderer &CrosshairRenderer::GetInstance()
    {
        static CrosshairRenderer instance;
        return instance;
    }

    CrosshairRenderer::CrosshairRenderer()
        : shader_(new CosmicEngine::Shader(kCrosshairVertexSource, kCrosshairFragmentSource)),
          vao_(0),
          vbo_(0),
          vertexCount_(0)
    {
        const std::vector<float> vertices = BuildUnitCrosshairVertices();
        vertexCount_ = static_cast<int>(vertices.size() / 3);
        vao_ = BuildPositionOnlyVao(vertices, vbo_);
    }

    CrosshairRenderer::~CrosshairRenderer()
    {
        delete shader_;
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void CrosshairRenderer::Draw(glm::vec3 position, glm::vec3 size, glm::vec3 color, float lockOn, float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        shader_->Use();
        shader_->SetModel("model", position, size, glm::vec3(0.0f), position);
        shader_->SetMatrix4("view", CosmicEngine::CameraManager::GetInstance().GetViewMatrix());
        shader_->SetMatrix4("projection", CosmicEngine::CameraManager::GetInstance().GetProjectionMatrix());
        shader_->SetVec3("uColor", color);
        shader_->SetFloat("uTime", elapsedTime);
        shader_->SetFloat("uLockOn", glm::clamp(lockOn, 0.0f, 1.0f));

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
        glBindVertexArray(0);

        shader_->EndUse();
    }

    // ---- ExplosionRenderer ----

    ExplosionRenderer &ExplosionRenderer::GetInstance()
    {
        static ExplosionRenderer instance;
        return instance;
    }

    ExplosionRenderer::ExplosionRenderer()
        : shader_(new CosmicEngine::Shader(kExplosionVertexSource, kExplosionFragmentSource)),
          vao_(0),
          vbo_(0),
          vertexCount_(0)
    {
        const std::vector<float> vertices = BuildUnitSphereVertices();
        vertexCount_ = static_cast<int>(vertices.size() / 3);
        vao_ = BuildPositionOnlyVao(vertices, vbo_);
    }

    ExplosionRenderer::~ExplosionRenderer()
    {
        delete shader_;
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void ExplosionRenderer::Draw(glm::vec3 position, float scale, float lifeFraction, float elapsedTime) const
    {
        if (!shader_)
        {
            return;
        }

        shader_->Use();
        shader_->SetModel("model", position, glm::vec3(scale), glm::vec3(0.0f), position);
        shader_->SetMatrix4("view", CosmicEngine::CameraManager::GetInstance().GetViewMatrix());
        shader_->SetMatrix4("projection", CosmicEngine::CameraManager::GetInstance().GetProjectionMatrix());
        shader_->SetFloat("uTime", elapsedTime);
        shader_->SetFloat("uLifeFraction", glm::clamp(lifeFraction, 0.0f, 1.0f));

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
        glBindVertexArray(0);

        shader_->EndUse();
    }
}
