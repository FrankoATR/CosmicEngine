/**
 * @file json_manager.cpp
 * @brief Implements the JSON persistence manager used by the engine.
 */

#include "json_manager.hpp"

#include "../../object/object_manager.hpp"
#include "../../../models/object/object.hpp"
#include "../../../utils/log.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace CosmicEngine
{
    JsonManager& JsonManager::GetInstance()
    {
        static JsonManager instance;
        return instance;
    }

    JsonManager::JsonManager()
    {
        RUNTIME_LIFECYCLE("Json manager", "created");
    }

    JsonManager::~JsonManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("Json manager", "destroyed");
    }

    void JsonManager::init()
    {
        initialized = true;
        ClearDocument();
        RUNTIME_LIFECYCLE("Json manager", "initialized");
    }

    void JsonManager::shutdown()
    {
        if (!initialized)
            return;

        // Flush pending changes before shutdown so the caller can see persistence failures in logs.
        if (!SaveFile(false))
        {
            RUNTIME_WARNING("[JsonManager] Pending changes could not be flushed during shutdown.");
        }

        initialized = false;
        RUNTIME_LIFECYCLE("Json manager", "shutdown");
    }

    void JsonManager::EnsureDocumentShape()
    {
        // Keep the document schema stable so save/load code can assume the same top-level shape.
        if (!doc.is_object())
            doc = nlohmann::json::object();

        if (!doc.contains("meta") || !doc["meta"].is_object())
            doc["meta"] = nlohmann::json::object();

        if (!doc["meta"].contains("version"))
            doc["meta"]["version"] = 1;

        if (!doc.contains("objects") || !doc["objects"].is_object())
            doc["objects"] = nlohmann::json::object();
    }

    void JsonManager::ClearDocument()
    {
        doc = nlohmann::json::object();
        EnsureDocumentShape();
        dirty = true;
    }

    bool JsonManager::WriteFileAtomic(const std::string& path, const std::string& content)
    {
        namespace fs = std::filesystem;

        fs::path p(path);
        fs::path tmp = p;
        tmp += ".tmp";

        {
            std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
            if (!out.is_open())
                return false;

            out.write(content.data(), static_cast<std::streamsize>(content.size()));
            out.flush();
            out.close();
        }

        std::error_code ec;

        // En Windows a veces rename falla si el destino existe, entonces lo removemos primero.
        if (fs::exists(p, ec))
        {
            fs::remove(p, ec);
        }

        fs::rename(tmp, p, ec);
        if (ec)
        {
            // Intento alternativo: copiar + borrar
            ec.clear();
            fs::copy_file(tmp, p, fs::copy_options::overwrite_existing, ec);
            if (ec) return false;
            fs::remove(tmp, ec);
        }

        return true;
    }

    bool JsonManager::OpenFile(const std::string& path)
    {
        if (!initialized)
        {
            RUNTIME_WARNING("[JsonManager] OpenFile failed: call init() first.");
            return false;
        }

        filePath = path;

        // Si no existe, creamos doc nuevo.
        if (!std::filesystem::exists(filePath))
        {
            ClearDocument();
            dirty = true;
            return SaveFile(true);
        }

        std::ifstream in(filePath, std::ios::binary);
        if (!in.is_open())
        {
            RUNTIME_WARNING("[JsonManager] Failed to open file: " << filePath);
            return false;
        }

        std::stringstream buffer;
        buffer << in.rdbuf();
        in.close();

        try
        {
            doc = nlohmann::json::parse(buffer.str());
        }
        catch (const std::exception& e)
        {
            RUNTIME_WARNING("[JsonManager] JSON parse error: " << e.what());
            // No abortamos el juego: creamos uno nuevo para no romper el flujo.
            ClearDocument();
            dirty = true;
            return SaveFile(true);
        }

        EnsureDocumentShape();
        dirty = false;
        return true;
    }

    bool JsonManager::SaveFile(bool force)
    {
        if (!initialized)
            return false;

        if (filePath.empty())
        {
            RUNTIME_WARNING("[JsonManager] SaveFile failed: filePath is empty. Call OpenFile(path).");
            return false;
        }

        if (!dirty && !force)
            return true;

        EnsureDocumentShape();

        // dump(2) para legible; puedes cambiar a dump() para compacto.
        std::string content = doc.dump(2);

        if (!WriteFileAtomic(filePath, content))
        {
            RUNTIME_WARNING("[JsonManager] Failed to write file: " << filePath);
            return false;
        }

        dirty = false;
        return true;
    }

    void JsonManager::RegisterSerialization(
        const std::string& className,
        std::vector<std::string> fields,
        std::function<nlohmann::json(const Object*)> serializer,
        std::function<Object*(const nlohmann::json&)> constructor)
    {
        JsonSerializedObjects reg;
        reg.Fields = std::move(fields);
        reg.Serializer = std::move(serializer);
        reg.Constructor = std::move(constructor);

        serialization_resources[className] = std::move(reg);
    }

    bool JsonManager::HasClassSection(const std::string& className) const
    {
        if (!doc.is_object()) return false;
        if (!doc.contains("objects")) return false;
        if (!doc["objects"].is_object()) return false;
        return doc["objects"].contains(className);
    }

    std::vector<std::string> JsonManager::GetRegisteredFields(const std::string& className) const
    {
        auto itReg = serialization_resources.find(className);
        if (itReg == serialization_resources.end())
        {
            return {};
        }

        return itReg->second.Fields;
    }

    Object* JsonManager::CreateObjectFromSerializedData(const std::string& className, const nlohmann::json& data) const
    {
        auto itReg = serialization_resources.find(className);
        if (itReg == serialization_resources.end())
        {
            return nullptr;
        }

        return itReg->second.Constructor(data);
    }

    void JsonManager::SaveObjectsData(const std::string& className)
    {
        if (!initialized)
            return;

        EnsureDocumentShape();

        auto itReg = serialization_resources.find(className);
        if (itReg == serialization_resources.end())
        {
            RUNTIME_WARNING("[JsonManager] SaveObjectsData: class not registered: " << className);
            return;
        }

        const auto& reg = itReg->second;

        auto objs = ObjectManager::GetInstance().FindByClassName(className);

        nlohmann::json arr = nlohmann::json::array();

        for (Object* obj : objs)
        {
            if (!obj) continue;

            nlohmann::json one = reg.Serializer(obj);
            if (one.is_null())
                continue;

            arr.push_back(one);
        }

        doc["objects"][className] = arr;
        dirty = true;
    }

    void JsonManager::LoadObjectsData(const std::string& className)
    {
        if (!initialized)
            return;

        EnsureDocumentShape();

        auto itReg = serialization_resources.find(className);
        if (itReg == serialization_resources.end())
        {
            RUNTIME_WARNING("[JsonManager] LoadObjectsData: class not registered: " << className);
            return;
        }

        const auto& reg = itReg->second;

        if (!doc["objects"].contains(className))
            return;

        const nlohmann::json& arr = doc["objects"][className];
        if (!arr.is_array())
            return;

        for (const auto& entry : arr)
        {
            try
            {
                Object* obj = reg.Constructor(entry);
                if (obj)
                    ObjectManager::GetInstance().Add(obj);
            }
            catch (const std::exception& e)
            {
                RUNTIME_WARNING("[JsonManager] LoadObjectsData error (" << className << "): " << e.what());
            }
        }
    }
}
