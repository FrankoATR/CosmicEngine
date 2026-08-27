#ifndef NABAZU_GLOW_MESH_RENDERER_HPP
#define NABAZU_GLOW_MESH_RENDERER_HPP

#include <glm/glm.hpp>

namespace CosmicEngine
{
    class Shader;
}

namespace NaBazu
{
    // Shared, lazily-built shader+mesh for player/enemy bullets: a tapered "bolt"
    // (bipyramid, see utilities/mesh_gen.hpp) mesh with a radial-glow emissive
    // fragment shader. One GPU shader program and VAO/VBO for the whole game --
    // Bullet/EnemyBullet call Draw() each frame with their own transform/tint rather
    // than each instance owning a shader/mesh copy (there can be many on screen).
    // Meyer's-singleton style (GetInstance()), matching the engine's own manager
    // convention, so no constructor-injection is needed at every spawn call site.
    class BoltGlowRenderer
    {
    public:
        static BoltGlowRenderer &GetInstance();

        BoltGlowRenderer(const BoltGlowRenderer &) = delete;
        BoltGlowRenderer &operator=(const BoltGlowRenderer &) = delete;

        // rotation: Euler degrees, same convention as CosmicEngine::Object::GetRotation().
        void Draw(glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 color, float elapsedTime) const;

    private:
        BoltGlowRenderer();
        ~BoltGlowRenderer();

        CosmicEngine::Shader *shader_;
        unsigned int vao_;
        unsigned int vbo_;
        int vertexCount_;
    };

    // Shared shader+mesh for a glowing sphere ("orb"): used directly by Missile for
    // the strong area-damage shot. Fresnel-style rim brightening + a slow pulse.
    class OrbGlowRenderer
    {
    public:
        static OrbGlowRenderer &GetInstance();

        OrbGlowRenderer(const OrbGlowRenderer &) = delete;
        OrbGlowRenderer &operator=(const OrbGlowRenderer &) = delete;

        void Draw(glm::vec3 position, glm::vec3 size, glm::vec3 color, float elapsedTime) const;

    private:
        OrbGlowRenderer();
        ~OrbGlowRenderer();

        CosmicEngine::Shader *shader_;
        unsigned int vao_;
        unsigned int vbo_;
        int vertexCount_;
    };

    // Shared shader + two star meshes for collectible pickups: a 3-pointed star for
    // ammo and a 5-pointed star for missile ammo (see BuildUnitStarVertices). One
    // shader program drives both; only the VAO and tint differ, so Pickup just picks
    // which star to draw. The fragment shader gives them an iridescent shifting glow
    // (tip-to-center gradient + time-varying hue shimmer) so they read as "collect me"
    // rather than as flat-colored geometry.
    class StarPickupRenderer
    {
    public:
        static StarPickupRenderer &GetInstance();

        StarPickupRenderer(const StarPickupRenderer &) = delete;
        StarPickupRenderer &operator=(const StarPickupRenderer &) = delete;

        // pointCount: 3 (ammo) or 5 (missile); any other value falls back to 5.
        void Draw(int pointCount, glm::vec3 position, glm::vec3 size, glm::vec3 rotation,
                   glm::vec3 color, float elapsedTime) const;

    private:
        StarPickupRenderer();
        ~StarPickupRenderer();

        CosmicEngine::Shader *shader_;
        unsigned int vaoStar3_;
        unsigned int vboStar3_;
        int vertexCountStar3_;
        unsigned int vaoStar5_;
        unsigned int vboStar5_;
        int vertexCountStar5_;
    };

    // Shared shader + torus mesh for the health pickup: a glowing ring with an energy
    // band travelling around it, so it reads as a distinct "restore" item rather than
    // just another star.
    class RingPickupRenderer
    {
    public:
        static RingPickupRenderer &GetInstance();

        RingPickupRenderer(const RingPickupRenderer &) = delete;
        RingPickupRenderer &operator=(const RingPickupRenderer &) = delete;

        void Draw(glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec3 color, float elapsedTime) const;

    private:
        RingPickupRenderer();
        ~RingPickupRenderer();

        CosmicEngine::Shader *shader_;
        unsigned int vao_;
        unsigned int vbo_;
        int vertexCount_;
    };

    // Shared shader+mesh for the aim reticle: a "+" crosshair (see
    // BuildUnitCrosshairVertices) instead of a plain cube. lockOn (0..1) blends the
    // color toward a hot "target acquired" tint and speeds up its pulse; Ship also
    // shrinks the size it passes in as lockOn rises, so the lock feedback reads both
    // through motion (shrink) and color.
    class CrosshairRenderer
    {
    public:
        static CrosshairRenderer &GetInstance();

        CrosshairRenderer(const CrosshairRenderer &) = delete;
        CrosshairRenderer &operator=(const CrosshairRenderer &) = delete;

        void Draw(glm::vec3 position, glm::vec3 size, glm::vec3 color, float lockOn, float elapsedTime) const;

    private:
        CrosshairRenderer();
        ~CrosshairRenderer();

        CosmicEngine::Shader *shader_;
        unsigned int vao_;
        unsigned int vbo_;
        int vertexCount_;
    };

    // Turbulent-noise fireball shader for ExplosionEffect (both enemy deaths and
    // missile impacts funnel through ExplosionEffect already). Reuses the same sphere
    // geometry as OrbGlowRenderer but owns an independent VBO/VAO and a different
    // (noise-driven, lifetime-fading) fragment shader.
    class ExplosionRenderer
    {
    public:
        static ExplosionRenderer &GetInstance();

        ExplosionRenderer(const ExplosionRenderer &) = delete;
        ExplosionRenderer &operator=(const ExplosionRenderer &) = delete;

        // lifeFraction: 0 at birth, 1 at death (drives cooling color + fade-out).
        void Draw(glm::vec3 position, float scale, float lifeFraction, float elapsedTime) const;

    private:
        ExplosionRenderer();
        ~ExplosionRenderer();

        CosmicEngine::Shader *shader_;
        unsigned int vao_;
        unsigned int vbo_;
        int vertexCount_;
    };
}

#endif
