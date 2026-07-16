#include "door_object.hpp"

#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER
#include EVENTMANAGER_HEADER

#include <sstream>

namespace PuzzleRPG
{
    static std::string MakeActivatorEventName(int activatorId)
    {
        std::ostringstream stream;
        stream << "puzzlerpg.activator_state." << activatorId;
        return stream.str();
    }

    DoorObject::DoorObject(int id, int textureId, Cell cell)
        : CosmicEngine::Object("Door",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               2),
          objectId(id),
          textureId(textureId),
          cell(cell),
          open(false)
    {
        SetColor(glm::vec3(0.55f, 0.35f, 0.20f));
        spriteVisual.SetStaticClip(ClipForDoor(open, textureId));
    }

    DoorObject::~DoorObject()
    {
        for (std::size_t i = 0; i < listenerIds.size(); ++i)
        {
            EVT_MN.UnregisterListener<bool>(listenerEventNames[i], listenerIds[i]);
        }
        listenerIds.clear();
        listenerEventNames.clear();
    }

    void DoorObject::BindActivators(const std::vector<int> &ids)
    {
        activatorIds      = ids;
        activatorStates.assign(ids.size(), false);

        for (std::size_t i = 0; i < ids.size(); ++i)
        {
            int activatorId = ids[i];
            std::size_t slot = i;
            std::string eventName = MakeActivatorEventName(activatorId);
            auto listenerId = EVT_MN.RegisterEventListener<bool>(
                eventName,
                std::function<void(bool)>([this, slot](bool state)
                {
                    if (slot < activatorStates.size())
                    {
                        activatorStates[slot] = state;
                        RecomputeOpenState();
                    }
                }));
            listenerIds.push_back(listenerId);
            listenerEventNames.push_back(std::move(eventName));
        }
        RecomputeOpenState();
    }

    void DoorObject::OnActivatorChanged(int activatorId, bool active)
    {
        for (std::size_t i = 0; i < activatorIds.size(); ++i)
        {
            if (activatorIds[i] == activatorId)
            {
                activatorStates[i] = active;
            }
        }
        RecomputeOpenState();
    }

    void DoorObject::SetOpen(bool value)
    {
        open = value;
        spriteVisual.SetStaticClip(ClipForDoor(open, textureId));
    }

    void DoorObject::RecomputeOpenState()
    {
        bool allActive = !activatorStates.empty();
        for (bool state : activatorStates)
        {
            if (!state) { allActive = false; break; }
        }
        SetOpen(allActive);
    }

    void DoorObject::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        bool drewSprite = spriteVisual.Draw(position, size, 0.0f, glm::vec3(1.0f), open ? 0.75f : 1.0f);
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            glm::vec3 color = open ? glm::vec3(0.30f, 0.85f, 0.30f) : glm::vec3(0.55f, 0.20f, 0.20f);

            RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f), color, open ? 0.35f : 1.0f, 1.0f, true);
            RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.05f), 1.0f, 1.5f, false);
        }

        RenderDebugTextBlock(position + size * 0.5f,
                             {
                                 {"Puerta",                              glm::vec3(1.0f)},
                                 {"C" + std::to_string(Class_Door),      glm::vec3(0.75f, 0.95f, 1.0f)},
                                 {"T" + std::to_string(textureId),       glm::vec3(0.65f, 1.0f, 0.60f)},
                                 {"#" + std::to_string(objectId),        glm::vec3(1.0f, 0.92f, 0.45f)},
                                 {open ? "OPEN" : "CLOSED",            open ? glm::vec3(0.45f, 1.0f, 0.55f) : glm::vec3(1.0f, 0.60f, 0.55f)}
                             });
    }
}
