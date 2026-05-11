#if defined(GAME_2D_CONFIGURATION)

    #include "GameGridCollisions.hpp"
    #include "../Models/Object/Object.hpp"
    #include "../Models/Body/Body.hpp"
    #include "../Managers/Resource/ResourceManager.hpp"

    #include <iostream>
    #include <algorithm>

    namespace WandEngine
    {

        GameGridCollisions::GameGridCollisions(glm::vec2 GridPosition, int Arrows, int Columns, int CellSize) : GridPosition(GridPosition), Arrows(Arrows), Columns(Columns), CellSize(CellSize), EntitiesOnGrid(0)
        {
            Cells.resize(Arrows);
            for (int i = 0; i < Arrows; i++)
            {
                Cells[i].resize(Columns);
                for (int j = 0; j < Columns; j++)
                {
                    //std::cout << "CREATE INDEX: [" << i << "][" << j << "]" << std::endl;
                    //std::cout << "AT POSITION: (" << GridPosition.x + j * CellSize << ", " << GridPosition.y + i * CellSize << ")" << std::endl;
                    Cells[i][j] = Cell(glm::vec2(GridPosition.x + j * CellSize, GridPosition.y + i * CellSize));
                }
            }
            GridSize = glm::vec2(Columns * CellSize, Arrows * CellSize);
        }

        glm::vec2 GameGridCollisions::GetPosition()
        {
            return this->GridPosition;
        }

        void GameGridCollisions::SetPosition(glm::vec2 NewsPosition)
        {
            this->GridPosition = NewsPosition;
        }

        void GameGridCollisions::DrawCells()
        {

            for (int i = 0; i <= Arrows; i++)
            {
                //al_draw_line(GridPosition.x, GridPosition.y + i * CellSize, GridPosition.x + Columns * CellSize, GridPosition.y + i * CellSize, al_map_rgba(255, 255, 255, 255), 1);
                ResourceManager::GetInstance().RenderLine(glm::vec3(GridPosition.x, GridPosition.y + i * CellSize, 0.0f), glm::vec3(GridPosition.x + Columns * CellSize, GridPosition.y + i * CellSize, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.5f, 2.0f);
            }
            for (int j = 0; j <= Columns; j++)
            {
                //al_draw_line(GridPosition.x + j * CellSize, GridPosition.y, GridPosition.x + j * CellSize, GridPosition.y + Arrows * CellSize, al_map_rgba(255, 255, 255, 255), 1);
                ResourceManager::GetInstance().RenderLine(glm::vec3(GridPosition.x + j * CellSize, GridPosition.y, 0.0f), glm::vec3(GridPosition.x + j * CellSize, GridPosition.y + Arrows * CellSize, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.5f, 2.0f);
            }
        }

        GameGridCollisions::~GameGridCollisions()
        {
            this->ClearGrid();
        }

        Cell *GameGridCollisions::GetCell(int Arrow, int Column)
        {
            if (Arrow >= 0 && Arrow < this->Arrows && Column >= 0 && Column < this->Columns)
            {
                return &Cells[Arrow][Column];
            }
            else
            {
                return nullptr;
            }
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
            EntitiesOnGrid++;
        }

        void GameGridCollisions::RemoveObject(Body *obj)
        {

            Cell *cell = GetCellByPositionAndSize(obj->GetPosition(), obj->GetSize());

            if (!cell)
                return;

            auto &objects = cell->objects;
            objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
            EntitiesOnGrid--;
        }

        void GameGridCollisions::ClearGrid()
        {
            for (int i = 0; i < Arrows; i++)
            {
                for (int j = 0; j < Columns; j++)
                {
                    Cells[i][j].objects.clear();
                }
            }
            EntitiesOnGrid = 0;
        }

        Cell *GameGridCollisions::GetCellByPositionAndSize(glm::vec2 position, glm::vec2 size)
        {
            int relX = position.x - GridPosition.x;
            int relY = position.y - GridPosition.y;

            if(relX < 0 || relY < 0 || relX + size.x > GridSize.x || relY + size.y > GridSize.y)
            {
                return nullptr;
            }

            int Column = relX / CellSize;
            int Arrow = relY / CellSize;

            if (Arrow >= 0 && Arrow < this->Arrows && Column >= 0 && Column < this->Columns)
            {
                return &Cells[Arrow][Column];
            }
            else
            {
                return nullptr;
            }
        }

        void GameGridCollisions::Check_cells_collisions(Cell *cell_1, Cell *cell_2)
        {
            for (auto &obj_1 : cell_1->objects)
            {
                for (auto &obj_2 : cell_2->objects)
                {
                    if (obj_1 != obj_2 && obj_1->GetParent()->IsAlive() && obj_2->GetParent()->IsAlive())
                    {
                        if (RectToRectCollisionBody(obj_1, obj_2))
                        {
                            obj_1->OnCollision(obj_2, GetCollisionSide(obj_1, obj_2));
                        }
                    }
                }
            }
        }

        void GameGridCollisions::Find_collision_grid()
        {
            for (int arrow = 0; arrow < this->Arrows; ++arrow)
            {
                for (int column = 0; column < this->Columns; ++column)
                {
                    auto current_cell = this->GetCell(arrow, column);
                    for (int dcolumn = -1; dcolumn <= 1; ++dcolumn)
                    {
                        for (int darrow = -1; darrow <= 1; ++darrow)
                        {
                            auto other_cell = this->GetCell(arrow + darrow, column + dcolumn);
                            if (current_cell && other_cell)
                            {
                                Check_cells_collisions(current_cell, other_cell);
                            }
                        }
                    }
                }
            }
        }

        bool GameGridCollisions::RectToRectCollisionBody(Body *body1, Body *body2)
        {
            return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
                    body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
                    body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
                    body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
        }


        BodyCollisionSide GameGridCollisions::GetCollisionSide(Body *body1, Body *body2)
        {

            float deltaLeft = (body2->GetPosition().x + body2->GetSize().x) - body1->GetPosition().x;
            float deltaRight = (body1->GetPosition().x + body1->GetSize().x) - body2->GetPosition().x;
            float deltaTop = (body2->GetPosition().y + body2->GetSize().y) - body1->GetPosition().y;
            float deltaBottom = (body1->GetPosition().y + body1->GetSize().y) - body2->GetPosition().y;

            float minDelta = std::min({deltaLeft, deltaRight, deltaTop, deltaBottom});

            if (minDelta == deltaLeft) return BodyCollisionSide::LEFT;
            if (minDelta == deltaRight) return BodyCollisionSide::RIGHT;
            if (minDelta == deltaTop) return BodyCollisionSide::TOP;
            if (minDelta == deltaBottom) return BodyCollisionSide::BOTTOM;

            return BodyCollisionSide::NONE;
        }


    }

#endif
