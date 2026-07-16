#ifndef PUZZLERPG_TELEPORTER_OBJECT_HPP
#define PUZZLERPG_TELEPORTER_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/light/light.hpp>

namespace PuzzleRPG
{
    // Teletransporte. Siempre viene emparejado con otro teletransporte (mismo
    // valor de `pairId`). Cuando un jugador entra en su celda, tras un retardo
    // breve el jugador se mueve a la celda del companyero. El re-disparo solo
    // ocurre cuando el jugador sale y vuelve a entrar.
    class TeleporterObject : public CosmicEngine::Object
    {
    public:
        TeleporterObject(int id, int textureId, Cell cell, int pairId);
        ~TeleporterObject() override;

        void draw() const override;

        int  GetObjectId() const { return objectId; }
        int  GetPairId() const { return pairId; }
        int  GetTextureId() const { return textureId; }
        Cell GetCell() const { return cell; }

    private:
        int objectId;
        int textureId;
        Cell cell;
        int pairId;
        SpriteSheetVisual spriteVisual;
        CosmicEngine::Light *light = nullptr;

        void ReleaseLight();
    };
}

#endif
