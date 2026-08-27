#include "missile.hpp"

#include "enemy_grunt.hpp"
#include "explosion_effect.hpp"
#include "../systems/glow_mesh_renderer.hpp"
#include "../utilities/nabazu_asset_defines.hpp"

#include "../systems/test_mode.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include BODYMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include TIMERMANAGER_HEADER

namespace NaBazu
{
    const std::string &Missile::StaticClassName()
    {
        static const std::string className = "Missile";
        return className;
    }

    Missile::Missile(glm::vec3 spawnPosition, glm::vec3 direction, float speed, float splashRadius, int splashDamage)
        : Object(StaticClassName(), spawnPosition, glm::vec3(0.6f)),
          splashRadius_(splashRadius),
          splashDamage_(splashDamage),
          bodyId_(-1),
          exploded_(false),
          lifetimeTimer_(nullptr)
    {
        SetColor(glm::vec3(1.0f, 0.5f, 0.1f));

        const glm::vec3 travelDirection = (glm::length(direction) > 1e-5f) ? glm::normalize(direction) : glm::vec3(0.0f, 0.0f, -1.0f);
        SetVelocity(travelDirection * speed);

        lifetimeTimer_ = new CosmicEngine::Timer(1.5, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(lifetimeTimer_);

        CreateBody();
    }

    Missile::~Missile()
    {
        if (bodyId_ >= 0)
        {
            BOD_MN.Remove(bodyId_);
            bodyId_ = -1;
        }

        if (lifetimeTimer_)
        {
            lifetimeTimer_->Destroy();
            lifetimeTimer_ = nullptr;
        }
    }

    void Missile::CreateBody()
    {
        // See Ship::CreateBody: rendering treats GetPosition() as the box center,
        // collision treats a Body's position as its min corner -- offset by -size/2
        // so the two agree.
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this,
            -GetSize() * 0.5f,
            GetSize(),
            CALLBACK_COLLISION_EVENT(OnBodyCollision));

        BOD_MN.Add(body);
        bodyId_ = body->GetID();
    }

    void Missile::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
    {
        (void)side;

        if (!other || other->GetClassName() != EnemyGrunt::StaticClassName())
        {
            return;
        }

        Explode();
    }

    void Missile::Explode()
    {
        if (exploded_)
        {
            return;
        }
        exploded_ = true;

        const glm::vec3 impact = GetPosition();

        for (auto *object : OBJ_MN.FindByClassName(EnemyGrunt::StaticClassName()))
        {
            if (!object)
            {
                continue;
            }

            if (glm::distance(object->GetPosition(), impact) <= splashRadius_)
            {
                static_cast<EnemyGrunt *>(object)->TakeDamage(splashDamage_);
            }
        }

        // ExplosionEffect's finalScale becomes its rendered sphere's diameter (a unit
        // sphere mesh has radius 0.5, so scale*0.5 = radius) -- tie it to splashRadius_
        // so the visible fireball actually matches the real kill radius, instead of a
        // fixed size that looked wrong once splashRadius_ changed independently.
        OBJ_MN.Add(new ExplosionEffect(impact, splashRadius_ * 2.0f));
        AUD_MN.PlayAt(NABAZU_SFX_BOMB_DESTROY_KEY, impact, CosmicEngine::SoundSpace::World3D, 0.9f, false, true);

        Destroy();
    }

    void Missile::update(float deltaTime)
    {
        // TEMPORARY test mode: hold position (and skip lifetime expiry) while the
        // world is frozen for collision inspection.
        if (IsWorldFrozen())
        {
            return;
        }

        (void)deltaTime;

        if (exploded_)
        {
            return;
        }

        if (lifetimeTimer_ && lifetimeTimer_->IsFinished())
        {
            Explode();
        }
    }

    void Missile::draw() const
    {
        // Bright glowing sphere (fresnel rim + pulse) instead of a plain textured
        // cube -- a sphere needs no travel-direction orientation, it looks identical
        // from every angle.
        const float elapsedTime = lifetimeTimer_ ? static_cast<float>(lifetimeTimer_->GetElapsedTime()) : 0.0f;
        OrbGlowRenderer::GetInstance().Draw(GetPosition(), GetSize() * 1.4f, GetColor(), elapsedTime);
    }
}
