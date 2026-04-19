#ifndef COSMIC_OBJECTMANAGER_HPP
#define COSMIC_OBJECTMANAGER_HPP

/**
 * @file object_manager.hpp
 * @brief Declares the object manager responsible for runtime object ownership and queries.
 */

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <string>

namespace CosmicEngine
{
    class Object;

    /**
     * @brief Owns runtime objects and provides update, draw, and search operations.
     *
     * ObjectManager holds every live Object in the scene.  Adding an object
     * assigns it a unique ID and brings it into the update/draw cycle.  Objects
     * marked as destroyed are cleaned up automatically on the next update pass.
     *
     * @par Example — adding, searching, and removing objects
     * @code
     * // Add a new object to the runtime:
     * auto* obj = new JsonDemoObject({100,200}, {96,96}, "NPC", 100, 1);
     * OBJ_MN.Add(obj);
     *
     * // Find all objects of a given class:
     * auto npcs = OBJ_MN.FindByClassName("JsonDemoObject");
     * for (auto* npc : npcs)
     *     npc->SetColor({1.0f, 0.0f, 0.0f});
     *
     * // Remove by ID:
     * OBJ_MN.Remove(obj->GetID());
     * @endcode
     */
    class ObjectManager
    {
    private:
        ObjectManager();
        ~ObjectManager();
        ObjectManager(const ObjectManager &) = delete;
        ObjectManager &operator=(const ObjectManager &) = delete;

        std::vector<Object *> objects;
        std::vector<int> toDelete;

        int nextEntityId;

    public:
        /** @brief Returns the singleton instance of the object manager. */
        static ObjectManager &GetInstance();

        /** @brief Initializes the object manager state. */
        void init();

        /** @brief Draws every visible object managed by the runtime. */
        void draw();
        /**
         * @brief Updates every live object and processes deferred removals.
         * @param deltaTime Fixed update time step.
         */
        void update(float deltaTime);
        
        /** @brief Adds a new object to the runtime and assigns it an identifier. */
        void Add(Object *obj);
        /** @brief Removes and deletes an object by identifier. */
        void Remove(int entityId);

        /** @brief Finds an object by its unique identifier. */
        Object *FindById(int EntityId);
        /** @brief Finds all objects with the given logical class name. */
        std::vector<Object *> FindByClassName(std::string className);
        /** @brief Returns a snapshot of all managed objects. */
        std::vector<Object *> GetAll();
        /** @brief Deletes and removes every managed object. */
        void Clear();

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            /** @brief Finds all 2D objects containing the provided position. */
            std::vector<Object *> FindByPosition(glm::vec2 position);
            /** @brief Finds all 2D objects currently under the mouse cursor. */
            std::vector<Object *> FindByMousePosition();
            /** @brief Finds all 2D objects on the requested layer. */
            std::vector<Object *> FindByLayer(int layerId);
            /** @brief Finds all 2D objects intersecting the provided area. */
            std::vector<Object *> FindByArea(glm::vec2 point1, glm::vec2 point2);
            /** @brief Sorts 2D objects by layer and stable identifier order. */
            void SortByLayer();

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            /** @brief Finds all 3D objects containing the provided position. */
            std::vector<Object *> FindByPosition(glm::vec3 position);
            /** @brief Finds all 3D objects inside a requested volume. */
            std::vector<Object *> FindByVolume(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
            /** @brief Finds 3D objects intersected by the current view ray. */
            std::vector<Object *> FindByViewRayCast(float maxDistance);
            /**
             * @brief Tests whether an object exists along the current camera view direction.
             * @param maxDistance Maximum ray distance.
             * @param hitPosition Optional hit object position output.
             * @param lastFreePosition Optional last free position output.
             * @return True when an object was found along the view direction.
             */
            bool ObjectOnViewDirection(float maxDistance, glm::vec3 *hitPosition = nullptr, glm::vec3 *lastFreePosition = nullptr);

        #else
            #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    };

}

#endif // COSMIC_OBJECTMANAGER_HPP