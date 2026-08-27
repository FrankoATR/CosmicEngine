#ifndef NABAZU_TEST_MODE_HPP
#define NABAZU_TEST_MODE_HPP

#include <glm/glm.hpp>

namespace NaBazu
{
    class Ship;

    // ==========================================================================
    // TEMPORARY collision-inspection mode, toggled at runtime with F3. The game boots
    // in normal play; F3 swaps to inspection and back.
    //
    //   F3 ......... toggle inspection mode on/off
    //   Mouse drag . orbit the camera freely around the ship
    //   Mouse wheel  zoom the orbit in/out
    //   J / K ...... step forward / backward along the rail by hand
    //   O .......... freeze every entity except the player
    // ==========================================================================
    bool IsTestModeEnabled();
    void SetTestModeEnabled(bool enabled);

    // Queried by every entity's update() so frozen objects hold position. Also zeroes
    // their velocity while frozen, because ObjectManager integrates velocity itself
    // (outside the entity's own update), so skipping entity logic alone is not enough.
    bool IsWorldFrozen();
    void ToggleWorldFrozen();

    // Free-orbit debug camera: mouse-driven yaw/pitch/zoom around the player.
    class OrbitCamera
    {
    public:
        // Hooks GameManager's mouse callbacks. Safe to call once per scene.
        void Attach();
        // MUST be called before this object dies: the callbacks registered in Attach()
        // capture `this`, and GameManager keeps calling them after the owning scene is
        // destroyed (e.g. on game over), which would dereference freed memory.
        void Detach();
        void Update(const Ship &ship);

    private:
        float yawDeg_ = 0.0f;
        float pitchDeg_ = -18.0f;
        float distance_ = 70.0f;
        bool attached_ = false;
    };
}

#endif
