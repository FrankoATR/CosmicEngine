#include "puzzle_tile.hpp"

#include "../systems/grid.hpp"
#include "../systems/class_ids.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <sstream>

namespace PuzzleRPG
{
    namespace
    {
        glm::vec3 PlaceholderColor(const TileLayer &layer)
        {
            if (layer.sheet == SheetId::kNone) return glm::vec3(0.05f, 0.05f, 0.08f);
            if (IsWallSheet(layer.sheet)) return glm::vec3(0.15f, 0.15f, 0.20f);
            // Suelo: ligera variacion segun (row,col) para distinguir cuando no hay sprite.
            float hue = static_cast<float>(((layer.row * 13 + layer.col) * 37) % 360) / 360.0f;
            float r = 0.4f + 0.3f * (1.0f - hue);
            float g = 0.45f + 0.25f * hue;
            float b = 0.55f;
            return glm::vec3(r, g, b);
        }
    }

    PuzzleTile::PuzzleTile(TileLayer layer, int gridX, int gridY)
        : CosmicEngine::Object("PuzzleTile",
                               CellToWorld(Cell(gridX, gridY)),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               static_cast<short>(std::max(0, layer.drawLayer))),
          tileLayer(layer),
          gridX(gridX),
          gridY(gridY)
    {
        SetColor(PlaceholderColor(layer));
    }

    void PuzzleTile::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        if (DrawSheetCell(SheetKeyForSheetId(tileLayer.sheet), tileLayer.row, tileLayer.col, position, size))
        {
            return;
        }

        // Si no hay sprite (asset faltante o sheet=None), solo pintamos el
        // placeholder cuando el modo debug visual esta activo.
        if (!IsPuzzleDebugVisualsEnabled()) return;

        // Fondo del tile.
        RS_MN.RenderRectangle(position,
                              position + size,
                              glm::vec2(0.0f),
                              glm::vec2(0.0f),
                              GetColor(),
                              1.0f,
                              1.0f,
                              true);

        // Borde para distinguir celdas.
        RS_MN.RenderRectangle(position,
                              position + size,
                              glm::vec2(0.0f),
                              glm::vec2(0.0f),
                              glm::vec3(0.05f, 0.05f, 0.08f),
                              0.7f,
                              1.0f,
                              false);
    }
}
