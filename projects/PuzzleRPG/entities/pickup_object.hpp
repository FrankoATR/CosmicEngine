#ifndef PUZZLERPG_PICKUP_OBJECT_HPP
#define PUZZLERPG_PICKUP_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

#include <string>

namespace PuzzleRPG
{
    // Objeto recogible. Reside en una celda del mundo hasta que un jugador lo
    // toma. Cuando esta en un inventario su Visible se pone en false y deja de
    // ocupar la celda.
    class PickupObject : public CosmicEngine::Object
    {
    public:
        PickupObject(int id, int textureId, Cell cell, std::string dialog = "");

        void update(float deltaTime) override;
        void draw() const override;

        bool IsOnAltar() const { return onAltar; }
        void SetOnAltar(bool value);

        int  GetObjectId() const { return objectId; }
        int  GetTextureId() const { return textureId; }
        Cell GetCell() const { return cell; }
        void SetCell(Cell newCell);

        bool IsCarried() const { return carried; }
        void SetCarried(bool value);

        const std::string &GetDialog() const { return dialog; }

        // Selecciona una sub-celda concreta de la hoja indicada. sheet == 0
        // restaura la animacion/clip por defecto.
        void SetSpriteVariant(int sheet, int row, int col);
        int GetSpriteSheet() const { return spriteSheet; }
        int GetSpriteRow() const { return spriteRow; }
        int GetSpriteCol() const { return spriteCol; }

    private:
        int objectId;
        int textureId;
        Cell cell;
        bool carried;
        std::string dialog;
        SpriteSheetVisual spriteVisual;
        int spriteSheet = 0;
        int spriteRow   = 0;
        int spriteCol   = 0;
        bool onAltar = false;
        float levitateTime = 0.0f;
    };
}

#endif
