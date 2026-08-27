#ifndef NABAZU_MATH_UTILS_HPP
#define NABAZU_MATH_UTILS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NaBazu
{
    // Rotates a local-space offset by the given Euler rotation (degrees), replicating
    // the exact Rx*Ry*Rz composition order ResourceManager uses when rendering 3D
    // primitives/models. Needed because CosmicEngine::Body offsets are never rotated
    // by their parent's orientation, so nose/hardpoint spawn points must be rotated
    // manually to track the ship's visual orientation through banking.
    inline glm::vec3 RotateLocalOffset(glm::vec3 localOffset, glm::vec3 rotationDeg)
    {
        glm::mat4 m(1.0f);
        m = glm::rotate(m, glm::radians(rotationDeg.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::rotate(m, glm::radians(rotationDeg.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::rotate(m, glm::radians(rotationDeg.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return glm::vec3(m * glm::vec4(localOffset, 0.0f));
    }
}

#endif
