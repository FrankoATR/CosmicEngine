#ifndef COSMIC_GAMEGRIDCOLLISIONS_HPP
#define COSMIC_GAMEGRIDCOLLISIONS_HPP

/**
 * @file game_grid_collisions.hpp
 * @brief Declares the uniform-grid collision area used by the engine.
 */

#include "collision_area.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
{
    class Body;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    /** @brief Stores bodies assigned to a 2D grid cell. */
    struct Cell
    {
        glm::vec2 Position;
        std::vector<Body *> objects;

        Cell() : Position(glm::vec2(0.0f)) {}
        explicit Cell(glm::vec2 position) : Position(position) {}
    };

    /**
     * @brief Uniform-grid collision accelerator for 2D runtime mode.
     *
     * Partitions space into equal-sized cells.  Each frame, every Body is inserted
     * into the cell(s) it overlaps, then neighbor-cell pairs are tested for actual
     * overlap and collision callbacks are fired.  Created automatically by
     * BodyManager::CreateCollisionArea(CollisionType::Grid, ...).
     *
     * @par Example (automatic creation via BodyManager)
     * @code
     * BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::Grid,
     *     glm::vec2(-500, -300), glm::vec2(1000, 600), 80, 6, 4);
     * @endcode
     */
    class GameGridCollisions : public CollisionArea
    {
    public:
        glm::vec2 GridPosition;   ///< World-space origin of the grid (top-left corner).
        glm::vec2 GridSize;       ///< Total grid extent in world units (Columns*CellSize, Arrows*CellSize).
        int Arrows;               ///< Number of cell rows along the Y axis.
        int Columns;              ///< Number of cell columns along the X axis.
        int CellSize;             ///< Size of every square cell in world units.
        std::vector<std::vector<Cell>> Cells; ///< Row-major matrix of cells holding inserted bodies.
        int EntitiesOnGrid;       ///< Number of bodies currently present in the grid.

        /**
         * @brief Builds a 2D uniform grid centered at @p gridPosition.
         * @param gridPosition World-space origin used as the grid top-left corner.
         * @param arrows Number of cell rows.
         * @param columns Number of cell columns.
         * @param cellSize Edge length of every cell, in world units.
         */
        GameGridCollisions(glm::vec2 gridPosition, int arrows, int columns, int cellSize);
        /** @brief Releases internal cell storage and detaches every tracked body. */
        ~GameGridCollisions();

        /** @brief Returns the world-space origin of the grid. */
        glm::vec2 GetPosition() const override;
        /** @brief Repositions the grid origin and rebuilds spatial assignments. */
        void SetPosition(glm::vec2 newPosition) override;
        /**
         * @brief Returns a pointer to the cell at row @p arrow and column @p column.
         * @return Pointer to the cell, or nullptr when the indices are out of range.
         */
        Cell *GetCell(int arrow, int column);
        /** @brief Draws the cell grid wireframe through PrimitiveManager (debug helper). */
        void DrawCells();
        /** @brief Renders both the grid and the bounding box of every tracked body. */
        void DrawDebug() override;
        /** @brief Inserts a body into every cell its AABB overlaps. */
        void AddObject(Body *obj) override;
        /** @brief Removes a body from every cell it currently occupies. */
        void RemoveObject(Body *obj) override;
        /** @brief Performs pairwise collision tests between bodies of two cells. */
        void CheckCellsCollisions(Cell *cell1, Cell *cell2);
        /** @brief Iterates every cell pair and dispatches @ref CheckCellsCollisions. */
        void FindCollisionGrid();
        /** @brief Runs a full collision pass over the grid during BodyManager::update(). */
        void FindCollisions() override;

        /**
         * @brief Returns the cell that contains the AABB defined by @p position and @p size.
         * @return Pointer to the cell, or nullptr when the AABB lies outside the grid.
         */
        Cell *GetCellByPositionAndSize(glm::vec2 position, glm::vec2 size);

        /** @brief Empties every cell without releasing the underlying storage. */
        void ClearGrid();
        /** @brief Clears the grid and detaches every tracked body. */
        void Clear() override;
    };

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    /** @brief Stores bodies assigned to a 3D grid cell. */
    struct Cell
    {
        glm::vec3 Position;
        std::vector<Body *> objects;

        Cell() : Position(glm::vec3(0.0f)) {}
        explicit Cell(glm::vec3 position) : Position(position) {}
    };

    /**
     * @brief Uniform-grid collision accelerator for 3D runtime mode.
     *
     * Same principle as the 2D grid but partitions a 3D volume into rows,
     * columns, and layers.  Created automatically by BodyManager.
     *
     * @par Example (automatic creation via BodyManager)
     * @code
     * BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::Grid,
     *     glm::vec3(-50, 0, -50), glm::vec3(100, 20, 100), 10, 5, 4);
     * @endcode
     */
    class GameGridCollisions : public CollisionArea
    {
    public:
        glm::vec3 GridPosition;   ///< World-space origin of the grid volume.
        glm::vec3 GridSize;       ///< Total grid extent in world units (Columns, Layers, Rows * CellSize).
        int Rows;                 ///< Number of cell rows along the Z axis.
        int Columns;              ///< Number of cell columns along the X axis.
        int Layers;               ///< Number of cell layers along the Y axis.
        int CellSize;             ///< Edge length of every cube cell, in world units.
        std::vector<std::vector<std::vector<Cell>>> Cells; ///< 3D matrix of cells indexed [row][column][layer].
        int EntitiesOnGrid;       ///< Number of bodies currently present in the grid.

        /**
         * @brief Builds a 3D uniform grid centered at @p gridPosition.
         * @param gridPosition World-space origin of the grid volume.
         * @param rows Number of rows along Z.
         * @param columns Number of columns along X.
         * @param layers Number of layers along Y.
         * @param cellSize Edge length of every cube cell.
         */
        GameGridCollisions(glm::vec3 gridPosition, int rows, int columns, int layers, int cellSize);
        /** @brief Releases internal cell storage and detaches every tracked body. */
        ~GameGridCollisions();

        /** @brief Returns the world-space origin of the grid. */
        glm::vec3 GetPosition() const override;
        /** @brief Repositions the grid origin and rebuilds spatial assignments. */
        void SetPosition(glm::vec3 newPosition) override;
        /**
         * @brief Returns the cell located at the supplied row/column/layer indices.
         * @return Pointer to the cell, or nullptr when the indices are out of range.
         */
        Cell *GetCell(int row, int column, int layer);
        /** @brief Draws the grid wireframe through PrimitiveManager (debug helper). */
        void DrawCells();
        /** @brief Renders the grid and the bounding volume of every tracked body. */
        void DrawDebug() override;
        /** @brief Inserts a body into every cell its 3D AABB overlaps. */
        void AddObject(Body *obj) override;
        /** @brief Removes a body from every cell it currently occupies. */
        void RemoveObject(Body *obj) override;
        /** @brief Performs pairwise collision tests between bodies of two cells. */
        void CheckCellsCollisions(Cell *cell1, Cell *cell2);
        /** @brief Iterates every cell pair and dispatches @ref CheckCellsCollisions. */
        void FindCollisionGrid();
        /** @brief Runs a full collision pass over the grid during BodyManager::update(). */
        void FindCollisions() override;

        /**
         * @brief Returns the cell containing the AABB defined by @p position and @p size.
         * @return Pointer to the cell, or nullptr when the AABB lies outside the grid.
         */
        Cell *GetCellByPositionAndSize(glm::vec3 position, glm::vec3 size);

        /** @brief Empties every cell without releasing the underlying storage. */
        void ClearGrid();
        /** @brief Clears the grid and detaches every tracked body. */
        void Clear() override;
    };

#else
    #error "[GameGridCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif
}

#endif // COSMIC_GAMEGRIDCOLLISIONS_HPP
