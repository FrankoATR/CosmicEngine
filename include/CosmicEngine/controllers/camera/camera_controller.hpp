#ifndef COSMIC_CAMERA_CONTROLLER_HPP
#define COSMIC_CAMERA_CONTROLLER_HPP

namespace CosmicEngine
{
    class ICameraController
    {
    public:
        virtual ~ICameraController() = default;

        virtual void Update(double deltaTime) = 0;
        virtual void OnMouseMove(double xpos, double ypos)
        {
            (void)xpos;
            (void)ypos;
        }

        virtual void OnMouseScroll(double xoffset, double yoffset)
        {
            (void)xoffset;
            (void)yoffset;
        }
    };
}

#endif