#ifndef PUZZLERPG_DOOR_OBJECT_HPP
#define PUZZLERPG_DOOR_OBJECT_HPP

#include "../systems/grid.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/models/object/object.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace PuzzleRPG
{
    // Puerta. Se abre cuando todos sus activadores estan activos (AND logico).
    // Mantiene la lista de ids de placas / botones que la controlan y se
    // suscribe a un evento global "puzzlerpg.activator_state.<id>".
    class DoorObject : public CosmicEngine::Object
    {
    public:
        DoorObject(int id, int textureId, Cell cell);
        ~DoorObject() override;

        void draw() const override;

        int  GetObjectId() const { return objectId; }
        Cell GetCell() const { return cell; }
        bool IsOpen() const { return open; }
        void SetOpen(bool value);

        // Registra a esta puerta como oyente de los estados de cada activador.
        void BindActivators(const std::vector<int> &activatorIds);

        // Llamado por LevelRuntime cuando cambia el estado de un activador.
        void OnActivatorChanged(int activatorId, bool active);

    private:
        int objectId;
        int textureId;
        Cell cell;
        bool open;
        std::vector<int> activatorIds;
        std::vector<bool> activatorStates;
        std::vector<std::uint64_t> listenerIds;
        std::vector<std::string> listenerEventNames;
        SpriteSheetVisual spriteVisual;

        void RecomputeOpenState();
    };
}

#endif
