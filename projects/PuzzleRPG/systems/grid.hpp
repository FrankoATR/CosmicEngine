#ifndef PUZZLERPG_GRID_HPP
#define PUZZLERPG_GRID_HPP

#include <glm/glm.hpp>

namespace PuzzleRPG
{
    // El mundo del juego es un grid 2D donde cada casilla mide kTileSize pixeles.
    // El origen (0,0) corresponde a la esquina superior izquierda de la sala.
    // Las posiciones del Object de CosmicEngine se calculan en pixeles desde
    // ese origen; la camara sigue al jugador en coordenadas de mundo.
    // Los sprites del juego son de 16x16 pixeles por celda; la camara aplica
    // un zoom mayor por defecto para que el resultado en pantalla quede grande.
    constexpr float kTileSize = 16.0f;

    struct Cell
    {
        int x;
        int y;

        Cell() : x(0), y(0) {}
        Cell(int xx, int yy) : x(xx), y(yy) {}

        bool operator==(const Cell &other) const { return x == other.x && y == other.y; }
        bool operator!=(const Cell &other) const { return !(*this == other); }
    };

    // Convierte una celda en posicion de mundo (esquina superior izquierda).
    inline glm::vec2 CellToWorld(const Cell &cell)
    {
        return glm::vec2(static_cast<float>(cell.x) * kTileSize,
                         static_cast<float>(cell.y) * kTileSize);
    }

    // Convierte una posicion de mundo a la celda que contiene ese punto.
    inline Cell WorldToCell(glm::vec2 worldPosition)
    {
        return Cell(static_cast<int>(std::floor(worldPosition.x / kTileSize)),
                    static_cast<int>(std::floor(worldPosition.y / kTileSize)));
    }

    // Devuelve la celda adyacente segun una direccion cardinal discreta.
    // 0 = abajo, 1 = arriba, 2 = izquierda, 3 = derecha.
    inline Cell CellInFront(const Cell &cell, int facing)
    {
        switch (facing)
        {
        case 0: return Cell(cell.x, cell.y + 1);
        case 1: return Cell(cell.x, cell.y - 1);
        case 2: return Cell(cell.x - 1, cell.y);
        case 3: return Cell(cell.x + 1, cell.y);
        }
        return cell;
    }

    // Devuelve el vector unitario (en pixeles) asociado a la direccion cardinal.
    inline glm::vec2 FacingToVector(int facing)
    {
        switch (facing)
        {
        case 0: return glm::vec2(0.0f, 1.0f);
        case 1: return glm::vec2(0.0f, -1.0f);
        case 2: return glm::vec2(-1.0f, 0.0f);
        case 3: return glm::vec2(1.0f, 0.0f);
        }
        return glm::vec2(0.0f, 1.0f);
    }
}

#endif
