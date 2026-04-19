#ifndef COSMIC_CAMERA_CONTROLLER_HPP
#define COSMIC_CAMERA_CONTROLLER_HPP

/**
 * @file camera_controller.hpp
 * @brief Declares the base interface implemented by camera controllers.
 */

namespace CosmicEngine
{
    /**
     * @brief Interface implemented by runtime camera controller strategies.
     *
     * Derive from ICameraController to implement custom camera behaviors.
     * The engine provides Classic2DCameraController and Classic3DCameraController
     * as built-in implementations.
     *
     * @par Example — using a camera controller
     * @code
     * std::unique_ptr<CosmicEngine::ICameraController> camCtrl;
     * camCtrl = std::make_unique<CosmicEngine::Classic2DCameraController>();
     *
     * // Call each frame:
     * camCtrl->Update(deltaTime);
     * @endcode
     */
    class ICameraController
    {
    public:
        virtual ~ICameraController() = default;

        /** @brief Updates camera control logic. */
        virtual void Update(double deltaTime) = 0;
        /** @brief Handles mouse movement input. */
        virtual void OnMouseMove(double xpos, double ypos)
        {
            (void)xpos;
            (void)ypos;
        }

        /** @brief Handles mouse scroll input. */
        virtual void OnMouseScroll(double xoffset, double yoffset)
        {
            (void)xoffset;
            (void)yoffset;
        }
    };
}

#endif