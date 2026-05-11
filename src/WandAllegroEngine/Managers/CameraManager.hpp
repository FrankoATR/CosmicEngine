#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include <allegro5/allegro5.h>
#include "../Interfaces/Definitions.hpp"


namespace WandEngine
{

    class GameObject;

    class CameraManager
    {
    private:
        ALLEGRO_TRANSFORM camera;
        void Transform();
        Vec2 Position;
        Size Size;

        CameraManager();
        ~CameraManager() = default;
        CameraManager(const CameraManager &) = delete;
        CameraManager &operator=(const CameraManager &) = delete;

        Vec2 GetGridPosition();
        void SetGridPosition(Vec2 NewPosition);

    public:
        static CameraManager &GetInstance()
        {
            static CameraManager instance;
            return instance;
        }

        void Draw();
        void FocusObject(GameObject *Obj);
        void FocusPosition(Vec2 NewPosition);
        void Reset();
    };

}

#endif // CAMERAMANAGER_HPP
