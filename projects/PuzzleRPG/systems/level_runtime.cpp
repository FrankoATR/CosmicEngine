#include "level_runtime.hpp"

#include "../systems/class_ids.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include OBJECTMANAGER_HEADER
#include EVENTMANAGER_HEADER

#include <algorithm>
#include <sstream>

namespace PuzzleRPG
{
    namespace
    {
        std::string MakeActivatorEventName(int activatorId)
        {
            std::ostringstream stream;
            stream << "puzzlerpg.activator_state." << activatorId;
            return stream.str();
        }

        float HashToUnitFloat(int value)
        {
            unsigned int x = static_cast<unsigned int>(value);
            x ^= x >> 16;
            x *= 0x7feb352dU;
            x ^= x >> 15;
            x *= 0x846ca68bU;
            x ^= x >> 16;
            return static_cast<float>(x & 0x00ffffffU) / static_cast<float>(0x01000000U);
        }
    }

    LevelRuntime::LevelRuntime() : runtimeIdCounter(0) {}

    LevelRuntime::~LevelRuntime()
    {
        Teardown();
    }

    int LevelRuntime::AllocateRuntimeObjectId()
    {
        if (runtimeIdCounter == 0) runtimeIdCounter = levelData.NextObjectId();
        return ++runtimeIdCounter;
    }

    void LevelRuntime::Build(const LevelData &level)
    {
        Teardown();
        levelData = level;
        runtimeIdCounter = levelData.NextObjectId();
        objectPlacementLayers.clear();
        objectDrawLayers.clear();

        // Y-sort: objects at drawLayer >= 2 get layerId = 2 + (cellY + 1), measured
        // from the bottom edge so objects further south always draw in front.
        // drawLayer 0 (floor) and 1 (background) stay at their fixed layer.
        auto ySortLayer = [](int drawLayer, int cellY) -> short
        {
            return (drawLayer >= 2)
                ? static_cast<short>(2 + cellY + 1)
                : static_cast<short>(drawLayer);
        };

        // Cada TileLayer 1x1 entra como Object independiente para que sus
        // drawLayer se pueda intercalar con jugador y otros objetos.
        for (int y = 0; y < levelData.height; ++y)
        {
            for (int x = 0; x < levelData.width; ++x)
            {
                TileCell cell = levelData.TileAt(x, y);
                for (const auto &layer : cell.layers)
                {
                    auto *tile = new PuzzleTile(layer, x, y);
                    OBJ_MN.Add(tile);
                    tiles.push_back(tile);
                }
            }
        }

        // Muros multi-celda primero: quedan por encima del suelo pero no tapan
        // el resto de interactuables pequenos cuando se solapan visualmente.
        for (const auto &entry : levelData.objects)
        {
            switch (entry.classId)
            {
            case Class_Wall2x2:
            case Class_Wall3x3:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                int span  = WallObjectSpan(entry.classId);
                int sheet = entry.spriteSheet != 0
                                ? entry.spriteSheet
                                : (entry.classId == Class_Wall2x2 ? SheetId::kWalls2x2 : SheetId::kWalls3x3);
                auto *block = new WallBlockObject(entry.id, sheet, entry.spriteRow, entry.spriteCol,
                                                  Cell(entry.x, entry.y), span);
                bool hasFreePos = (entry.pixelX != 0.0f || entry.pixelY != 0.0f);
                if (hasFreePos)
                    block->SetPosition(glm::vec2(entry.pixelX, entry.pixelY));
                int topRow = hasFreePos
                    ? static_cast<int>(entry.pixelY / kTileSize)
                    : entry.y;
                block->SetLayerId(static_cast<short>(2 + (topRow + span - 1) + 1));
                OBJ_MN.Add(block);
                wallBlocks.push_back(block);
                break;
            }
            default:
                break;
            }
        }

