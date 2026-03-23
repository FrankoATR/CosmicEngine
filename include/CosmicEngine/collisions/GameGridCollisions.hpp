#ifndef COSMIC_GAMEGRIDCOLLISIONS_HPP
#define COSMIC_GAMEGRIDCOLLISIONS_HPP

#include "CollisionArea.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
{
    class Body;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    struct Cell
    {
        glm::vec2 Position;
        std::vector<Body *> objects;

        Cell() : Position(glm::vec2(0.0f)) {}
        explicit Cell(glm::vec2 position) : Position(position) {}
    };

    class GameGridCollisions : public CollisionArea
    {
    public:
        glm::vec2 GridPosition;
        glm::vec2 GridSize;
        int Arrows;
        int Columns;
        int CellSize;
        std::vector<std::vector<Cell>> Cells;
        int EntitiesOnGrid;

        GameGridCollisions(glm::vec2 gridPosition, int arrows, int columns, int cellSize);
        ~GameGridCollisions();

        glm::vec2 GetPosition() const override;
        void SetPosition(glm::vec2 newPosition) override;
        Cell *GetCell(int arrow, int column);
        void DrawCells();
        void DrawDebug() override;
        void AddObject(Body *obj) override;
        void RemoveObject(Body *obj) override;
        void CheckCellsCollisions(Cell *cell1, Cell *cell2);
        void FindCollisionGrid();
        void FindCollisions() override;

        Cell *GetCellByPositionAndSize(glm::vec2 position, glm::vec2 size);

        void ClearGrid();
        void Clear() override;
    };

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    struct Cell
    {
        glm::vec3 Position;
        std::vector<Body *> objects;

        Cell() : Position(glm::vec3(0.0f)) {}
        explicit Cell(glm::vec3 position) : Position(position) {}
    };

    class GameGridCollisions : public CollisionArea
    {
    public:
        glm::vec3 GridPosition;
        glm::vec3 GridSize;
        int Rows;
        int Columns;
        int Layers;
        int CellSize;
        std::vector<std::vector<std::vector<Cell>>> Cells;
        int EntitiesOnGrid;

        GameGridCollisions(glm::vec3 gridPosition, int rows, int columns, int layers, int cellSize);
        ~GameGridCollisions();

        glm::vec3 GetPosition() const override;
        void SetPosition(glm::vec3 newPosition) override;
        Cell *GetCell(int row, int column, int layer);
        void DrawCells();
        void DrawDebug() override;
        void AddObject(Body *obj) override;
        void RemoveObject(Body *obj) override;
        void CheckCellsCollisions(Cell *cell1, Cell *cell2);
        void FindCollisionGrid();
        void FindCollisions() override;

        Cell *GetCellByPositionAndSize(glm::vec3 position, glm::vec3 size);

        void ClearGrid();
        void Clear() override;
    };

#else
    #error "[GameGridCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}

#endif // COSMIC_GAMEGRIDCOLLISIONS_HPP