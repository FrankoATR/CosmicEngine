#include "puzzle_player.hpp"

#include "../systems/class_ids.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <algorithm>
#include <sstream>

namespace PuzzleRPG
{
    PuzzlePlayer::PuzzlePlayer(int playerIndex, Cell spawn, bool isLocal, std::string username,
                               int drawLayer, bool showNameplate)
        : CosmicEngine::Object("PuzzlePlayer",
                       // Logical object occupies 1x2 tiles; position top-left so
                       // its bottom cell aligns with `spawn`.
                       CellToWorld(spawn) - glm::vec2(0.0f, kTileSize * 1.0f),
                       // Logical size: width 1 tile, height 2 tiles
                       glm::vec2(kTileSize * 1.0f, kTileSize * 2.0f),
                               0.0f,
                               static_cast<short>(drawLayer)),
          playerIndex(playerIndex),
          cell(spawn),
          tweenFromCell(spawn),
          tweenToCell(spawn),
          tweenActive(false),
          tweenProgress(0.0f),
          baseTweenDuration(0.18f),
          tweenDuration(baseTweenDuration),
          facing(0),
          isLocal(isLocal),
          username(std::move(username)),
            showNameplate(showNameplate),
          selectedSlot(0)
    {
        inventory.fill(0);
        inventoryObjectIds.fill(0);
                transparency = 1.0f;
        // Color distintivo segun el indice de jugador (P1 = naranja, P2 = cian).
        SetColor(playerIndex == 0 ? glm::vec3(0.95f, 0.55f, 0.20f)
                                  : glm::vec3(0.25f, 0.75f, 0.95f));
        spriteVisual.SetAnimatedClip(ClipForPlayer(facing, false), GetID(), true);
    }

    void PuzzlePlayer::TeleportToCell(Cell target)
    {
        cell = target;
        tweenFromCell = target;
        tweenToCell   = target;
        tweenActive   = false;
        tweenProgress = 0.0f;
        // Keep the logical position (1x2) aligned to the bottom cell
        SetPosition(CellToWorld(target) - glm::vec2(0.0f, kTileSize * 1.0f));
        spriteVisual.SetAnimatedClip(ClipForPlayer(facing, false), GetID(), false);
    }

    void PuzzlePlayer::BeginMoveTo(Cell target)
    {
        if (tweenActive) return;
        tweenFromCell = cell;
        tweenToCell   = target;
        tweenActive   = true;
        tweenProgress = 0.0f;
        cell          = target; // semantica: el jugador "ya ocupa" la nueva celda
        spriteVisual.SetAnimatedClip(ClipForPlayer(facing, true), GetID(), false);
        // Apply movement speed modifier: set move duration so movement speed
        // becomes 60% of base speed (i.e., duration increases by 1/0.6).
        const float speedFactor = 0.6f;
        tweenDuration = baseTweenDuration / speedFactor;
    }

    void PuzzlePlayer::SetFacing(int newFacing)
    {
        facing = newFacing;
        spriteVisual.SetAnimatedClip(ClipForPlayer(facing, tweenActive), GetID(), false);
    }

    void PuzzlePlayer::Freeze(float seconds)
    {
        if (seconds > freezeTimer) freezeTimer = seconds;
    }

    void PuzzlePlayer::update(float deltaTime)
    {
        if (freezeTimer > 0.0f)
        {
            freezeTimer -= deltaTime;
            if (freezeTimer < 0.0f) freezeTimer = 0.0f;
        }

        if (!tweenActive) return;
        tweenProgress += deltaTime / tweenDuration;
        if (tweenProgress >= 1.0f)
        {
            tweenActive   = false;
            tweenProgress = 1.0f;
            SetPosition(CellToWorld(tweenToCell) - glm::vec2(0.0f, kTileSize * 1.0f));
            spriteVisual.SetAnimatedClip(ClipForPlayer(facing, false), GetID(), false);
            // Restore tweenDuration to base for subsequent moves
            tweenDuration = baseTweenDuration;
            return;
        }
        glm::vec2 a = CellToWorld(tweenFromCell) - glm::vec2(0.0f, kTileSize * 1.0f);
        glm::vec2 b = CellToWorld(tweenToCell)   - glm::vec2(0.0f, kTileSize * 1.0f);
        SetPosition(a + (b - a) * tweenProgress);
        spriteVisual.SetAnimatedClip(ClipForPlayer(facing, true), GetID(), false);
    }

    int PuzzlePlayer::FindFreeInventorySlot() const
    {
        for (int i = 0; i < kInventorySize; ++i)
        {
            if (inventory[static_cast<std::size_t>(i)] == 0) return i;
        }
        return -1;
    }

