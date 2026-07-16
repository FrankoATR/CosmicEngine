#ifndef PUZZLERPG_WALL_BLOCK_OBJECT_HPP
#define PUZZLERPG_WALL_BLOCK_OBJECT_HPP

#include "../systems/grid.hpp"

#include <CosmicEngine/models/object/object.hpp>

namespace PuzzleRPG
{
    // Muro multi-celda (2x2 o 3x3) que se dibuja como un solo sprite
    // procedente de la hoja walls-2x2.png / walls-3x3.png. Bloquea TODAS las
    // celdas que cubre. Posicion (cell) corresponde a la esquina superior
    // izquierda.
    class WallBlockObject : public CosmicEngine::Object
    {
    public:
        WallBlockObject(int id, int sheet, int row, int col, Cell topLeft, int span);

        void draw() const override;

        int  GetObjectId() const { return objectId; }
        Cell GetCell() const     { return cell; }
        int  GetSpan() const     { return span; }
        int  GetSheet() const    { return sheet; }
        int  GetRow() const      { return row; }
        int  GetCol() const      { return col; }

        // True si la celda dada queda dentro del rango cubierto por el muro.
        bool CoversCell(Cell c) const
        {
            return c.x >= cell.x && c.x < cell.x + span
                && c.y >= cell.y && c.y < cell.y + span;
        }

    private:
        int  objectId;
        int  sheet;
        int  row;
        int  col;
        Cell cell;
        int  span;
    };
}

#endif
