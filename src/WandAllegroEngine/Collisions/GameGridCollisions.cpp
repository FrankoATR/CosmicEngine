#include "GameGridCollisions.h"
#include "../Models/GameObject.h"

GameGridCollisions::GameGridCollisions(Vec2 GridPosition, int Arrows, int Columns, int CellSize) : GridPosition(GridPosition), Arrows(Arrows), Columns(Columns), CellSize(CellSize), EntitiesOnGrid(0)
{
    Cells.resize(Arrows);
    for (int i = 0; i < Arrows; i++)
    {
        Cells[i].resize(Columns);
        for (int j = 0; j < Columns; j++)
        {
            Cells[i][j] = Cell(Vec2(GridPosition.x + j * CellSize, GridPosition.y + i * CellSize));
        }
    }
}

void GameGridCollisions::DrawCells()
{

    for (int i = 0; i <= Arrows; i++)
    {
        al_draw_line(GridPosition.x, GridPosition.y + i * CellSize, GridPosition.x + Columns * CellSize, GridPosition.y + i * CellSize, al_map_rgba(255, 255, 255, 255), 1);
    }
    for (int j = 0; j <= Columns; j++)
    {
        al_draw_line(GridPosition.x + j * CellSize, GridPosition.y, GridPosition.x + j * CellSize, GridPosition.y + Arrows * CellSize, al_map_rgba(255, 255, 255, 255), 1);
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

void GameGridCollisions::AddObject(GameBodyObject *obj)
{

    Cell *cell = GetCellByPosition(obj->GetPosition());
    if (!cell)
    {
        obj->GetParent()->Destroy();
        return;
    }

    cell->objects.push_back(obj);
    EntitiesOnGrid++;
}

void GameGridCollisions::RemoveObject(GameBodyObject *obj)
{

    Cell *cell = GetCellByPosition(obj->GetPosition());

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

Cell *GameGridCollisions::GetCellByPosition(Vec2 position)
{
    int relX = position.x - GridPosition.x;
    int relY = position.y - GridPosition.y;

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
            if (obj_1 != obj_2)
            {
                if (RectToRectCollisionBody(obj_1, obj_2))
                {
                    obj_1->GetParent()->OnCollision(obj_2->GetParent());
                    obj_2->GetParent()->OnCollision(obj_1->GetParent());
                }
            }
        }
    }
}

void GameGridCollisions::Find_collision_grid()
{
    for (int column = 0; column < this->Columns; ++column)
    {
        for (int arrow = 0; arrow < this->Arrows; ++arrow)
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

bool GameGridCollisions::RectToRectCollisionBody(GameBodyObject *body1, GameBodyObject *body2)
{
    return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
            body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
            body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
            body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
}