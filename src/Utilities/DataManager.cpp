#include "DataManager.hpp"
#include "../WandAllegroEngine/Models/GameObject.hpp"
#include <iostream>

namespace WandEngine
{
    DataManager::DataManager()
    {
        if (sqlite3_open("./saves/game_data.db", &this->db) != SQLITE_OK)
        {
            std::cerr << "No se pudo abrir la base de datos." << std::endl;
        }
        else
        {
            CreateTable();
        }
    }

    DataManager::~DataManager()
    {
    }

    void DataManager::CreateTable()
    {
        const char *createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS GameObjects (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL,
            PositionX REAL,
            PositionY REAL,
            SizeX REAL,
            SizeY REAL,
            LayerId INTEGER,
            AliveInGameManager BOOLEAN
        );
    )";

        char *errorMessage = nullptr;
        int exit = sqlite3_exec(this->db, createTableSQL, nullptr, nullptr, &errorMessage);

        if (exit != SQLITE_OK)
        {
            std::cerr << "Error al crear la tabla: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
        }
        else
        {
            std::cout << "Tabla creada o ya existe." << std::endl;
        }
    }

    void DataManager::ClearGameObjects()
    {
        const char *deleteSQL = "DELETE FROM GameObjects;";
        char *errorMessage = nullptr;

        int exit = sqlite3_exec(db, deleteSQL, nullptr, nullptr, &errorMessage);
        if (exit != SQLITE_OK)
        {
            std::cerr << "Error al limpiar la tabla GameObjects: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
        }
        else
        {
            std::cout << "Tabla GameObjects limpiada correctamente." << std::endl;
        }
    }

    void DataManager::SaveData(const GameObject &obj)
    {
        ClearGameObjects();

        const char *insertSQL = R"(
            INSERT INTO GameObjects (Name, PositionX, PositionY, SizeX, SizeY, LayerId, AliveInGameManager)
            VALUES (?, ?, ?, ?, ?, ?, ?);
        )";

        sqlite3_stmt *stmt;
        int exit = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
        if (exit != SQLITE_OK)
        {
            std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Bind de los parámetros
        sqlite3_bind_text(stmt, 1, obj.GetObjectName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, obj.GetPosition().x);
        sqlite3_bind_double(stmt, 4, obj.GetPosition().y);
        sqlite3_bind_double(stmt, 5, obj.GetSize().x);
        sqlite3_bind_double(stmt, 6, obj.GetSize().y);
        sqlite3_bind_int(stmt, 7, obj.GetLayerId());
        sqlite3_bind_int(stmt, 8, obj.GetAliveInGameManager() ? 1 : 0);

        // Ejecutar la consulta
        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            std::cerr << "Error al insertar el GameObject: " << sqlite3_errmsg(db) << std::endl;
        }
        else
        {
            std::cout << "GameObject insertado correctamente." << std::endl;
        }

        sqlite3_finalize(stmt);
    }

    std::vector<GameObject> DataManager::LoadData()
    {
        const char *selectSQL = "SELECT Name, PositionX, PositionY, SizeX, SizeY, LayerId, AliveInGameManager FROM GameObjects;";
        sqlite3_stmt *stmt;
        std::vector<GameObject> objects;

        int exit = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);
        if (exit != SQLITE_OK)
        {
            std::cerr << "Error al preparar la consulta SELECT: " << sqlite3_errmsg(db) << std::endl;
            return objects;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::string name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            float posX = sqlite3_column_double(stmt, 2);
            float posY = sqlite3_column_double(stmt, 3);
            float sizeX = sqlite3_column_double(stmt, 4);
            float sizeY = sqlite3_column_double(stmt, 5);
            int layerId = sqlite3_column_int(stmt, 6);
            bool aliveInGameManager = sqlite3_column_int(stmt, 7) != 0;

            // Construir el objeto
            #include "../Entities/LinkObject.hpp"
            GameObject obj(Object::DynamicEntity, {posX, posY}, {sizeX, sizeY}, name, nullptr, layerId);
            objects.push_back(obj);
        }

        sqlite3_finalize(stmt);
        return objects;
    }

}
