#include "DataBaseManager.hpp"
#include "../Object/ObjectManager.hpp"
#include "../../Models/Object/Object.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>

namespace WandEngine
{
    DataBaseManager &DataBaseManager::GetInstance()
    {
        static DataBaseManager instance;
        return instance;
    }

    DataBaseManager::DataBaseManager()
    {
        std::cout << "DataBase manager created" << std::endl;
    }

    DataBaseManager::~DataBaseManager()
    {
        std::cout << "DataBase manager destroyed" << std::endl;
    }

    void DataBaseManager::Init()
    {
        sqlite3_initialize();
        std::cout << "DataBase manager initialized" << std::endl;
    }

    void DataBaseManager::Shutdown()
    {
        sqlite3_shutdown();
    }


    bool DataBaseManager::OpenDatabase(const std::string &newDbName)
    {
        if (db)
        {
            sqlite3_close(db);
            std::cout << "Cerrando base de datos actual: " << dbName << std::endl;
            db = nullptr;
        }
        dbName = newDbName;
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK)
        {
            std::cerr << "Error al abrir la base de datos: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        std::cout << "Base de datos abierta para guardar: " << dbName << std::endl;
        ExecuteSQL("PRAGMA busy_timeout = 5000;");
        // ClearDatabase();

        ExecuteSQL("BEGIN TRANSACTION;");

        return true;
    }



    void DataBaseManager::ClearDatabase()
    {
        if (!db)
        {
            std::cerr << "Error: No hay base de datos abierta para borrar tablas." << std::endl;
            return;
        }
        std::cout << "Eliminando todas las tablas existentes..." << std::endl;
        ExecuteSQL("PRAGMA foreign_keys = OFF;");
        ExecuteSQL("PRAGMA wal_checkpoint(TRUNCATE);");

        std::vector<std::string> tableNames;
        auto callback = [](void *data, int argc, char **argv, char **colNames) -> int
        {
            auto *tables = static_cast<std::vector<std::string> *>(data);
            if (argc > 0 && argv[0])
                tables->push_back(argv[0]);
            return 0;
        };
        ExecuteQuery("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';", callback, &tableNames);
        for (const auto &table : tableNames)
        {
            std::cout << "Borrando tabla: " << table << std::endl;
            ExecuteSQL("DROP TABLE IF EXISTS " + table + ";");
        }
        ExecuteSQL("PRAGMA foreign_keys = ON;");
    }

    bool DataBaseManager::ExecuteSQL(const std::string &sql)
    {
        if (!db)
        {
            std::cerr << "Error: No hay base de datos abierta." << std::endl;
            return false;
        }
        char *errMsg = nullptr;
        int attempts = 5;
        while (attempts--)
        {
            int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
            if (rc == SQLITE_OK)
                return true;
            else if (rc == SQLITE_BUSY)
            {
                std::cerr << "Base de datos bloqueada, reintentando..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            else
            {
                std::cerr << "Error en SQL: " << errMsg << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                sqlite3_free(errMsg);
                return false;
            }
        }
        std::cerr << "Error: No se pudo ejecutar la consulta SQL tras múltiples intentos." << std::endl;
        return false;
    }

    void DataBaseManager::ExecuteQuery(const std::string &sql, int (*callback)(void *, int, char **, char **), void *data)
    {
        if (!db)
        {
            std::cerr << "Error: No hay base de datos abierta." << std::endl;
            return;
        }
        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, data, &errMsg);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Error en consulta SQL: " << errMsg << std::endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            sqlite3_free(errMsg);
        }
    }

    void DataBaseManager::CreateTable(const std::string &tableName, const std::string &columns)
    {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columns + ");";
        std::cout << sql << std::endl;
        ExecuteSQL(sql);
    }

    void DataBaseManager::ConsultTable(const std::string &tableName, const std::string &columns, int (*callback)(void *, int, char **, char **), void *data)
    {
        std::string sql = "SELECT " + columns + " FROM " + tableName + ";";
        std::cout << sql << std::endl;
        ExecuteQuery(sql, callback, data);
    }

    void DataBaseManager::RegisterSerialization(const std::string &className, std::vector<std::pair<std::string, std::string>> columns, std::function<Object *(char **)> constructor)
    {
        serialization_resources[className] = {columns, constructor};
    }

    void DataBaseManager::SaveObjectsData(const std::string &className)
    {
        auto objs = ObjectManager::GetInstance().FindByClassName(className);

        if (objs.empty())
            return;

        auto first = dynamic_cast<Object *>(objs[0]);
        auto columns = serialization_resources[className].Columns;

        std::ostringstream colTypeStr;
        for (size_t i = 0; i < columns.size(); ++i)
        {
            colTypeStr << columns[i].first << " " << columns[i].second;
            if (i != columns.size() - 1)
                colTypeStr << ", ";
        }

        CreateTable(className, "id INTEGER PRIMARY KEY AUTOINCREMENT, " + colTypeStr.str());

        std::ostringstream colStr;
        for (size_t i = 0; i < columns.size(); ++i)
        {
            colStr << columns[i].first;
            if (i != columns.size() - 1)
                colStr << ", ";
        }

        for (Object *obj : objs)
        {
            std::ostringstream valStr;
            auto values = obj->GetAllValues();

            for (size_t i = 0; i < values.size(); ++i)
            {
                valStr << values[i];
                if (i != values.size() - 1)
                    valStr << ", ";
            }
            InsertData(className, colStr.str(), valStr.str());
        }
    }

    void DataBaseManager::LoadObjectsData(const std::string &className)
    {
        auto callback = [](void *userdata, int argc, char **argv, char **colNames) -> int
        {
            std::string className = *static_cast<std::string *>(userdata);

            DataBaseManager *self = &DataBaseManager::GetInstance();

            auto it = self->serialization_resources.find(className);
            if (it != self->serialization_resources.end())
            {
                Object *obj = it->second.Constructor(argv);
                ObjectManager::GetInstance().Add(obj);
            }
            return 0;
        };

        std::ostringstream cols;
        for (size_t i = 0; i < serialization_resources[className].Columns.size(); ++i)
        {
            cols << serialization_resources[className].Columns[i].first;
            if (i != serialization_resources[className].Columns.size() - 1)
                cols << ", ";
        }

        ConsultTable(className, cols.str(), callback, (void *)&className);
    }

    void DataBaseManager::InsertData(const std::string &table, const std::string &columns, const std::string &values)
    {
        std::string sql = "INSERT INTO " + table + " (" + columns + ") VALUES (" + values + ");";
        std::cout << sql << std::endl;
        ExecuteSQL(sql);
    }

    int DataBaseManager::GetLastInsertID()
    {
        if (!db)
            return -1;
        return static_cast<int>(sqlite3_last_insert_rowid(db));
    }

    void DataBaseManager::CloseDatabase()
    {
        ExecuteSQL("COMMIT;");

        if (db)
        {
            ExecuteSQL("PRAGMA wal_checkpoint(TRUNCATE);");
            ExecuteSQL("PRAGMA optimize;");
            int result = sqlite3_close(db);
            if (result != SQLITE_OK)
                std::cerr << "Error al cerrar la base de datos: " << sqlite3_errmsg(db) << std::endl;
            else
                std::cout << "Base de datos cerrada: " << dbName << std::endl;
            db = nullptr;
        }
    }

}
