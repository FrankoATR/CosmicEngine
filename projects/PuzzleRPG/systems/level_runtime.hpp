#ifndef PUZZLERPG_LEVEL_RUNTIME_HPP
#define PUZZLERPG_LEVEL_RUNTIME_HPP

#include "../entities/button_object.hpp"
#include "../entities/collision_block.hpp"
#include "../entities/decor_object.hpp"
#include "../entities/door_object.hpp"
#include "../entities/firefly_light_object.hpp"
#include "../entities/pickup_object.hpp"
#include "../entities/pressure_plate.hpp"
#include "../entities/puzzle_player.hpp"
#include "../entities/puzzle_tile.hpp"
#include "../entities/pushable_object.hpp"
#include "../entities/sign_object.hpp"
#include "../entities/teleporter_object.hpp"
#include "../entities/torch_light_object.hpp"
#include "../entities/wall_block_object.hpp"
#include "level_data.hpp"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace PuzzleRPG
{
    // Resultado de un intento de movimiento por parte de un jugador.
    enum class MoveResult
    {
        Blocked,
        Walked,
        Pushed
    };

    // LevelRuntime es la fuente unica de verdad del estado del puzzle. Las
    // coordenadas de grid son autoritativas y sustituyen al sistema de
    // colisiones AABB del motor. Este modulo:
    //   - Mantiene la matriz de tiles y la lista de objetos vivos.
    //   - Ofrece operaciones de alto nivel: caminar, empujar, recoger, dejar,
    //     interactuar, evaluar placas, conmutar botones.
    //   - Recalcula el estado abierto/cerrado de cada puerta como AND de sus
    //     activadores.
    class LevelRuntime
    {
    public:
        LevelRuntime();
        ~LevelRuntime();

        // Construye el runtime a partir de los datos del nivel y los registra
        // en el ObjectManager para que se dibujen y actualicen.
        void Build(const LevelData &level);
        // Libera todos los objetos del nivel actual.
        void Teardown();

        const LevelData &GetLevelData() const { return levelData; }

        // --- Consulta ---
        bool IsCellInsideLevel(Cell cell) const;
        bool IsCellWalkable(Cell cell) const; // tiles + puertas cerradas + empujables
        bool IsCellFree(Cell cell) const;     // walkable AND no jugadores

        PuzzlePlayer   *GetPlayerAtCell(Cell cell) const;
        PushableObject *GetPushableAtCell(Cell cell) const;
        DoorObject     *GetDoorAtCell(Cell cell) const;
        ButtonObject   *GetButtonAtCell(Cell cell) const;
        PickupObject   *GetPickupAtCell(Cell cell) const;
        PressurePlate  *GetPlateAtCell(Cell cell) const;
        SignObject     *GetSignAtCell(Cell cell) const;
        TeleporterObject *GetTeleporterAtCell(Cell cell) const;
        TeleporterObject *GetTeleporterPartner(const TeleporterObject &teleporter) const;
        const std::vector<TeleporterObject *> &GetTeleporters() const { return teleporters; }
        const std::vector<PickupObject *>     &GetPickups()     const { return pickups; }
        const std::vector<PushableObject *>   &GetPushables()   const { return pushables; }
        const std::vector<ButtonObject *>     &GetButtons()     const { return buttons; }
        const std::vector<DoorObject *>       &GetDoors()       const { return doors; }
        const std::vector<PressurePlate *>    &GetPlates()      const { return plates; }
        const std::vector<WallBlockObject *>  &GetWallBlocks()  const { return wallBlocks; }
        const std::vector<TorchLightObject *> &GetTorches()     const { return torches; }
        const std::vector<FireflyLightObject *> &GetFireflies() const { return fireflies; }
        const std::vector<SignObject *>        &GetSigns()           const { return signs; }
        const std::vector<DecorObject *>      &GetDecors()          const { return decors; }
        const std::vector<CollisionBlock *>   &GetCollisionBlocks() const { return collisionBlocks; }

        // --- Mutaciones ---
        // Intenta mover un jugador en la direccion `facing`. Cambia internamente
        // la celda actual del jugador y devuelve el resultado. No anima: se
        // espera que la capa de presentacion arranque su tween.
        MoveResult TryMovePlayer(PuzzlePlayer &player, int facing);

        // Recoge el pickup que el jugador tiene enfrente y lo guarda en su
        // inventario. Devuelve true si tuvo exito.
        bool TryPickupInFront(PuzzlePlayer &player);

        // Suelta el item seleccionado del inventario en la celda de enfrente
        // (si esta libre). Devuelve true si tuvo exito.
        bool TryPlaceInFront(PuzzlePlayer &player);

        // Interactua con un boton enfrente. Devuelve true si se activo / conmuto.
        bool TryInteractInFront(PuzzlePlayer &player);

        // Devuelve el texto de dialogo del objeto al que mira el jugador
        // (si tiene); cadena vacia si no aplica.
        std::string GetDialogInFront(const PuzzlePlayer &player) const;

        // Registra al jugador en el runtime para que sea consultable.
        void AddPlayer(PuzzlePlayer *player);
        void RemovePlayer(PuzzlePlayer *player);
        const std::vector<PuzzlePlayer *> &GetPlayers() const { return players; }

        // Devuelve un id local libre para un pickup nuevo creado in-game.
        int AllocateRuntimeObjectId();

        // Re-evalua todas las placas (publico: el cliente lo llama tras aplicar
        // un movimiento replicado por red).
        void RebuildPlateStates();

        // Re-evalua todas las placas (util tras una replicacion de movimiento
        // recibida por red, ya que el cliente no ejecuta TryMovePlayer).
        void ForceReevaluateAll();

        // True si la condicion de victoria del nivel se ha cumplido.
        bool IsLevelComplete() const { return levelComplete; }

        // Callback que se dispara cuando un jugador entra en una celda con
        // teletransporte aun no "armado" para el. La capa de juego decide si
        // congelar al jugador y mover (host) o no hacer nada (cliente).
        using TeleporterStepCallback = std::function<void(PuzzlePlayer &player, TeleporterObject &teleporter)>;
        void SetTeleporterStepCallback(TeleporterStepCallback callback) { onTeleporterStep = std::move(callback); }

        // Callback opcional al cumplirse la condicion de victoria.
        using LevelCompletedCallback = std::function<void()>;
        void SetLevelCompletedCallback(LevelCompletedCallback callback) { onLevelCompleted = std::move(callback); }

        // Llamado por la capa de juego cada vez que un jugador acaba de aterrizar
        // en una celda nueva (host: tras TryMovePlayer; cliente: tras aplicar
        // un Msg_PlayerState recibido). Gestiona el rearme de teletransportes.
        void HandlePlayerArrived(PuzzlePlayer &player);

    private:
        LevelData levelData;
        std::vector<PuzzleTile *>      tiles;
        std::vector<PickupObject *>    pickups;
        std::vector<PressurePlate *>   plates;
        std::vector<ButtonObject *>    buttons;
        std::vector<PushableObject *>  pushables;
        std::vector<DoorObject *>      doors;
        std::vector<TeleporterObject *> teleporters;
        std::vector<WallBlockObject *>  wallBlocks;
        std::vector<TorchLightObject *> torches;
        std::vector<FireflyLightObject *> fireflies;
        std::vector<SignObject *>       signs;
        std::vector<DecorObject *>      decors;
        std::vector<CollisionBlock *>   collisionBlocks;
        std::vector<PuzzlePlayer *>    players;
        int runtimeIdCounter;

        // Estado de activadores y meta del nivel.
        std::unordered_set<int> activeActivators;
        bool                    levelComplete = false;

        TeleporterStepCallback   onTeleporterStep;
        LevelCompletedCallback   onLevelCompleted;
        std::unordered_map<int, int> objectPlacementLayers;
        std::unordered_map<int, int> objectDrawLayers;

        void EmitActivatorState(int activatorId, bool active);
        void EvaluatePlate(PressurePlate &plate);
        void EvaluateAllPlatesAround(Cell cell);
        void CheckWinCondition();
        int GetPlacementLayerForObjectId(int objectId) const;
        int GetDrawLayerForObjectId(int objectId) const;
        bool IsPlacementLayerOccupied(Cell cell, int placementLayer, int ignoredObjectId = 0) const;
    };
}

#endif