    bool PuzzlePlayer::AddToInventory(int textureId, int objectId)
    {
        int slot = FindFreeInventorySlot();
        if (slot < 0) return false;
        inventory[static_cast<std::size_t>(slot)] = textureId;
        inventoryObjectIds[static_cast<std::size_t>(slot)] = objectId;
        return true;
    }

    bool PuzzlePlayer::RemoveFromInventoryAt(int slot, int &outTextureId, int &outObjectId)
    {
        if (slot < 0 || slot >= kInventorySize) return false;
        int value = inventory[static_cast<std::size_t>(slot)];
        if (value == 0) return false;
        outTextureId = value;
        outObjectId = inventoryObjectIds[static_cast<std::size_t>(slot)];
        inventory[static_cast<std::size_t>(slot)] = 0;
        inventoryObjectIds[static_cast<std::size_t>(slot)] = 0;
        return true;
    }

    int PuzzlePlayer::PeekInventoryAt(int slot) const
    {
        if (slot < 0 || slot >= kInventorySize) return 0;
        return inventory[static_cast<std::size_t>(slot)];
    }

    int PuzzlePlayer::PeekInventoryObjectIdAt(int slot) const
    {
        if (slot < 0 || slot >= kInventorySize) return 0;
        return inventoryObjectIds[static_cast<std::size_t>(slot)];
    }

    void PuzzlePlayer::SetSelectedSlot(int slot)
    {
        if (slot < 0) slot = 0;
        if (slot >= kInventorySize) slot = kInventorySize - 1;
        selectedSlot = slot;
    }

    void PuzzlePlayer::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        // Render sprite scaled to the logical footprint: 1 tile wide x 2 tiles tall.
        // The source artwork frames are 32x64 px, but we scale them down so the
        // sprite occupies exactly one grid column and two grid rows.
        glm::vec2 renderSize = glm::vec2(kTileSize * 1.0f, kTileSize * 2.0f);
        glm::vec2 renderPos  = glm::vec2(
            position.x + (size.x - renderSize.x) * 0.5f,
            position.y + size.y - renderSize.y);

        if (!spriteVisual.Draw(renderPos, renderSize, GetRotation(), glm::vec3(1.0f), transparency))
        {
            if (IsPuzzleDebugVisualsEnabled())
            {
                // Cuerpo del jugador: ahora solo la celda inferior (la que cubre
                // la parte de "pies"). Centrado horizontalmente dentro del ancho
                // del sprite y limitado a una celda de altura.
                glm::vec2 lowerCellOrigin = position + glm::vec2((size.x - kTileSize) * 0.5f, size.y - kTileSize);
                glm::vec2 bodyA = lowerCellOrigin + glm::vec2(kTileSize * 0.15f, kTileSize * 0.15f);
                glm::vec2 bodyB = lowerCellOrigin + glm::vec2(kTileSize * 0.85f, kTileSize * 0.85f);
                RS_MN.RenderRectangle(bodyA, bodyB, glm::vec2(0.0f), glm::vec2(0.0f), GetColor(), 1.0f, 1.0f, true);
                RS_MN.RenderRectangle(bodyA, bodyB, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.05f), 1.0f, 1.5f, false);

                // Indicador de facing: triangulo aproximado mediante un rectangulo
                // pequenyo desplazado en la direccion correcta.
                glm::vec2 nose;
                glm::vec2 noseSize(kTileSize * 0.15f, kTileSize * 0.15f);
                glm::vec2 center = position + size * 0.5f;
                switch (facing)
                {
                case 0: nose = glm::vec2(center.x - noseSize.x * 0.5f, position.y + size.y - noseSize.y - 2.0f); break;
                case 1: nose = glm::vec2(center.x - noseSize.x * 0.5f, position.y + 2.0f); break;
                case 2: nose = glm::vec2(position.x + 2.0f, center.y - noseSize.y * 0.5f); break;
                case 3: nose = glm::vec2(position.x + size.x - noseSize.x - 2.0f, center.y - noseSize.y * 0.5f); break;
                }
                RS_MN.RenderRectangle(nose, nose + noseSize, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.95f, 0.95f, 0.95f), 1.0f, 1.0f, true);
            }
        }

        // Nombre encima.
        if (showNameplate && !username.empty())
        {
            constexpr float kNameScale = 0.28f;
            float approxWidth = static_cast<float>(username.size()) * kNameScale * 10.0f;
            RS_MN.RenderText(username,
                             "puzzle_font",
                             glm::vec3(position.x + size.x * 0.5f - approxWidth * 0.5f,
                                       position.y - 12.0f,
                                       0.0f),
                             glm::vec3(kNameScale),
                             glm::vec3(0.0f),
                             glm::vec3(0.0f),
                             glm::vec3(1.0f),
                             1.0f);
        }
    }
}
