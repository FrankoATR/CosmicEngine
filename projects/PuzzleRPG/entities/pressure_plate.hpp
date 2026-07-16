#ifndef PUZZLERPG_PRESSURE_PLATE_HPP
#define PUZZLERPG_PRESSURE_PLATE_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/light/light.hpp>

#include <vector>

namespace PuzzleRPG
{
    // Placa de presion: se activa cuando hay un jugador o un objeto empujable
    // sobre su celda. El modo `activator` define que tipo de entidad la activa.
    class PressurePlate : public CosmicEngine::Object
    {
    public:
        PressurePlate(int id, int textureId, Cell cell, int activator, std::vector<int> targets,
                      int requiredItemTextureId = 0);
        ~PressurePlate() override;

        void draw() const override;

        int  GetObjectId() const { return objectId; }
        Cell GetCell() const { return cell; }
        int  GetActivator() const { return activator; }
        int  GetRequiredItemTextureId() const { return requiredItemTextureId; }
        const std::vector<int> &GetTargets() const { return targets; }

        int GetTextureId() const { return textureId; }

        bool IsActive() const { return active; }
        void SetActive(bool value);

    private:
        int objectId;
        int textureId;
        Cell cell;
        int  activator;
        bool active;
        int  requiredItemTextureId;
        std::vector<int> targets;
        SpriteSheetVisual spriteVisual;
        CosmicEngine::Light *light = nullptr;

        void ReleaseLight();
    };
}

#endif
