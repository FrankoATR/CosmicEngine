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
        WAND_VEC2 Position;
        WAND_SIZE Size;

        CameraManager();
        ~CameraManager() = default;
        CameraManager(const CameraManager &) = delete;
        CameraManager &operator=(const CameraManager &) = delete;

        WAND_VEC2 GetGridPosition() const;
        void SetGridPosition(WAND_VEC2 NewPosition);

    public:
        static CameraManager &GetInstance()
        {
            static CameraManager instance;
            return instance;
        }

        WAND_VEC2 GetPosition() const;
        WAND_SIZE GetSize() const;
        void Draw();
        void FocusObject(GameObject *Obj);
        void FocusPosition(WAND_VEC2 NewPosition);
        WAND_VEC2 GetFocusPosition() const;

        void Reset();

        bool IsObjectInsideCameraArea(GameObject *Obj) const;
    };

}

#endif // CAMERAMANAGER_HPP
