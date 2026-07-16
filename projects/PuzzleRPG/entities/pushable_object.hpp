#ifndef PUZZLERPG_PUSHABLE_OBJECT_HPP
#define PUZZLERPG_PUSHABLE_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    // Caja empujable. Bloquea la celda en la que esta. Cuando un jugador trata
    // de moverse hacia ella, si la celda detras esta libre, se empuja una casilla.
    class PushableObject : public CosmicEngine::Object
    {
    public:
        PushableObject(int id, int textureId, Cell cell);

        void update(float deltaTime) override;
        void draw() const override;

        int  GetObjectId() const { return objectId; }
        int  GetTextureId() const { return textureId; }
        Cell GetCell() const { return cell; }
        void SetCell(Cell newCell);
        void BeginMoveTo(Cell newCell, float durationSeconds = 0.18f);

        // Selecciona una sub-celda concreta de la hoja indicada. sheet == 0
        // restaura la animacion/clip por defecto.
        void SetSpriteVariant(int sheet, int row, int col);

    private:
        int objectId;
        int textureId;
        Cell cell;
        Cell tweenFromCell;
        Cell tweenToCell;
        bool tweenActive = false;
        float tweenProgress = 0.0f;
        float tweenDuration = 0.18f;
        SpriteSheetVisual spriteVisual;
        int spriteSheet = 0;
        int spriteRow   = 0;
        int spriteCol   = 0;
    };
}

#endif
