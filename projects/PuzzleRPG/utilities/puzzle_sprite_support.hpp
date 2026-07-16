#ifndef PUZZLERPG_SPRITE_SUPPORT_HPP
#define PUZZLERPG_SPRITE_SUPPORT_HPP

#include "puzzle_asset_defines.hpp"

#include "../systems/class_ids.hpp"

#include <CosmicEngine/managers/animation/animation_manager.hpp>
#include <CosmicEngine/managers/resource/resource_manager.hpp>
#include <CosmicEngine/models/animation/animation_clip.hpp>
#include <CosmicEngine/models/animation/animation_player.hpp>
#include <CosmicEngine/models/shader/shader.hpp>

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace PuzzleRPG
{
    inline bool &PuzzleGameplayLightingEnabledFlag()
    {
        static bool enabled = false;
        return enabled;
    }

    inline bool IsPuzzleGameplayLightingEnabled()
    {
        return PuzzleGameplayLightingEnabledFlag();
    }

    inline void SetPuzzleGameplayLightingEnabled(bool enabled)
    {
        PuzzleGameplayLightingEnabledFlag() = enabled;
    }

    namespace detail
    {
        inline std::unordered_set<std::string> &LoadedSheetKeys()
        {
            static std::unordered_set<std::string> instance;
            return instance;
        }

        // True una vez que RegisterPuzzleSpriteAssets() ya corrio completo
        // en la escena actual. Sin este flag, DrawSheetCell golpea el disco
        // (std::filesystem::exists) miles de veces por frame al iterar
        // tiles y objetos, lo cual tira los FPS.
        inline bool &SpritesRegisteredFlag()
        {
            static bool registered = false;
            return registered;
        }

        inline bool RegisterSheetIfPresent(const char *key, const char *path, int rows, int columns, int padding)
        {
                std::string p(path ? path : "");
                std::string resolvedPath = p;

                // Resolve strictly from the runtime working directory. This keeps
                // PuzzleRPG loading assets from the copied `assets/` folder that
                // lives next to the executable, instead of falling back to the
                // repository source tree.
                try
                {
                    std::filesystem::path candidate = std::filesystem::current_path() / p;
                    if (std::filesystem::exists(candidate))
                    {
                        resolvedPath = candidate.lexically_normal().string();
                    }
                }
                catch (...) { }

                bool ok = CosmicEngine::ResourceManager::GetInstance().LoadTextureSheet(key, resolvedPath.c_str(), rows, columns, padding);
            if (ok || detail::LoadedSheetKeys().find(key) != detail::LoadedSheetKeys().end())
            {
                LoadedSheetKeys().insert(key);
                return true;
            }
            return false;
        }

        inline void RegisterClipIfMissing(const char *clipName,
                                          const char *sheetKey,
                                          std::initializer_list<CosmicEngine::AnimationFrame> frames,
                                          bool loop = true)
        {
            if (CosmicEngine::AnimationManager::GetInstance().GetClip(clipName)) return;
            CosmicEngine::AnimationManager::GetInstance().RegisterClip(CosmicEngine::AnimationClip(
                clipName,
                sheetKey,
                std::vector<CosmicEngine::AnimationFrame>(frames.begin(), frames.end()),
                loop));
        }
    }

    inline void RegisterPuzzleSpriteAssets()
    {
        if (detail::SpritesRegisteredFlag()) return;

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_TILE_FLOOR_KEY, PUZZLERPG_SHEET_TILE_FLOOR_PATH,
                                           PUZZLERPG_SHEET_TILE_FLOOR_ROWS, PUZZLERPG_SHEET_TILE_FLOOR_COLS,
                                           PUZZLERPG_SHEET_TILE_FLOOR_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_TILE_FLOOR, PUZZLERPG_SHEET_TILE_FLOOR_KEY,
                                          { {0, 0, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_TILE_WALL_KEY, PUZZLERPG_SHEET_TILE_WALL_PATH,
                                           PUZZLERPG_SHEET_TILE_WALL_ROWS, PUZZLERPG_SHEET_TILE_WALL_COLS,
                                           PUZZLERPG_SHEET_TILE_WALL_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_TILE_WALL, PUZZLERPG_SHEET_TILE_WALL_KEY,
                                          { {0, 0, 1.0} }, true);
        }

        // Walls 2x2 y 3x3: solo cargamos la hoja, no registramos clip (se usan
        // como objetos multi-celda con DrawSheetCell directo).
        detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_TILE_WALL_2X2_KEY, PUZZLERPG_SHEET_TILE_WALL_2X2_PATH,
                                       PUZZLERPG_SHEET_TILE_WALL_2X2_ROWS, PUZZLERPG_SHEET_TILE_WALL_2X2_COLS,
                                       PUZZLERPG_SHEET_TILE_WALL_2X2_PAD);
        detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_TILE_WALL_3X3_KEY, PUZZLERPG_SHEET_TILE_WALL_3X3_PATH,
                                       PUZZLERPG_SHEET_TILE_WALL_3X3_ROWS, PUZZLERPG_SHEET_TILE_WALL_3X3_COLS,
                                       PUZZLERPG_SHEET_TILE_WALL_3X3_PAD);

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_PLAYER_KEY, PUZZLERPG_SHEET_PLAYER_PATH,
                                           PUZZLERPG_SHEET_PLAYER_ROWS, PUZZLERPG_SHEET_PLAYER_COLS,
                                           PUZZLERPG_SHEET_PLAYER_PAD))
        {
            // New player sheet layout: 4 rows x 8 columns.
            // Row 0 = up, Row 1 = right, Row 2 = down, Row 3 = left.
            // Idle uses column 0 with a longer duration; walking uses columns 0..7 with 0.05s per frame.
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_UP_IDLE,    PUZZLERPG_SHEET_PLAYER_KEY, { {0, 0, 0.2} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_RIGHT_IDLE, PUZZLERPG_SHEET_PLAYER_KEY, { {1, 0, 0.2} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_DOWN_IDLE,  PUZZLERPG_SHEET_PLAYER_KEY, { {2, 0, 0.2} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_LEFT_IDLE,  PUZZLERPG_SHEET_PLAYER_KEY, { {3, 0, 0.2} }, true);

            // Walking animations: 8 frames, 0.05s each
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_UP_WALK,    PUZZLERPG_SHEET_PLAYER_KEY,
                                          { {0,0,0.05},{0,1,0.05},{0,2,0.05},{0,3,0.05},{0,4,0.05},{0,5,0.05},{0,6,0.05},{0,7,0.05} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_RIGHT_WALK, PUZZLERPG_SHEET_PLAYER_KEY,
                                          { {1,0,0.05},{1,1,0.05},{1,2,0.05},{1,3,0.05},{1,4,0.05},{1,5,0.05},{1,6,0.05},{1,7,0.05} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_DOWN_WALK,  PUZZLERPG_SHEET_PLAYER_KEY,
                                          { {2,0,0.05},{2,1,0.05},{2,2,0.05},{2,3,0.05},{2,4,0.05},{2,5,0.05},{2,6,0.05},{2,7,0.05} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLAYER_LEFT_WALK,  PUZZLERPG_SHEET_PLAYER_KEY,
                                          { {3,0,0.05},{3,1,0.05},{3,2,0.05},{3,3,0.05},{3,4,0.05},{3,5,0.05},{3,6,0.05},{3,7,0.05} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_PICKUP_KEY, PUZZLERPG_SHEET_PICKUP_PATH,
                                           PUZZLERPG_SHEET_PICKUP_ROWS, PUZZLERPG_SHEET_PICKUP_COLS,
                                           PUZZLERPG_SHEET_PICKUP_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PICKUP_IDLE, PUZZLERPG_SHEET_PICKUP_KEY,
                                          { {0, 0, 0.12}, {0, 1, 0.12}, {0, 2, 0.12}, {0, 3, 0.12} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_PLATE_KEY, PUZZLERPG_SHEET_PLATE_PATH,
                                           PUZZLERPG_SHEET_PLATE_ROWS, PUZZLERPG_SHEET_PLATE_COLS,
                                           PUZZLERPG_SHEET_PLATE_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLATE_OFF, PUZZLERPG_SHEET_PLATE_KEY, { {0, 0, 1.0} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PLATE_ON,  PUZZLERPG_SHEET_PLATE_KEY, { {0, 1, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_RECEPTACLE_KEY, PUZZLERPG_SHEET_RECEPTACLE_PATH,
                                           PUZZLERPG_SHEET_RECEPTACLE_ROWS, PUZZLERPG_SHEET_RECEPTACLE_COLS,
                                           PUZZLERPG_SHEET_RECEPTACLE_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_RECEPTACLE_OFF, PUZZLERPG_SHEET_RECEPTACLE_KEY, { {0, 0, 1.0} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_RECEPTACLE_ON,  PUZZLERPG_SHEET_RECEPTACLE_KEY, { {0, 1, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_DOOR_KEY, PUZZLERPG_SHEET_DOOR_PATH,
                                           PUZZLERPG_SHEET_DOOR_ROWS, PUZZLERPG_SHEET_DOOR_COLS,
                                           PUZZLERPG_SHEET_DOOR_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_DOOR_CLOSED, PUZZLERPG_SHEET_DOOR_KEY, { {0, 0, 1.0} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_DOOR_OPEN,   PUZZLERPG_SHEET_DOOR_KEY, { {0, 1, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_BUTTON_KEY, PUZZLERPG_SHEET_BUTTON_PATH,
                                           PUZZLERPG_SHEET_BUTTON_ROWS, PUZZLERPG_SHEET_BUTTON_COLS,
                                           PUZZLERPG_SHEET_BUTTON_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_BUTTON_OFF, PUZZLERPG_SHEET_BUTTON_KEY, { {0, 0, 1.0} }, true);
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_BUTTON_ON,  PUZZLERPG_SHEET_BUTTON_KEY, { {0, 1, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_PUSHABLE_KEY, PUZZLERPG_SHEET_PUSHABLE_PATH,
                                           PUZZLERPG_SHEET_PUSHABLE_ROWS, PUZZLERPG_SHEET_PUSHABLE_COLS,
                                           PUZZLERPG_SHEET_PUSHABLE_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_PUSHABLE_IDLE, PUZZLERPG_SHEET_PUSHABLE_KEY, { {0, 0, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_TELEPORTER_KEY, PUZZLERPG_SHEET_TELEPORTER_PATH,
                                           PUZZLERPG_SHEET_TELEPORTER_ROWS, PUZZLERPG_SHEET_TELEPORTER_COLS,
                                           PUZZLERPG_SHEET_TELEPORTER_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_TELEPORTER_IDLE, PUZZLERPG_SHEET_TELEPORTER_KEY,
                                          { {0, 0, 0.10}, {0, 1, 0.10}, {0, 2, 0.10}, {0, 3, 0.10} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_SIGN_KEY, PUZZLERPG_SHEET_SIGN_PATH,
                                           PUZZLERPG_SHEET_SIGN_ROWS, PUZZLERPG_SHEET_SIGN_COLS,
                                           PUZZLERPG_SHEET_SIGN_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_SIGN_IDLE, PUZZLERPG_SHEET_SIGN_KEY, { {0, 0, 1.0} }, true);
        }

        if (detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_TORCH_KEY, PUZZLERPG_SHEET_TORCH_PATH,
                                           PUZZLERPG_SHEET_TORCH_ROWS, PUZZLERPG_SHEET_TORCH_COLS,
                                           PUZZLERPG_SHEET_TORCH_PAD))
        {
            detail::RegisterClipIfMissing(PUZZLERPG_CLIP_TORCH_IDLE, PUZZLERPG_SHEET_TORCH_KEY,
                                          { {0, 0, 0.25}, {0, 1, 0.25}, {0, 2, 0.25}, {0, 3, 0.25} }, true);
        }

        // Decoraciones: hoja visual sin colision ni logica.
        detail::RegisterSheetIfPresent(PUZZLERPG_SHEET_DECOR_KEY, PUZZLERPG_SHEET_DECOR_PATH,
                                       PUZZLERPG_SHEET_DECOR_ROWS, PUZZLERPG_SHEET_DECOR_COLS,
                                       PUZZLERPG_SHEET_DECOR_PAD);

        detail::SpritesRegisteredFlag() = true;
    }

    inline void ResetPuzzleSpriteAssetCache()
    {
        detail::LoadedSheetKeys().clear();
        detail::SpritesRegisteredFlag() = false;
    }

    inline const char *ClipForTextureId(int textureId)
    {
        switch (textureId)
        {
            case DefaultTextureId::kFloor:         return PUZZLERPG_CLIP_TILE_FLOOR;
            case DefaultTextureId::kWall:          return PUZZLERPG_CLIP_TILE_WALL;
            case DefaultTextureId::kPickup:        return PUZZLERPG_CLIP_PICKUP_IDLE;
            case DefaultTextureId::kPressurePlate: return PUZZLERPG_CLIP_PLATE_OFF;
            case DefaultTextureId::kDoorClosed:    return PUZZLERPG_CLIP_DOOR_CLOSED;
            case DefaultTextureId::kDoorOpen:      return PUZZLERPG_CLIP_DOOR_OPEN;
            case DefaultTextureId::kButtonOff:     return PUZZLERPG_CLIP_BUTTON_OFF;
            case DefaultTextureId::kButtonOn:      return PUZZLERPG_CLIP_BUTTON_ON;
            case DefaultTextureId::kPushable:      return PUZZLERPG_CLIP_PUSHABLE_IDLE;
            case DefaultTextureId::kTeleporter:    return PUZZLERPG_CLIP_TELEPORTER_IDLE;
            case DefaultTextureId::kReceptacle:    return PUZZLERPG_CLIP_RECEPTACLE_OFF;
            default:                               return nullptr;
        }
    }

    // Devuelve la key de spritesheet asociada a un SheetId (TileCell.sheet).
    // Si el id no esta mapeado o vale kNone, devuelve nullptr.
    inline const char *SheetKeyForSheetId(int sheetId)
    {
        switch (sheetId)
        {
            case SheetId::kGround:   return PUZZLERPG_SHEET_TILE_FLOOR_KEY;
            case SheetId::kWalls1x1: return PUZZLERPG_SHEET_TILE_WALL_KEY;
            case SheetId::kWalls2x2: return PUZZLERPG_SHEET_TILE_WALL_2X2_KEY;
            case SheetId::kWalls3x3: return PUZZLERPG_SHEET_TILE_WALL_3X3_KEY;
            case SheetId::kPushable: return PUZZLERPG_SHEET_PUSHABLE_KEY;
            case SheetId::kPickable: return PUZZLERPG_SHEET_PICKUP_KEY;
            case SheetId::kPressure: return PUZZLERPG_SHEET_PLATE_KEY;
            case SheetId::kAltar:    return PUZZLERPG_SHEET_RECEPTACLE_KEY;
            case SheetId::kPlayer:   return PUZZLERPG_SHEET_PLAYER_KEY;
            case SheetId::kDecor:    return PUZZLERPG_SHEET_DECOR_KEY;
            default:                 return nullptr;
        }
    }

    // Dibuja directamente la sub-celda (row, col) de un spritesheet, sin
    // necesidad de registrar un AnimationClip para esa celda. Devuelve true
    // si la hoja esta cargada (asset presente). Si no, no dibuja nada y
    // devuelve false: el llamante puede pintar su propio placeholder.
    inline bool DrawSheetCell(const char *sheetKey,
                              int row,
                              int col,
                              glm::vec2 position,
                              glm::vec2 size,
                              float rotation = 0.0f,
                              glm::vec3 color = glm::vec3(1.0f),
                              float alpha = 1.0f,
                              CosmicEngine::ViewType viewType = CosmicEngine::ViewType::Ortho)
    {
        RegisterPuzzleSpriteAssets();
        if (!sheetKey || !sheetKey[0]) return false;
        if (detail::LoadedSheetKeys().find(sheetKey) == detail::LoadedSheetKeys().end()) return false;
        if (viewType != CosmicEngine::ViewType::UI && IsPuzzleGameplayLightingEnabled())
        {
            CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetLit(
                sheetKey, row, col, position, size, rotation, color, alpha, viewType);
        }
        else
        {
            CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetUnlit(
                sheetKey, row, col, position, size, rotation, color, alpha, viewType);
        }
        return true;
    }

    // --- Toggle global de "primitivas/bodies de debug" ---------------------
    // Activa/desactiva el pintado de rectangulos placeholder y etiquetas de
    // texto encima de tiles y objetos del PuzzleRPG. Los sprites reales se
    // siguen dibujando siempre.
    inline bool &PuzzleDebugVisualsFlag()
    {
        static bool enabled = true;
        return enabled;
    }
    inline bool IsPuzzleDebugVisualsEnabled()        { return PuzzleDebugVisualsFlag(); }
    inline void SetPuzzleDebugVisualsEnabled(bool v) { PuzzleDebugVisualsFlag() = v; }
    inline void TogglePuzzleDebugVisuals()           { PuzzleDebugVisualsFlag() = !PuzzleDebugVisualsFlag(); }

    inline const char *ClipForDoor(bool open, int fallbackTextureId)
    {
        if (open) return PUZZLERPG_CLIP_DOOR_OPEN;
        const char *fallback = ClipForTextureId(fallbackTextureId);
        return fallback ? fallback : PUZZLERPG_CLIP_DOOR_CLOSED;
    }

    inline const char *ClipForButton(bool active, int fallbackTextureId)
    {
        if (active) return PUZZLERPG_CLIP_BUTTON_ON;
        const char *fallback = ClipForTextureId(fallbackTextureId);
        return fallback ? fallback : PUZZLERPG_CLIP_BUTTON_OFF;
    }

    inline const char *ClipForPressurePlate(bool active, int fallbackTextureId)
    {
        if (fallbackTextureId == DefaultTextureId::kReceptacle)
            return active ? PUZZLERPG_CLIP_RECEPTACLE_ON : PUZZLERPG_CLIP_RECEPTACLE_OFF;
        if (active) return PUZZLERPG_CLIP_PLATE_ON;
        const char *fallback = ClipForTextureId(fallbackTextureId);
        return fallback ? fallback : PUZZLERPG_CLIP_PLATE_OFF;
    }

    inline const char *ClipForPlayer(int facing, bool moving)
    {
        switch (facing)
        {
            case 1: return moving ? PUZZLERPG_CLIP_PLAYER_UP_WALK    : PUZZLERPG_CLIP_PLAYER_UP_IDLE;
            case 2: return moving ? PUZZLERPG_CLIP_PLAYER_LEFT_WALK  : PUZZLERPG_CLIP_PLAYER_LEFT_IDLE;
            case 3: return moving ? PUZZLERPG_CLIP_PLAYER_RIGHT_WALK : PUZZLERPG_CLIP_PLAYER_RIGHT_IDLE;
            case 0:
            default:
                return moving ? PUZZLERPG_CLIP_PLAYER_DOWN_WALK : PUZZLERPG_CLIP_PLAYER_DOWN_IDLE;
        }
    }

    inline bool DrawClipFirstFrame(const char *clipName,
                                   glm::vec2 position,
                                   glm::vec2 size,
                                   float rotation = 0.0f,
                                   glm::vec3 color = glm::vec3(1.0f),
                                   float alpha = 1.0f,
                                   CosmicEngine::ViewType viewType = CosmicEngine::ViewType::Ortho)
    {
        RegisterPuzzleSpriteAssets();
        if (!clipName || !clipName[0]) return false;

        const auto *clip = CosmicEngine::AnimationManager::GetInstance().GetClip(clipName);
        if (!clip) return false;
        const auto *frame = clip->GetFrame(0);
        if (!frame) return false;

        if (viewType != CosmicEngine::ViewType::UI && IsPuzzleGameplayLightingEnabled())
        {
            CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetLit(
                clip->GetTextureSheetKey(),
                frame->row,
                frame->column,
                position,
                size,
                rotation,
                color,
                alpha,
                viewType);
        }
        else
        {
            CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetUnlit(
                clip->GetTextureSheetKey(),
                frame->row,
                frame->column,
                position,
                size,
                rotation,
                color,
                alpha,
                viewType);
        }
        return true;
    }

    class SpriteSheetVisual
    {
    public:
        SpriteSheetVisual() = default;
        ~SpriteSheetVisual() { Reset(); }

        SpriteSheetVisual(const SpriteSheetVisual &) = delete;
        SpriteSheetVisual &operator=(const SpriteSheetVisual &) = delete;

        void Reset()
        {
            if (player)
            {
                player->Destroy();
                player = nullptr;
            }
            staticClipName.clear();
        }

        void SetStaticClip(const char *clipName)
        {
            RegisterPuzzleSpriteAssets();
            if (player)
            {
                player->Destroy();
                player = nullptr;
            }
            staticClipName = clipName ? clipName : "";
        }

        void SetAnimatedClip(const char *clipName, std::int64_t ownerObjectId, bool restartIfChanged = false)
        {
            RegisterPuzzleSpriteAssets();
            if (!clipName || !clipName[0])
            {
                Reset();
                return;
            }

            const auto *clip = CosmicEngine::AnimationManager::GetInstance().GetClip(clipName);
            if (!clip) return;

            staticClipName.clear();
            if (!player)
            {
                player = CosmicEngine::AnimationManager::GetInstance().CreatePlayer(clipName, ownerObjectId);
                if (player) player->Play();
                return;
            }

            player->SetOwnerObjectId(ownerObjectId);
            if (player->GetClipName() != clipName)
            {
                // Always restart when switching to a different clip so frame
                // indices from a longer previous clip do not leak into the new one.
                player->SetClip(clip, true);
            }
            if (!player->IsPlaying()) player->Play();
        }

        bool Draw(glm::vec2 position,
                  glm::vec2 size,
                  float rotation = 0.0f,
                  glm::vec3 color = glm::vec3(1.0f),
                  float alpha = 1.0f,
                  CosmicEngine::ViewType viewType = CosmicEngine::ViewType::Ortho) const
        {
            if (player && !player->GetTextureSheetKey().empty())
            {
                // Verify that the texture sheet resource is actually loaded
                // before attempting to render. If the sheet is missing, fall
                // through so callers can draw a debug placeholder instead.
                const std::string &sheetKey = player->GetTextureSheetKey();
                if (detail::LoadedSheetKeys().find(sheetKey) != detail::LoadedSheetKeys().end())
                {
                    if (viewType != CosmicEngine::ViewType::UI && IsPuzzleGameplayLightingEnabled())
                    {
                        CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetLit(
                            sheetKey,
                            player->GetCurrentRow(),
                            player->GetCurrentColumn(),
                            position,
                            size,
                            rotation,
                            color,
                            alpha,
                            viewType);
                    }
                    else
                    {
                        CosmicEngine::ResourceManager::GetInstance().Render2DSpriteFromTextureSheetUnlit(
                            sheetKey,
                            player->GetCurrentRow(),
                            player->GetCurrentColumn(),
                            position,
                            size,
                            rotation,
                            color,
                            alpha,
                            viewType);
                    }
                    return true;
                }
            }

            if (!staticClipName.empty())
            {
                return DrawClipFirstFrame(staticClipName.c_str(), position, size, rotation, color, alpha, viewType);
            }
            return false;
        }

    private:
        CosmicEngine::AnimationPlayer *player = nullptr;
        std::string staticClipName;
    };
}

#endif