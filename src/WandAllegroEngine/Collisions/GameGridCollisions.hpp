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
        Vec2 Position;
        std::vector<GameBodyObject *> objects;
        Cell() : Position(Vec2(0, 0)) {}
        Cell(Vec2 Position) : Position(Position) {}
    };

    
    class GameGridCollisions
    {
    public:
        Vec2 GridPosition;
        int Arrows;
        int Columns;
        int CellSize;
        std::vector<std::vector<Cell>> Cells;

        int EntitiesOnGrid;

        GameGridCollisions(Vec2 GridPosition, int Arrows, int Columns, int CellSize);
        ~GameGridCollisions();

        Vec2 GetPosition();
        void SetPosition(Vec2 NewsPosition);
        Cell *GetCell(int Arrow, int Column);
        void DrawCells();
        void AddObject(GameBodyObject *obj);
        void RemoveObject(GameBodyObject *obj);
        void Check_cells_collisions(Cell *cell_1, Cell *cell_2);
        void Find_collision_grid();
        bool RectToRectCollisionBody(GameBodyObject *body1, GameBodyObject *body2);
        Cell *GetCellByPosition(Vec2 position);

        void ClearGrid();
    };

}

#endif // GAMEGRIDCOLLISIONS_HPP