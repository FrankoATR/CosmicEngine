#include "GameGridCollisions.h"
#include "../Models/GameObject.h"


GameGridCollisions::GameGridCollisions(Vec2 GridPosition, int Arrows, int Columns, int CellSize) : GridPosition(GridPosition), Arrows(Arrows), Columns(Columns), CellSize(CellSize), EntitiesOnGrid(0)
{
    Cells = new Cell*[Arrows];
    for (int i = 0; i < Arrows; i++) {
        Cells[i] = new Cell[Columns];
        for (int j = 0; j < Columns; j++) {
            Cells[i][j] = Cell(Vec2(GridPosition.x + j * CellSize, GridPosition.y + i * CellSize));
            std::cout << "Cell[" << i << "][" << j << "] initialized at position: "
                    << Cells[i][j].Position.x << ", " << Cells[i][j].Position.y << std::endl;
        }
    }
}


void GameGridCollisions::DrawCells(){

    /*
    for(int i=0; i<=Arrows; i++){
        al_draw_line(GridPosition.x, GridPosition.y+i*CellSize, GridPosition.x+Columns*CellSize, GridPosition.y+i*CellSize, al_map_rgba(255,255,255,255), 1);
    }
    for(int j=0; j<=Columns; j++){
        al_draw_line(GridPosition.x+j*CellSize, GridPosition.y, GridPosition.x+j*CellSize, GridPosition.y+Arrows*CellSize, al_map_rgba(255,255,255,255), 1);
    }

    */

    for (int i = 0; i < Arrows; i++) {
        for (int j = 0; j < Columns; j++) {
            Cell* cell = GetCell(i, j);
            if(cell){
                int x = cell->Position.x;
                int y = cell->Position.y;
                if(cell->objects.empty()) {
                    al_draw_rectangle(x+1, y+1, x+CellSize-1, y+CellSize-1, al_map_rgba(255,255,255,255), 1);
                }
                else{
                    al_draw_rectangle(x+1, y+1, x+CellSize-1, y+CellSize-1, al_map_rgba(255,0,0,255), 1);
                }
            }
        }
    }
}


GameGridCollisions::~GameGridCollisions() {
    for (int i = 0; i < Arrows; ++i)
        delete[] Cells[i];
    delete[] Cells;
    EntitiesOnGrid = 0;;

}


Cell* GameGridCollisions::GetCell(int Arrow, int Column) {
    if (Arrow >= 0 && Arrow < this->Arrows && Column >= 0 && Column < this->Columns) {
        return &Cells[Arrow][Column];
    } else {
        return nullptr;
    }
}


void GameGridCollisions::AddObject(GameBodyObject* obj) {
    int relX = obj->GetPosition().x - GridPosition.x;
    int relY = obj->GetPosition().y - GridPosition.y;

    if(relX >= 0 && relY >= 0) {
        int Column = relX / CellSize;
        int Arrow = relY / CellSize;

        if (Arrow < Arrows && Column < Columns) {
            Cell* Cell = GetCell(Arrow, Column);
            if (Cell) {
                Cell->objects.push_back(obj);
                //std::cout << "Insert: " << obj->GetObjectId() << " in arrow: "<< Arrow << ", column: "<< Column << std::endl;
                EntitiesOnGrid++;
            }
        }
    }
}



void GameGridCollisions::RemoveObject(GameBodyObject* obj) {
    int relX = obj->GetPosition().x - GridPosition.x;
    int relY = obj->GetPosition().y - GridPosition.y;

    if(relX >= 0 && relY >= 0) {
        int Column = relX / CellSize;
        int Arrow = relY / CellSize;

        if (Arrow < Arrows && Column < Columns) {
            Cell* Cell = GetCell(Arrow, Column);

            if (Cell) {
                auto& objects = Cell->objects;
                objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
                EntitiesOnGrid--;
            }
        }
    }

}


void GameGridCollisions::UpdatePositions(){
    std::vector<std::tuple<GameBodyObject*, int, int>> toMove;

    for (int y = 0; y < Arrows; ++y) {
        for (int x = 0; x < Columns; ++x) {
            auto& cell = Cells[y][x];
            auto it = cell.objects.begin();
            while (it != cell.objects.end()) {
                GameBodyObject* obj = *it;
                int relX = obj->GetPosition().x - GridPosition.x;
                int relY = obj->GetPosition().y - GridPosition.y;

                if(relX >= 0 && relY >= 0) {
                    int newColumn = relX / CellSize;
                    int newArrow = relY / CellSize;

                    if (newArrow < Arrows && newColumn < Columns && (newArrow != y || newColumn != x)) {
                        toMove.push_back(std::make_tuple(obj, newArrow, newColumn));
                        it = cell.objects.erase(it);
                    } else {
                        ++it;
                    }
                }
                else{
                    ++it;
                }
            }
        }
    }

    for (auto& item : toMove) {
        GameBodyObject* obj = std::get<0>(item);
        int newArrow = std::get<1>(item);
        int newColumn = std::get<2>(item);

        if (newArrow >= 0 && newArrow < Arrows && newColumn >= 0 && newColumn < Columns) {
            Cells[newArrow][newColumn].objects.push_back(obj);
        }
    }

}



void GameGridCollisions::Check_cells_collisions(Cell* cell_1, Cell* cell_2) {
    for (auto obj_1 : cell_1->objects) {
        for (auto obj_2 : cell_2->objects) {
            if (obj_1 != obj_2) {
                if (RectToRectCollisionBody(obj_1, obj_2)) {
                    obj_1->GetParent()->OnCollision(obj_2->GetParent());
                    obj_2->GetParent()->OnCollision(obj_1->GetParent());
                }
            }
        }
    }
}


void GameGridCollisions::Find_collision_grid() {
    for (int column = 1; column < this->Columns - 1; ++column) {
        for (int arrow = 1; arrow < this->Arrows - 1; ++arrow) {
            auto current_cell = this->GetCell(arrow, column);
            for (int dcolumn = -1; dcolumn <= 1; ++dcolumn) {
                for (int darrow = -1; darrow <= 1; ++darrow) {
                    auto other_cell = this->GetCell(arrow + darrow, column + dcolumn);
                    if (current_cell && other_cell){
                        Check_cells_collisions(current_cell, other_cell);
                    }
                }
            }
        }
    }
}


bool GameGridCollisions::RectToRectCollisionBody(GameBodyObject* body1, GameBodyObject* body2) {
    return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
            body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
            body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
            body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
}