#pragma once
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/body/body.hpp>

#include "../utilities/bench_state.hpp"

namespace Bench
{
    // A moving box that bounces inside the benchmark area. Optionally carries a
    // collision Body and/or draws a sprite (2D only).
    class BenchObject : public CosmicEngine::Object
    {
    public:
        BenchObject(Vec position, Vec size, Vec velocity, bool withBody, bool withSprite);
        void init() override;
        void update(float deltaTime) override;
        void draw() const override;
        long long OwnCallbacks() const { return ownCallbacks; }

    private:
        void OnCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);

        Vec velocity;
        long long ownCallbacks = 0;
        bool withBody;
        bool withSprite;
    };
}
