#ifndef NABAZU_CHASE_CAMERA_HPP
#define NABAZU_CHASE_CAMERA_HPP

#include "rail_path.hpp"

#include <glm/glm.hpp>

namespace NaBazu
{
    class Ship;

    // Dedicated chase-cam, NOT CosmicEngine::Classic3DCameraController (that one is a
    // free-fly WASD+mouselook controller and does not fit a rail shooter). Follows the
    // rail's own stabilized frame rather than the ship's banked orientation so the
    // camera does not roll along with the ship.
    class ChaseCamera
    {
    public:
        void Update(const Ship &ship, const RailPath &rail, float deltaTime);
        // Drops the smoothing history so the next Update() snaps straight to the ideal
        // pose. Without this, returning from the free-orbit camera makes the view swoop
        // across the map from wherever the smoothing was left.
        void Reset();

    private:
        glm::vec3 smoothedPosition_ = glm::vec3(0.0f);
        glm::vec3 smoothedLookTarget_ = glm::vec3(0.0f);
        bool initialized_ = false;
    };
}

#endif
