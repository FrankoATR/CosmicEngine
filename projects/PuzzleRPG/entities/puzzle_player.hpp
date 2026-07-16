#ifndef PUZZLERPG_PUZZLE_PLAYER_HPP
#define PUZZLERPG_PUZZLE_PLAYER_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

#include <array>
#include <string>

namespace PuzzleRPG
{
    class LevelRuntime;

    // Jugador en grid. Se mueve casilla a casilla con un pequenyo tween para
    // dar fluidez. Mantiene la facing direction independiente del movimiento
    // (puede "mirar" en una direccion sin caminar). Cada jugador tiene su
    // inventario fijo de 4 huecos (cada hueco guarda un id de textura).
    class PuzzlePlayer : public CosmicEngine::Object
    {
    public:
        struct InventorySlot
        {
            int textureId = 0;
            int objectId  = 0;
        };

        static constexpr int kInventorySize = 1;

        PuzzlePlayer(int playerIndex, Cell spawn, bool isLocal, std::string username,
                 int drawLayer, bool showNameplate);

        void update(float deltaTime) override;
        void draw() const override;

        int  GetPlayerIndex() const { return playerIndex; }
        Cell GetCell() const { return cell; }
        int  GetFacing() const { return facing; }
        bool IsMoving() const { return tweenActive; }
        bool IsLocal() const { return isLocal; }
        const std::string &GetUsername() const { return username; }
        void SetUsername(const std::string &newUsername) { username = newUsername; }

        void TeleportToCell(Cell target);
        void BeginMoveTo(Cell target);
        void SetFacing(int newFacing);

        // Bloquea movimiento/acciones durante `seconds` (animacion de teletransporte).
        void Freeze(float seconds);
        bool IsFrozen() const { return freezeTimer > 0.0f; }

        // Id del ultimo teletransporte sobre el que estuvimos. Sirve para no
        // re-disparar el efecto hasta que el jugador salga y vuelva a entrar.
        int  GetLastTeleporterId() const { return lastTeleporterId; }
        void SetLastTeleporterId(int id) { lastTeleporterId = id; }

        // Inventario.
        const std::array<int, kInventorySize> &GetInventory() const { return inventory; }
        const std::array<int, kInventorySize> &GetInventoryObjectIds() const { return inventoryObjectIds; }
        void SetInventoryAll(const std::array<int, kInventorySize> &inv) { inventory = inv; }
        void SetInventoryObjectIds(const std::array<int, kInventorySize> &ids) { inventoryObjectIds = ids; }
        void SetInventoryState(const std::array<int, kInventorySize> &inv,
                               const std::array<int, kInventorySize> &ids)
        {
            inventory = inv;
            inventoryObjectIds = ids;
        }
        int  FindFreeInventorySlot() const;
        bool AddToInventory(int textureId, int objectId = 0);
        bool RemoveFromInventoryAt(int slot, int &outTextureId, int &outObjectId);
        int  PeekInventoryAt(int slot) const;
        int  PeekInventoryObjectIdAt(int slot) const;
        int  GetSelectedSlot() const { return selectedSlot; }
        void SetSelectedSlot(int slot);

        // Velocidad del tween (segundos por celda). This sets the base
        // duration used for moves; actual duration for a move may be
        // adjusted (e.g. movement modifiers).
        void SetTweenDuration(float seconds) { baseTweenDuration = std::max(0.05f, seconds); tweenDuration = baseTweenDuration; }

    private:
        int  playerIndex;
        Cell cell;
        Cell tweenFromCell;
        Cell tweenToCell;
        bool tweenActive;
        float tweenProgress;
        float tweenDuration;
        // Base tween duration (seconds per cell) used to compute adjusted
        // tweenDuration when applying movement speed modifiers.
        float baseTweenDuration;
        int  facing;
        bool isLocal;
        std::string username;
        bool showNameplate;
        std::array<int, kInventorySize> inventory;
        std::array<int, kInventorySize> inventoryObjectIds;
        int selectedSlot;
        float freezeTimer       = 0.0f;
        int   lastTeleporterId  = 0;
        SpriteSheetVisual spriteVisual;
    };
}

#endif
