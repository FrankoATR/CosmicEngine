#ifndef PUZZLERPG_BUTTON_OBJECT_HPP
#define PUZZLERPG_BUTTON_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

#include <vector>

namespace PuzzleRPG
{
    // Boton interactivo. Solo se activa cuando el jugador esta en la casilla
    // de enfrente mirando hacia el boton y decide interactuar. Su efecto en
    // las puertas es persistente (toggle) hasta que se vuelva a presionar.
    class ButtonObject : public CosmicEngine::Object
    {
    public:
        ButtonObject(int id, int textureId, Cell cell, int requiredFacing, std::vector<int> targets);

        void draw() const override;

        int  GetObjectId() const { return objectId; }
        Cell GetCell() const { return cell; }
        int  GetRequiredFacing() const { return requiredFacing; }
        const std::vector<int> &GetTargets() const { return targets; }

        bool IsActive() const { return active; }
        void Toggle();
        void SetActive(bool value);

    private:
        int objectId;
        int textureId;
        Cell cell;
        int  requiredFacing;
        bool active;
        std::vector<int> targets;
        SpriteSheetVisual spriteVisual;
    };
}

#endif
