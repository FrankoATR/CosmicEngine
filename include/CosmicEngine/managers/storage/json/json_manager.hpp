#ifndef COSMIC_JSONMANAGER_HPP
#define COSMIC_JSONMANAGER_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

#include <nlohmann/json.hpp>

namespace CosmicEngine
{
    class Object;

    struct JsonSerializedObjects
    {
        // Opcional: nombres de campos (solo para documentación/validación ligera)
        std::vector<std::string> Fields;

        // Convierte Object -> json (1 objeto)
        std::function<nlohmann::json(const Object*)> Serializer;

        // Convierte json -> Object* (1 objeto)
        std::function<Object*(const nlohmann::json&)> Constructor;
    };

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
        static JsonManager& GetInstance();

        void init();
        void shutdown();

        // Carga/crea el archivo JSON de nivel
        bool OpenFile(const std::string& path);

        // Guarda el doc actual a disco (si dirty)
        bool SaveFile(bool force = false);

        // Limpia el documento en memoria (no borra archivo hasta SaveFile)
        void ClearDocument();

        // Registro por clase (Spike, Enemy, etc.)
        void RegisterSerialization(
            const std::string& className,
            std::vector<std::string> fields,
            std::function<nlohmann::json(const Object*)> serializer,
            std::function<Object*(const nlohmann::json&)> constructor
        );

        // Persiste/lee objetos desde ObjectManager
        void SaveObjectsData(const std::string& className);
        void LoadObjectsData(const std::string& className);

        // Utilidades
        bool HasClassSection(const std::string& className) const;
    };
}

#endif // COSMIC_JSONMANAGER_HPP
