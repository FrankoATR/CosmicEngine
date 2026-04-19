/**
 * @file sql_manager.cpp
 * @brief Implements the SQLite persistence manager used by the engine.
 */

#include "sql_manager.hpp"
#include "../../object/object_manager.hpp"
#include "../../../models/object/object.hpp"
#include "../../../utils/log.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>

namespace CosmicEngine
{
    SQLManager &SQLManager::GetInstance()
    {
        static SQLManager instance;
        return instance;
    }

    SQLManager::SQLManager()
    {
        RUNTIME_LIFECYCLE("Database manager", "created");
    }

    SQLManager::~SQLManager()
    {
        RUNTIME_LIFECYCLE("Database manager", "destroyed");
    }

    bool SQLManager::init()
    {
        if (sqlite3_initialize() != SQLITE_OK)
        {
            RUNTIME_WARNING("[SQLManager] Failed to initialize SQLite.");
            return false;
        }
        RUNTIME_LIFECYCLE("Database manager", "initialized");
        return true;
    }

    void SQLManager::shutdown()
    {
        CloseDatabase();
        sqlite3_shutdown();
        RUNTIME_LIFECYCLE("Database manager", "shutdown");
    }


    bool SQLManager::OpenDatabase(const std::string &newDbName)
    {
        if (db)
        {
            sqlite3_close(db);
            RUNTIME_INFO("[SQLManager] Closing current database: " << dbName);
            db = nullptr;
        }
        dbName = newDbName;
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK)
        {
            RUNTIME_WARNING("[SQLManager] Failed to open database: " << sqlite3_errmsg(db));
            return false;
        }
        RUNTIME_INFO("[SQLManager] Database opened for saving: " << dbName);
        ExecuteSQL("PRAGMA busy_timeout = 5000;");
        // ClearDatabase();

        ExecuteSQL("BEGIN TRANSACTION;");

        return true;
    }



    void SQLManager::ClearDatabase()
    {
        if (!db)
        {
            RUNTIME_WARNING("[SQLManager] No open database is available for dropping tables.");
            return;
        }
        RUNTIME_INFO("[SQLManager] Dropping all existing tables...");
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
            RUNTIME_INFO("[SQLManager] Dropping table: " << table);
            ExecuteSQL("DROP TABLE IF EXISTS " + table + ";");
        }
        ExecuteSQL("PRAGMA foreign_keys = ON;");
    }

    bool SQLManager::ExecuteSQL(const std::string &sql)
    {
        if (!db)
        {
            RUNTIME_WARNING("[SQLManager] No open database is available.");
            return false;
        }
        char *errMsg = nullptr;
        int attempts = 5;
        while (attempts--)
        {
            // Retry transient SQLITE_BUSY states because object saves can happen in bursty sequences.
            int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
            if (rc == SQLITE_OK)
                return true;
            else if (rc == SQLITE_BUSY)
            {
                RUNTIME_WARNING("[SQLManager] Database is locked, retrying...");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            else
            {
                RUNTIME_WARNING("[SQLManager] SQL error: " << errMsg);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                sqlite3_free(errMsg);
                return false;
            }
        }
        RUNTIME_WARNING("[SQLManager] Failed to execute the SQL statement after multiple attempts.");
        return false;
    }

    void SQLManager::ExecuteQuery(const std::string &sql, int (*callback)(void *, int, char **, char **), void *data)
    {
        if (!db)
        {
            RUNTIME_WARNING("[SQLManager] No open database is available.");
            return;
        }
        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, data, &errMsg);
        if (rc != SQLITE_OK)
        {
            RUNTIME_WARNING("[SQLManager] SQL query error: " << errMsg);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            sqlite3_free(errMsg);
        }
    }

    void SQLManager::CreateTable(const std::string &tableName, const std::string &columns)
    {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columns + ");";
        RUNTIME_INFO("[SQLManager] " << sql);
        ExecuteSQL(sql);
    }

    void SQLManager::ConsultTable(const std::string &tableName, const std::string &columns, int (*callback)(void *, int, char **, char **), void *data)
    {
        std::string sql = "SELECT " + columns + " FROM " + tableName + ";";
        RUNTIME_INFO("[SQLManager] " << sql);
        ExecuteQuery(sql, callback, data);
    }

    void SQLManager::RegisterSerialization(const std::string &className, std::vector<std::pair<std::string, std::string>> columns, std::function<Object *(char **)> constructor)
    {
        serialization_resources[className] = {columns, constructor};
    }

    void SQLManager::SaveObjectsData(const std::string &className)
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

    void SQLManager::LoadObjectsData(const std::string &className)
    {
        auto callback = [](void *userdata, int argc, char **argv, char **colNames) -> int
        {
            std::string className = *static_cast<std::string *>(userdata);

            SQLManager *self = &SQLManager::GetInstance();

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

    void SQLManager::InsertData(const std::string &table, const std::string &columns, const std::string &values)
    {
        std::string sql = "INSERT INTO " + table + " (" + columns + ") VALUES (" + values + ");";
        RUNTIME_INFO("[SQLManager] " << sql);
        ExecuteSQL(sql);
    }

    int SQLManager::GetLastInsertID()
    {
        if (!db)
            return -1;
        return static_cast<int>(sqlite3_last_insert_rowid(db));
    }

    void SQLManager::CloseDatabase()
    {
        ExecuteSQL("COMMIT;");

        if (db)
        {
            ExecuteSQL("PRAGMA wal_checkpoint(TRUNCATE);");
            ExecuteSQL("PRAGMA optimize;");
            int result = sqlite3_close(db);
            if (result != SQLITE_OK)
                RUNTIME_WARNING("[SQLManager] Failed to close database: " << sqlite3_errmsg(db));
            else
                RUNTIME_INFO("[SQLManager] Database closed: " << dbName);
            db = nullptr;
        }
    }

}
