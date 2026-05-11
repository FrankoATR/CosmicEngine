#ifndef GAMEGRIDCOLLISIONS_HPP
#define GAMEGRIDCOLLISIONS_HPP

#include <glm/glm.hpp>
#include <vector>

namespace WandEngine
{

    enum class CollisionType
    {
        Grid,
        QuadTree
    };
    
    enum class CollisionSide;

    class GameBodyObject;

    struct Cell
    {
        glm::vec2 Position;
        std::vector<GameBodyObject *> objects;
        Cell() : Position(glm::vec2(0.0f)) {}
        Cell(glm::vec2 Position) : Position(Position) {}
    };

    
    class GameGridCollisions
    {
    public:
        glm::vec2 GridPosition;
        glm::vec2 GridSize;
        int Arrows;
        int Columns;
        int CellSize;
        std::vector<std::vector<Cell>> Cells;

        int EntitiesOnGrid;

        GameGridCollisions(glm::vec2 GridPosition, int Arrows, int Columns, int CellSize);
        ~GameGridCollisions();

        glm::vec2 GetPosition();
        void SetPosition(glm::vec2 NewsPosition);
        Cell *GetCell(int Arrow, int Column);
        void DrawCells();
        void AddObject(GameBodyObject *obj);
        void RemoveObject(GameBodyObject *obj);
        void Check_cells_collisions(Cell *cell_1, Cell *cell_2);
        void Find_collision_grid();
        bool RectToRectCollisionBody(GameBodyObject *body1, GameBodyObject *body2);
        CollisionSide GetCollisionSide(GameBodyObject *body1, GameBodyObject *body2);
        
        Cell *GetCellByPositionAndSize(glm::vec2 position, glm::vec2 size);

        void ClearGrid();
    };

}

#endif // GAMEGRIDCOLLISIONS_HPP