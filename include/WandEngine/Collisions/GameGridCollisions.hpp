#ifndef GAMEGRIDCOLLISIONS_HPP
#define GAMEGRIDCOLLISIONS_HPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include "../Interfaces/Definitions.hpp"

namespace WandEngine
{

    class GameBodyObject;

    struct Cell
    {
        WAND_VEC2 Position;
        std::vector<GameBodyObject *> objects;
        Cell() : Position(WAND_VEC2(0, 0)) {}
        Cell(WAND_VEC2 Position) : Position(Position) {}
    };

    
    class GameGridCollisions
    {
    public:
        WAND_VEC2 GridPosition;
        WAND_SIZE GridSize;
        int Arrows;
        int Columns;
        int CellSize;
        std::vector<std::vector<Cell>> Cells;

        int EntitiesOnGrid;

        GameGridCollisions(WAND_VEC2 GridPosition, int Arrows, int Columns, int CellSize);
        ~GameGridCollisions();

        WAND_VEC2 GetPosition();
        void SetPosition(WAND_VEC2 NewsPosition);
        Cell *GetCell(int Arrow, int Column);
        void DrawCells();
        void AddObject(GameBodyObject *obj);
        void RemoveObject(GameBodyObject *obj);
        void Check_cells_collisions(Cell *cell_1, Cell *cell_2);
        void Find_collision_grid();
        bool RectToRectCollisionBody(GameBodyObject *body1, GameBodyObject *body2);
        CollisionSide GetCollisionSide(GameBodyObject *body1, GameBodyObject *body2);
        
        Cell *GetCellByPositionAndSize(WAND_VEC2 position, WAND_VEC2 size);

        void ClearGrid();
    };

}

#endif // GAMEGRIDCOLLISIONS_HPP