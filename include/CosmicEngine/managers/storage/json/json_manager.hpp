#ifndef COSMIC_JSONMANAGER_HPP
#define COSMIC_JSONMANAGER_HPP

/**
 * @file json_manager.hpp
 * @brief Declares the JSON-based persistence manager used by the engine.
 */

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

#include <nlohmann/json.hpp>

namespace CosmicEngine
{
    class Object;

    /**
     * @brief Describes how a class is serialized to and from JSON.
     */
    struct JsonSerializedObjects
    {
        /** @brief Optional field names used for lightweight validation or documentation. */
        std::vector<std::string> Fields;

        /** @brief Callback that converts an object into a JSON value. */
        std::function<nlohmann::json(const Object*)> Serializer;

        /** @brief Callback that reconstructs an object from a JSON value. */
        std::function<Object*(const nlohmann::json&)> Constructor;
    };

    /**
     * @brief Manages JSON persistence for engine objects and runtime state.
     *
     * JsonManager provides a simple workflow for saving and loading game objects
     * to/from a JSON file.  Each class registers a serializer and a deserializer
     * via RegisterSerialization().  At save-time, SaveObjectsData() queries the
     * ObjectManager for every matching object and serializes them into the
     * in-memory JSON document, which is then flushed to disk with SaveFile().
     *
    * @par Example - full save/load workflow
     * @code
     * // 1) Register serialization for your class (once)
     * JSON_MN.RegisterSerialization(
     *     "JsonDemoObject",
     *     {"label", "health", "position", "size"},
     *     [](const CosmicEngine::Object* o) -> nlohmann::json {
     *         auto* d = dynamic_cast<const JsonDemoObject*>(o);
     *         return {{"label", d->GetLabel()}, {"health", d->GetHealth()},
     *                 {"position", {{"x", d->GetPosition().x}, {"y", d->GetPosition().y}}}};
     *     },
     *     [](const nlohmann::json& data) -> CosmicEngine::Object* {
     *         return new JsonDemoObject(
     *             {data["position"]["x"], data["position"]["y"]},
     *             {96, 96}, data.value("label", "obj"), data.value("health", 100), 1);
     *     });
     *
     * // 2) Open a file
     * JSON_MN.OpenFile("saves/demo.json");
     *
     * // 3) Save:
     * JSON_MN.SaveObjectsData("JsonDemoObject");
     * JSON_MN.SaveFile(true);
     *
     * // 4) Load:
     * if (JSON_MN.HasClassSection("JsonDemoObject"))
     *     JSON_MN.LoadObjectsData("JsonDemoObject");
     * @endcode
     */
    class JsonManager
    {
    private:
        JsonManager();
        ~JsonManager();
        JsonManager(const JsonManager&) = delete;
        JsonManager& operator=(const JsonManager&) = delete;

        bool initialized = false;

        std::string filePath;
        nlohmann::json doc;   // Documento completo en memoria
        bool dirty = false;

        std::unordered_map<std::string, JsonSerializedObjects> serialization_resources;

        void EnsureDocumentShape();
        bool WriteFileAtomic(const std::string& path, const std::string& content);

    public:
        /** @brief Returns the singleton instance of the JSON manager. */
        static JsonManager& GetInstance();

        /** @brief Initializes the JSON manager state. */
        void init();
        /** @brief Shuts the JSON manager down and flushes pending changes. */
        void shutdown();

        /** @brief Opens an existing JSON document or creates a new one on demand. */
        bool OpenFile(const std::string& path);

        /** @brief Saves the current in-memory document to disk. */
        bool SaveFile(bool force = false);

        /** @brief Clears the in-memory document without deleting the file immediately. */
        void ClearDocument();

        /** @brief Registers JSON serialization rules for a class. */
        void RegisterSerialization(
            const std::string& className,
            std::vector<std::string> fields,
            std::function<nlohmann::json(const Object*)> serializer,
            std::function<Object*(const nlohmann::json&)> constructor
        );

        /** @brief Saves objects of the requested class from the runtime into the JSON document. */
        void SaveObjectsData(const std::string& className);
        /** @brief Loads objects of the requested class from the JSON document into the runtime. */
        void LoadObjectsData(const std::string& className);

        /** @brief Returns the registered serialized field names for a class, or an empty list when it is not registered. */
        std::vector<std::string> GetRegisteredFields(const std::string& className) const;
        /** @brief Builds a single object instance from serialized JSON using a previously registered constructor. */
        Object* CreateObjectFromSerializedData(const std::string& className, const nlohmann::json& data) const;

        /** @brief Returns whether the JSON document already contains a section for the given class. */
        bool HasClassSection(const std::string& className) const;
    };
}

#endif // COSMIC_JSONMANAGER_HPP
