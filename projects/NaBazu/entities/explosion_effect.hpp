#ifndef NABAZU_EXPLOSION_EFFECT_HPP
#define NABAZU_EXPLOSION_EFFECT_HPP

#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    // Hand-built scale/fade VFX. CosmicEngine has no particle system, so this fills
    // that gap with a single primitive that grows and fades out over a short lifetime.
    class ExplosionEffect : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        ExplosionEffect(glm::vec3 position, float finalScale);
        ~ExplosionEffect() override;

        void update(float deltaTime) override;
        void draw() const override;

    private:
        float finalScale_;
        CosmicEngine::Timer *lifetimeTimer_;
    };
}

#endif
