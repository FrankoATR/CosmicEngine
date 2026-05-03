/**
 * @file game_grid_collisions.cpp
 * @brief Implements the uniform-grid collision accelerator used by the engine.
 */

#include "game_grid_collisions.hpp"

#include "../managers/resource/resource_manager.hpp"
#include "../models/body/body.hpp"
#include "../models/object/object.hpp"

#include <algorithm>

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    GameGridCollisions::GameGridCollisions(glm::vec2 gridPosition, int arrows, int columns, int cellSize)
        : CollisionArea(gridPosition, glm::vec2(columns * cellSize, arrows * cellSize), CollisionType::Grid),
          GridPosition(gridPosition),
          GridSize(glm::vec2(columns * cellSize, arrows * cellSize)),
          Arrows(arrows),
          Columns(columns),
          CellSize(cellSize),
          EntitiesOnGrid(0)
    {
        Cells.resize(Arrows);
        for (int arrow = 0; arrow < Arrows; ++arrow)
        {
            Cells[arrow].resize(Columns);
            for (int column = 0; column < Columns; ++column)
            {
                Cells[arrow][column] = Cell(glm::vec2(GridPosition.x + column * CellSize, GridPosition.y + arrow * CellSize));
            }
        }
    }

    GameGridCollisions::~GameGridCollisions()
    {
        ClearGrid();
    }

    glm::vec2 GameGridCollisions::GetPosition() const
    {
        return GridPosition;
    }

    void GameGridCollisions::SetPosition(glm::vec2 newPosition)
    {
        GridPosition = newPosition;
        areaPosition = newPosition;

        for (int arrow = 0; arrow < Arrows; ++arrow)
        {
            for (int column = 0; column < Columns; ++column)
            {
                Cells[arrow][column].Position = glm::vec2(GridPosition.x + column * CellSize, GridPosition.y + arrow * CellSize);
            }
        }
    }

    Cell *GameGridCollisions::GetCell(int arrow, int column)
    {
        if (arrow < 0 || arrow >= Arrows || column < 0 || column >= Columns)
        {
            return nullptr;
        }

        return &Cells[arrow][column];
    }

    void GameGridCollisions::DrawCells()
    {
        for (int arrow = 0; arrow <= Arrows; ++arrow)
        {
            ResourceManager::GetInstance().RenderLine(
                glm::vec2(GridPosition.x, GridPosition.y + arrow * CellSize),
                glm::vec2(GridPosition.x + Columns * CellSize, GridPosition.y + arrow * CellSize),
                glm::vec2(0.0f),
                glm::vec2(0.0f),
                glm::vec3(0.0f),
                0.5f,
                2.0f,
                ViewType::Ortho);
        }

        for (int column = 0; column <= Columns; ++column)
        {
            ResourceManager::GetInstance().RenderLine(
                glm::vec2(GridPosition.x + column * CellSize, GridPosition.y),
                glm::vec2(GridPosition.x + column * CellSize, GridPosition.y + Arrows * CellSize),
                glm::vec2(0.0f),
                glm::vec2(0.0f),
                glm::vec3(0.0f),
                0.5f,
                2.0f,
                ViewType::Ortho);
        }
    }

    void GameGridCollisions::DrawDebug()
    {
        DrawCells();
    }

    void GameGridCollisions::AddObject(Body *obj)
    {
        Cell *cell = GetCellByPositionAndSize(obj->GetPosition(), obj->GetSize());
        if (!cell)
        {
            obj->GetParent()->SetInsideGridArea(false);
            return;
        }

        obj->GetParent()->SetInsideGridArea(true);
        cell->objects.push_back(obj);
        ++EntitiesOnGrid;
    }

    void GameGridCollisions::RemoveObject(Body *obj)
    {
        Cell *cell = GetCellByPositionAndSize(obj->GetPosition(), obj->GetSize());
        if (!cell)
        {
            return;
        }

        auto &objects = cell->objects;
        objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
        EntitiesOnGrid = std::max(0, EntitiesOnGrid - 1);
    }

    void GameGridCollisions::CheckCellsCollisions(Cell *cell1, Cell *cell2)
    {
        // Neighbor-cell comparison limits checks to nearby partitions without a full N^2 scan.
        for (Body *body1 : cell1->objects)
        {
            for (Body *body2 : cell2->objects)
            {
                if (body1 == body2 || !body1->GetParent()->IsAlive() || !body2->GetParent()->IsAlive())
                {
                    continue;
                }

                if (BodiesOverlap(body1, body2))
                {
                    body1->OnCollision(body2, GetCollisionSide(body1, body2));
                }
            }
        }
    }

    void GameGridCollisions::FindCollisionGrid()
    {
        for (int arrow = 0; arrow < Arrows; ++arrow)
        {
            for (int column = 0; column < Columns; ++column)
            {
                Cell *currentCell = GetCell(arrow, column);
                for (int deltaColumn = -1; deltaColumn <= 1; ++deltaColumn)
                {
                    for (int deltaArrow = -1; deltaArrow <= 1; ++deltaArrow)
                    {
                        Cell *otherCell = GetCell(arrow + deltaArrow, column + deltaColumn);
                        if (currentCell && otherCell)
                        {
                            CheckCellsCollisions(currentCell, otherCell);
                        }
                    }
                }
            }
        }
    }

    void GameGridCollisions::FindCollisions()
    {
        FindCollisionGrid();
    }

    Cell *GameGridCollisions::GetCellByPositionAndSize(glm::vec2 position, glm::vec2 size)
    {
        int relX = static_cast<int>(position.x - GridPosition.x);
        int relY = static_cast<int>(position.y - GridPosition.y);

        if (relX < 0 || relY < 0 || relX + size.x > GridSize.x || relY + size.y > GridSize.y)
        {
            return nullptr;
        }

        int column = relX / CellSize;
        int arrow = relY / CellSize;
        return GetCell(arrow, column);
    }

    void GameGridCollisions::ClearGrid()
    {
        for (int arrow = 0; arrow < Arrows; ++arrow)
        {
            for (int column = 0; column < Columns; ++column)
            {
                Cells[arrow][column].objects.clear();
            }
        }

        EntitiesOnGrid = 0;
    }

    void GameGridCollisions::Clear()
    {
        ClearGrid();
    }

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    GameGridCollisions::GameGridCollisions(glm::vec3 gridPosition, int rows, int columns, int layers, int cellSize)
        : CollisionArea(gridPosition, glm::vec3(columns * cellSize, rows * cellSize, layers * cellSize), CollisionType::Grid),
          GridPosition(gridPosition),
          GridSize(glm::vec3(columns * cellSize, rows * cellSize, layers * cellSize)),
          Rows(rows),
          Columns(columns),
          Layers(layers),
          CellSize(cellSize),
          EntitiesOnGrid(0)
    {
        Cells.resize(Layers);
        for (int layer = 0; layer < Layers; ++layer)
        {
            Cells[layer].resize(Rows);
            for (int row = 0; row < Rows; ++row)
            {
                Cells[layer][row].resize(Columns);
                for (int column = 0; column < Columns; ++column)
                {
                    Cells[layer][row][column] = Cell(glm::vec3(
                        GridPosition.x + column * CellSize,
                        GridPosition.y + row * CellSize,
                        GridPosition.z + layer * CellSize));
                }
            }
        }
    }

    GameGridCollisions::~GameGridCollisions()
    {
        ClearGrid();
    }

    glm::vec3 GameGridCollisions::GetPosition() const
    {
        return GridPosition;
    }

    void GameGridCollisions::SetPosition(glm::vec3 newPosition)
    {
        GridPosition = newPosition;
        areaPosition = newPosition;

        for (int layer = 0; layer < Layers; ++layer)
        {
            for (int row = 0; row < Rows; ++row)
            {
                for (int column = 0; column < Columns; ++column)
                {
                    Cells[layer][row][column].Position = glm::vec3(
                        GridPosition.x + column * CellSize,
                        GridPosition.y + row * CellSize,
                        GridPosition.z + layer * CellSize);
                }
            }
        }
    }

    Cell *GameGridCollisions::GetCell(int row, int column, int layer)
    {
        if (row < 0 || row >= Rows || column < 0 || column >= Columns || layer < 0 || layer >= Layers)
        {
            return nullptr;
        }

        return &Cells[layer][row][column];
    }

    void GameGridCollisions::DrawCells()
    {
        for (int layer = 0; layer < Layers; ++layer)
        {
            for (int row = 0; row < Rows; ++row)
            {
                for (int column = 0; column < Columns; ++column)
                {
                    const glm::vec3 cellCenter = Cells[layer][row][column].Position + glm::vec3(CellSize * 0.5f);
                    ResourceManager::GetInstance().RenderParallelepipedLines(
                        cellCenter,
                        glm::vec3(CellSize),
                        glm::vec3(0.0f),
                        glm::vec3(0.0f),
                        glm::vec3(0.0f, 0.35f, 0.0f),
                        0.35f,
                        1.0f,
                        false,
                        ViewType::Projection);
                }
            }
        }
    }

    void GameGridCollisions::DrawDebug()
    {
        DrawCells();
    }

    void GameGridCollisions::AddObject(Body *obj)
    {
        Cell *cell = GetCellByPositionAndSize(obj->GetPosition(), obj->GetSize());
        if (!cell)
        {
            obj->GetParent()->SetInsideGridArea(false);
            return;
        }

        obj->GetParent()->SetInsideGridArea(true);
        cell->objects.push_back(obj);
        ++EntitiesOnGrid;
    }

    void GameGridCollisions::RemoveObject(Body *obj)
    {
        Cell *cell = GetCellByPositionAndSize(obj->GetPosition(), obj->GetSize());
        if (!cell)
        {
            return;
        }

        auto &objects = cell->objects;
        objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
        EntitiesOnGrid = std::max(0, EntitiesOnGrid - 1);
    }

    void GameGridCollisions::CheckCellsCollisions(Cell *cell1, Cell *cell2)
    {
        for (Body *body1 : cell1->objects)
        {
            for (Body *body2 : cell2->objects)
            {
                if (body1 == body2 || !body1->GetParent()->IsAlive() || !body2->GetParent()->IsAlive())
                {
                    continue;
                }

                if (BodiesOverlap(body1, body2))
                {
                    body1->OnCollision(body2, GetCollisionSide(body1, body2));
                }
            }
        }
    }

    void GameGridCollisions::FindCollisionGrid()
    {
        for (int layer = 0; layer < Layers; ++layer)
        {
            for (int row = 0; row < Rows; ++row)
            {
                for (int column = 0; column < Columns; ++column)
                {
                    Cell *currentCell = GetCell(row, column, layer);
                    for (int deltaLayer = -1; deltaLayer <= 1; ++deltaLayer)
                    {
                        for (int deltaRow = -1; deltaRow <= 1; ++deltaRow)
                        {
                            for (int deltaColumn = -1; deltaColumn <= 1; ++deltaColumn)
                            {
                                Cell *otherCell = GetCell(row + deltaRow, column + deltaColumn, layer + deltaLayer);
                                if (currentCell && otherCell)
                                {
                                    CheckCellsCollisions(currentCell, otherCell);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void GameGridCollisions::FindCollisions()
    {
        FindCollisionGrid();
    }

    Cell *GameGridCollisions::GetCellByPositionAndSize(glm::vec3 position, glm::vec3 size)
    {
        int relX = static_cast<int>(position.x - GridPosition.x);
        int relY = static_cast<int>(position.y - GridPosition.y);
        int relZ = static_cast<int>(position.z - GridPosition.z);

        if (relX < 0 || relY < 0 || relZ < 0 ||
            relX + size.x > GridSize.x || relY + size.y > GridSize.y || relZ + size.z > GridSize.z)
        {
            return nullptr;
        }

        int column = relX / CellSize;
        int row = relY / CellSize;
        int layer = relZ / CellSize;
        return GetCell(row, column, layer);
    }

    void GameGridCollisions::ClearGrid()
    {
        for (int layer = 0; layer < Layers; ++layer)
        {
            for (int row = 0; row < Rows; ++row)
            {
                for (int column = 0; column < Columns; ++column)
                {
                    Cells[layer][row][column].objects.clear();
                }
            }
        }

        EntitiesOnGrid = 0;
    }

    void GameGridCollisions::Clear()
    {
        ClearGrid();
    }

#else
    #error "[GameGridCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}