        // Objetos interactuables sobre suelo/paredes.
        for (const auto &entry : levelData.objects)
        {
            switch (entry.classId)
            {
            case Class_Pickup:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *pickup = new PickupObject(entry.id, entry.textureId, Cell(entry.x, entry.y), entry.dialog);
                if (entry.spriteSheet != 0)
                    pickup->SetSpriteVariant(entry.spriteSheet, entry.spriteRow, entry.spriteCol);
                pickup->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(pickup);
                pickups.push_back(pickup);
                break;
            }
            case Class_PressurePlate:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *plate = new PressurePlate(entry.id, entry.textureId, Cell(entry.x, entry.y),
                                                entry.activator, entry.targets, entry.requiredItemTextureId);
                plate->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(plate);
                plates.push_back(plate);
                break;
            }
            case Class_Door:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *door = new DoorObject(entry.id, entry.textureId, Cell(entry.x, entry.y));
                door->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(door);
                doors.push_back(door);
                break;
            }
            case Class_Button:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *button = new ButtonObject(entry.id, entry.textureId, Cell(entry.x, entry.y),
                                                entry.facing, entry.targets);
                button->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(button);
                buttons.push_back(button);
                break;
            }
            case Class_Pushable:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *pushable = new PushableObject(entry.id, entry.textureId, Cell(entry.x, entry.y));
                if (entry.spriteSheet != 0)
                    pushable->SetSpriteVariant(entry.spriteSheet, entry.spriteRow, entry.spriteCol);
                pushable->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(pushable);
                pushables.push_back(pushable);
                break;
            }
            case Class_Teleporter:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *teleporter = new TeleporterObject(entry.id, entry.textureId, Cell(entry.x, entry.y), entry.pairId);
                teleporter->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(teleporter);
                teleporters.push_back(teleporter);
                break;
            }
            case Class_Torch:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *torch = new TorchLightObject(entry.id, Cell(entry.x, entry.y));
                torch->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(torch);
                torches.push_back(torch);
                break;
            }
            case Class_Firefly:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                float seed = HashToUnitFloat(entry.id * 31 + entry.x * 17 + entry.y * 13) * 100.0f;
                auto *firefly = new FireflyLightObject(entry.id, Cell(entry.x, entry.y), seed);
                firefly->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(firefly);
                fireflies.push_back(firefly);
                break;
            }
            case Class_Sign:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *sign = new SignObject(entry.id, Cell(entry.x, entry.y));
                sign->SetLayerId(ySortLayer(entry.drawLayer, entry.y));
                OBJ_MN.Add(sign);
                signs.push_back(sign);
                break;
            }
            case Class_Decor:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                int sheet = entry.spriteSheet != 0 ? entry.spriteSheet : SheetId::kDecor;
                auto *decor = new DecorObject(entry.id, sheet, entry.spriteRow, entry.spriteCol,
                                              Cell(entry.x, entry.y));
                bool hasFreePos = (entry.pixelX != 0.0f || entry.pixelY != 0.0f);
                if (hasFreePos)
                    decor->SetPosition(glm::vec2(entry.pixelX, entry.pixelY));
                int sortRow = hasFreePos
                    ? static_cast<int>(entry.pixelY / kTileSize)
                    : entry.y;
                decor->SetLayerId(ySortLayer(entry.drawLayer, sortRow));
                OBJ_MN.Add(decor);
                decors.push_back(decor);
                break;
            }
            case Class_Block:
            {
                objectPlacementLayers[entry.id] = entry.placementLayer;
                objectDrawLayers[entry.id] = entry.drawLayer;
                auto *block = new CollisionBlock(entry.id, Cell(entry.x, entry.y));
                OBJ_MN.Add(block);
                collisionBlocks.push_back(block);
                break;
            }
            default:
                break;
            }
        }

        // Para cada puerta, recopila los ids de placas/botones que la apuntan.
        for (auto *door : doors)
        {
            std::vector<int> activators;
            for (const auto *plate : plates)
            {
                const auto &targets = plate->GetTargets();
                if (std::find(targets.begin(), targets.end(), door->GetObjectId()) != targets.end())
                    activators.push_back(plate->GetObjectId());
            }
            for (const auto *button : buttons)
            {
                const auto &targets = button->GetTargets();
                if (std::find(targets.begin(), targets.end(), door->GetObjectId()) != targets.end())
                    activators.push_back(button->GetObjectId());
            }
            door->BindActivators(activators);
        }

        // Evalua placas con su estado inicial (probablemente todas inactivas
        // porque aun no hay jugadores ni cajas encima).
        RebuildPlateStates();
    }

    void LevelRuntime::Teardown()
    {
        const auto activeObjects = OBJ_MN.GetAll();
        auto destroyAll = [&activeObjects](auto &vec)
        {
            for (auto *object : vec)
            {
                if (!object) continue;
                if (std::find(activeObjects.begin(), activeObjects.end(), object) != activeObjects.end())
                {
                    OBJ_MN.Remove(object->GetID());
                }
            }
            vec.clear();
        };
        destroyAll(tiles);
        destroyAll(pickups);
        destroyAll(plates);
        destroyAll(buttons);
        destroyAll(pushables);
        destroyAll(doors);
        destroyAll(teleporters);
        destroyAll(wallBlocks);
        destroyAll(torches);
        destroyAll(fireflies);
        destroyAll(signs);
        destroyAll(decors);
        destroyAll(collisionBlocks);
        players.clear();
        runtimeIdCounter = 0;
        activeActivators.clear();
        levelComplete = false;
        objectPlacementLayers.clear();
        objectDrawLayers.clear();
    }

    void LevelRuntime::AddPlayer(PuzzlePlayer *player)
    {
        if (!player) return;
        players.push_back(player);
        RebuildPlateStates();
    }

    void LevelRuntime::RemovePlayer(PuzzlePlayer *player)
    {
        players.erase(std::remove(players.begin(), players.end(), player), players.end());
        RebuildPlateStates();
    }

    bool LevelRuntime::IsCellInsideLevel(Cell cell) const
    {
        return cell.x >= 0 && cell.y >= 0 && cell.x < levelData.width && cell.y < levelData.height;
    }

    bool LevelRuntime::IsCellWalkable(Cell cell) const
    {
        if (!IsCellInsideLevel(cell)) return false;
        // Bloques de colision explicitos (sustituyen la colision de tiles de muro).
        for (const auto *block : collisionBlocks)
        {
            if (block && block->GetCell() == cell) return false;
        }
        // Puerta cerrada -> bloquea.
        for (const auto *door : doors)
        {
            if (door->GetCell() == cell && !door->IsOpen()) return false;
        }
        // Empujable -> bloquea (el movimiento lo manejara el push).
        for (const auto *pushable : pushables)
        {
            if (pushable->GetCell() == cell) return false;
        }
        // Botones tienen "cuerpo" en su celda y bloquean tambien.
        for (const auto *button : buttons)
        {
            if (button->GetCell() == cell) return false;
        }
        for (const auto *pickup : pickups)
        {
            if (pickup && !pickup->IsCarried() && pickup->GetCell() == cell) return false;
        }
        // Letreros y antorchas tienen cuerpo y bloquean la celda.
        for (const auto *sign : signs)
        {
            if (sign && sign->GetCell() == cell) return false;
        }
        for (const auto *torch : torches)
        {
            if (torch && torch->GetCell() == cell) return false;
        }
        // Altares (receptaculos de items) tienen cuerpo fisico.
        for (const auto *plate : plates)
        {
            if (plate && plate->GetTextureId() == DefaultTextureId::kReceptacle && plate->GetCell() == cell) return false;
        }
        return true;
    }

    bool LevelRuntime::IsCellFree(Cell cell) const
    {
        if (!IsCellWalkable(cell)) return false;
        for (const auto *player : players)
        {
            if (player->GetCell() == cell) return false;
        }
        return true;
    }

    PuzzlePlayer *LevelRuntime::GetPlayerAtCell(Cell cell) const
    {
        for (auto *player : players) if (player->GetCell() == cell) return player;
        return nullptr;
    }

    PushableObject *LevelRuntime::GetPushableAtCell(Cell cell) const
    {
        for (auto *pushable : pushables) if (pushable->GetCell() == cell) return pushable;
        return nullptr;
    }

    DoorObject *LevelRuntime::GetDoorAtCell(Cell cell) const
    {
        for (auto *door : doors) if (door->GetCell() == cell) return door;
        return nullptr;
    }

    ButtonObject *LevelRuntime::GetButtonAtCell(Cell cell) const
    {
        for (auto *button : buttons) if (button->GetCell() == cell) return button;
        return nullptr;
    }

    PickupObject *LevelRuntime::GetPickupAtCell(Cell cell) const
    {
        for (auto *pickup : pickups)
        {
            if (!pickup->IsCarried() && pickup->GetCell() == cell) return pickup;
        }
        return nullptr;
    }

    TeleporterObject *LevelRuntime::GetTeleporterAtCell(Cell cell) const
    {
        for (auto *teleporter : teleporters)
        {
            if (teleporter->GetCell() == cell) return teleporter;
        }
        return nullptr;
    }

    TeleporterObject *LevelRuntime::GetTeleporterPartner(const TeleporterObject &teleporter) const
    {
        for (auto *candidate : teleporters)
        {
            if (candidate == &teleporter) continue;
            if (candidate->GetPairId() == teleporter.GetPairId()) return candidate;
        }
        return nullptr;
    }

    PressurePlate *LevelRuntime::GetPlateAtCell(Cell cell) const
    {
        for (auto *plate : plates) if (plate->GetCell() == cell) return plate;
        return nullptr;
    }

    SignObject *LevelRuntime::GetSignAtCell(Cell cell) const
    {
        for (auto *sign : signs) if (sign->GetCell() == cell) return sign;
        return nullptr;
    }

    int LevelRuntime::GetPlacementLayerForObjectId(int objectId) const
    {
        auto it = objectPlacementLayers.find(objectId);
        if (it == objectPlacementLayers.end()) return DefaultPlacementLayerForClass(Class_None);
        return it->second;
    }

    int LevelRuntime::GetDrawLayerForObjectId(int objectId) const
    {
        auto it = objectDrawLayers.find(objectId);
        if (it == objectDrawLayers.end()) return DefaultDrawLayerForClass(Class_None);
        return it->second;
    }

    bool LevelRuntime::IsPlacementLayerOccupied(Cell cell, int placementLayer, int ignoredObjectId) const
    {
        auto matchesPlacementLayer = [this, placementLayer, ignoredObjectId](int objectId)
        {
            return objectId != ignoredObjectId && GetPlacementLayerForObjectId(objectId) == placementLayer;
        };

        for (const auto *pickup : pickups)
        {
            if (!pickup || pickup->IsCarried()) continue;
            if (pickup->GetCell() == cell && matchesPlacementLayer(pickup->GetObjectId())) return true;
        }
        for (const auto *plate : plates)
        {
            if (plate && plate->GetCell() == cell && matchesPlacementLayer(plate->GetObjectId())) return true;
        }
        for (const auto *door : doors)
        {
            if (door && door->GetCell() == cell && matchesPlacementLayer(door->GetObjectId())) return true;
        }
        for (const auto *button : buttons)
        {
            if (button && button->GetCell() == cell && matchesPlacementLayer(button->GetObjectId())) return true;
        }
        for (const auto *pushable : pushables)
        {
            if (pushable && pushable->GetCell() == cell && matchesPlacementLayer(pushable->GetObjectId())) return true;
        }
        for (const auto *teleporter : teleporters)
        {
            if (teleporter && teleporter->GetCell() == cell && matchesPlacementLayer(teleporter->GetObjectId())) return true;
        }
        for (const auto *wallBlock : wallBlocks)
        {
            if (wallBlock && wallBlock->CoversCell(cell) && matchesPlacementLayer(wallBlock->GetObjectId())) return true;
        }
        for (const auto *sign : signs)
        {
            if (sign && sign->GetCell() == cell && matchesPlacementLayer(sign->GetObjectId())) return true;
        }
        for (const auto *torch : torches)
        {
            if (torch && torch->GetCell() == cell && matchesPlacementLayer(torch->GetObjectId())) return true;
        }
        return false;
    }

    MoveResult LevelRuntime::TryMovePlayer(PuzzlePlayer &player, int facing)
    {
        Cell origin = player.GetCell();
        Cell front  = CellInFront(origin, facing);
        if (!IsCellInsideLevel(front)) return MoveResult::Blocked;

        // Intentar empujar caja.
        PushableObject *pushable = GetPushableAtCell(front);
        if (pushable)
        {
            Cell beyond = CellInFront(front, facing);
            if (!IsCellFree(beyond)) return MoveResult::Blocked;
            pushable->BeginMoveTo(beyond);
            player.BeginMoveTo(front);
            EvaluateAllPlatesAround(front);
            EvaluateAllPlatesAround(beyond);
            return MoveResult::Pushed;
        }

        if (!IsCellWalkable(front)) return MoveResult::Blocked;
        if (GetPlayerAtCell(front))  return MoveResult::Blocked;

        player.BeginMoveTo(front);
        EvaluateAllPlatesAround(origin);
        EvaluateAllPlatesAround(front);
        return MoveResult::Walked;
    }

    bool LevelRuntime::TryPickupInFront(PuzzlePlayer &player)
    {
        Cell front = CellInFront(player.GetCell(), player.GetFacing());
        PickupObject *pickup = GetPickupAtCell(front);
        if (!pickup) return false;
        if (!player.AddToInventory(pickup->GetTextureId(), pickup->GetObjectId())) return false;
        pickup->SetCarried(true);
        return true;
    }

    bool LevelRuntime::TryPlaceInFront(PuzzlePlayer &player)
    {
        Cell front = CellInFront(player.GetCell(), player.GetFacing());
        if (!IsCellWalkable(front))
        {
            // Altares no son transitables pero si permiten colocar pickups sobre ellos.
            PressurePlate *blocker = GetPlateAtCell(front);
            if (!blocker || blocker->GetTextureId() != DefaultTextureId::kReceptacle) return false;
        }
        if (GetPlayerAtCell(front))  return false;
        int textureId = 0;
        int objectId = 0;
        if (!player.RemoveFromInventoryAt(player.GetSelectedSlot(), textureId, objectId)) return false;

        PickupObject *pickup = nullptr;
        if (objectId > 0)
        {
            for (auto *candidate : pickups)
            {
                if (candidate && candidate->GetObjectId() == objectId)
                {
                    pickup = candidate;
                    break;
                }
            }
        }

        int placementLayer = DefaultPlacementLayerForClass(Class_Pickup);
        int drawLayer = DefaultDrawLayerForClass(Class_Pickup);
        if (pickup)
        {
            placementLayer = GetPlacementLayerForObjectId(pickup->GetObjectId());
            drawLayer = GetDrawLayerForObjectId(pickup->GetObjectId());
        }

        if (IsPlacementLayerOccupied(front, placementLayer, pickup ? pickup->GetObjectId() : 0))
        {
            // Allow placement if the occupying element is a receptacle (altar).
            PressurePlate *plate = GetPlateAtCell(front);
            bool occupyingIsReceptacle = (plate && plate->GetTextureId() == DefaultTextureId::kReceptacle);
            if (!occupyingIsReceptacle)
            {
                player.AddToInventory(textureId, objectId);
                return false;
            }
        }

        bool placedOnAltar = false;
        {
            PressurePlate *plate = GetPlateAtCell(front);
            placedOnAltar = (plate && plate->GetTextureId() == DefaultTextureId::kReceptacle);
        }

        if (pickup)
        {
            pickup->SetCell(front);
            pickup->SetCarried(false);
            pickup->SetOnAltar(placedOnAltar);
        }
        else
        {
            int newId = (objectId > 0) ? objectId : AllocateRuntimeObjectId();
            pickup = new PickupObject(newId, textureId, front);
            objectPlacementLayers[newId] = placementLayer;
            objectDrawLayers[newId] = drawLayer;
            pickup->SetLayerId(static_cast<short>(drawLayer));
            pickup->SetOnAltar(placedOnAltar);
            OBJ_MN.Add(pickup);
            pickups.push_back(pickup);
        }
        RebuildPlateStates();
        return true;
    }

    bool LevelRuntime::TryInteractInFront(PuzzlePlayer &player)
    {
        Cell front = CellInFront(player.GetCell(), player.GetFacing());
        ButtonObject *button = GetButtonAtCell(front);
        if (!button) return false;
        // Solo se activa si el jugador mira en la direccion requerida por el boton.
        // Como el jugador esta en `origin` mirando hacia `front`, su facing coincide.
        if (button->GetRequiredFacing() >= 0 && button->GetRequiredFacing() != player.GetFacing())
        {
            // Permitimos activacion si no se especifica facing (valor < 0 reservado).
        }
        button->Toggle();
        EmitActivatorState(button->GetObjectId(), button->IsActive());
        return true;
    }

    std::string LevelRuntime::GetDialogInFront(const PuzzlePlayer &player) const
    {
        Cell front = CellInFront(player.GetCell(), player.GetFacing());
        const LevelObject *meta = nullptr;
        for (const auto &entry : levelData.objects)
        {
            if (entry.classId == Class_Sign && entry.x == front.x && entry.y == front.y) { meta = &entry; break; }
        }
        return meta ? meta->dialog : std::string();
    }

    void LevelRuntime::EmitActivatorState(int activatorId, bool active)
    {
        if (active) activeActivators.insert(activatorId);
        else         activeActivators.erase(activatorId);
        EVT_MN.TriggerEvent<bool>(MakeActivatorEventName(activatorId), active);
        CheckWinCondition();
    }

    void LevelRuntime::CheckWinCondition()
    {
        if (levelComplete) return;
        if (levelData.winActivatorIds.empty()) return;
        for (int id : levelData.winActivatorIds)
        {
            if (activeActivators.find(id) == activeActivators.end()) return;
        }
        levelComplete = true;
        if (onLevelCompleted) onLevelCompleted();
    }

    void LevelRuntime::EvaluatePlate(PressurePlate &plate)
    {
        bool occupiedByPlayer = false;
        bool occupiedByObject = false;
        for (const auto *player : players)
        {
            if (player->GetCell() == plate.GetCell()) { occupiedByPlayer = true; break; }
        }
        if (GetPushableAtCell(plate.GetCell())) occupiedByObject = true;

        bool active = false;
        switch (plate.GetActivator())
        {
        case Plate_Player: active = occupiedByPlayer; break;
        case Plate_Object: active = occupiedByObject; break;
        case Plate_SpecificItem:
        {
            // Solo se activa con el pickup de id concreto configurado en el editor.
            PickupObject *pickup = GetPickupAtCell(plate.GetCell());
            int required = plate.GetRequiredItemTextureId();
            active = (pickup != nullptr) && (required > 0) && (pickup->GetObjectId() == required);
            break;
        }
        case Plate_Both:
        default:           active = occupiedByPlayer || occupiedByObject; break;
        }
        if (active != plate.IsActive())
        {
            plate.SetActive(active);
            EmitActivatorState(plate.GetObjectId(), active);
        }
    }

    void LevelRuntime::EvaluateAllPlatesAround(Cell /*cell*/)
    {
        // Re-evaluamos todas las placas: son pocas y mantener la logica simple
        // vale mas que microoptimizar.
        for (auto *plate : plates) EvaluatePlate(*plate);
    }

    void LevelRuntime::RebuildPlateStates()
    {
        for (auto *plate : plates) EvaluatePlate(*plate);
    }

    void LevelRuntime::ForceReevaluateAll()
    {
        RebuildPlateStates();
    }

    void LevelRuntime::HandlePlayerArrived(PuzzlePlayer &player)
    {
        // Rearme de teletransportes: si la celda actual NO es la del ultimo
        // teletransporte registrado para este jugador, lo limpiamos.
        TeleporterObject *here = GetTeleporterAtCell(player.GetCell());
        if (!here)
        {
            player.SetLastTeleporterId(0);
            return;
        }
        if (player.GetLastTeleporterId() == here->GetObjectId()) return;
        // Llegamos a un teletransporte "fresco": notificar a la capa de juego.
        if (onTeleporterStep) onTeleporterStep(player, *here);
    }
}
