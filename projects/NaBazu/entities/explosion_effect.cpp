#include "explosion_effect.hpp"

#include "../systems/glow_mesh_renderer.hpp"

#include "../systems/test_mode.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include TIMERMANAGER_HEADER

namespace NaBazu
{
    const std::string &ExplosionEffect::StaticClassName()
    {
        static const std::string className = "ExplosionEffect";
        return className;
    }

    ExplosionEffect::ExplosionEffect(glm::vec3 position, float finalScale)
        : Object(StaticClassName(), position, glm::vec3(0.2f)),
          finalScale_(finalScale),
          lifetimeTimer_(nullptr)
    {
        SetColor(glm::vec3(1.0f, 0.55f, 0.15f));
        lifetimeTimer_ = new CosmicEngine::Timer(0.35, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(lifetimeTimer_);
    }

    ExplosionEffect::~ExplosionEffect()
    {
        if (lifetimeTimer_)
        {
            lifetimeTimer_->Destroy();
            lifetimeTimer_ = nullptr;
        }
    }

    void ExplosionEffect::update(float deltaTime)
    {
        // TEMPORARY test mode: hold position (and skip lifetime expiry) while the
        // world is frozen for collision inspection.
        if (IsWorldFrozen())
        {
            return;
        }

        (void)deltaTime;

        if (!lifetimeTimer_)
        {
            Destroy();
            return;
        }

        const float t = static_cast<float>(lifetimeTimer_->GetElapsedTime() / lifetimeTimer_->GetTargetTime());
        SetSize(glm::vec3(glm::mix(0.2f, finalScale_, glm::clamp(t, 0.0f, 1.0f))));

        if (lifetimeTimer_->IsFinished())
        {
            Destroy();
        }
    }

    void ExplosionEffect::draw() const
    {
        if (!lifetimeTimer_)
        {
            return;
        }

        const float lifeFraction = glm::clamp(
            static_cast<float>(lifetimeTimer_->GetElapsedTime() / lifetimeTimer_->GetTargetTime()), 0.0f, 1.0f);
        const float elapsedTime = static_cast<float>(lifetimeTimer_->GetElapsedTime());

        // Turbulent-noise fireball shader instead of a plain fading textured cube --
        // GetSize() is already animated (grows 0.2 -> finalScale_) by update() above.
        ExplosionRenderer::GetInstance().Draw(GetPosition(), GetSize().x, lifeFraction, elapsedTime);
    }
}
