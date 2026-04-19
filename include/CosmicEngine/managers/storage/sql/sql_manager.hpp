#ifndef COSMIC_SQLMANAGER_HPP
#define COSMIC_SQLMANAGER_HPP

/**
 * @file sql_manager.hpp
 * @brief Declares the SQLite-based persistence manager used by the engine.
 */

#include <iostream>
#include <sqlite/sqlite3.h>
#include <string>
#include <unordered_map>
#include <functional>

namespace CosmicEngine
{
    class Object;

    /**
     * @brief Describes how a class is serialized to and from a SQL table.
     */
    struct SerializedObjects
    {
        /** @brief SQL columns used for the serialized object. */
        std::vector<std::pair<std::string, std::string>> Columns;
        /** @brief Callback that reconstructs an object from SQL row values. */
        std::function<Object*(char**)> Constructor;
    };

    /**
     * @brief Manages SQLite persistence for engine objects.
     *
     * SQLManager provides a table-per-class persistence strategy.  Register
     * each class with RegisterSerialization(), then call SaveObjectsData()
     * and LoadObjectsData() to round-trip objects to/from a SQLite database.
     *
     * @par Example — basic SQL persistence workflow
     * @code
     * SQL_MN.OpenDatabase("saves/world.db");
     *
     * SQL_MN.RegisterSerialization("Enemy",
     *     {{"name", "TEXT"}, {"health", "INTEGER"}, {"x", "REAL"}, {"y", "REAL"}},
     *     [](char** row) -> CosmicEngine::Object* {
     *         return new Enemy(row[0], std::atoi(row[1]),
     *             std::atof(row[2]), std::atof(row[3]));
     *     });
     *
     * SQL_MN.SaveObjectsData("Enemy");
     * SQL_MN.LoadObjectsData("Enemy");
     * @endcode
     */
    class SQLManager
    {
    private:
        SQLManager();
        ~SQLManager();
        SQLManager(const SQLManager &) = delete;
        SQLManager &operator=(const SQLManager &) = delete;

        sqlite3 *db;
        std::string dbName;
        std::unordered_map<std::string, SerializedObjects> serialization_resources;


    public:
        /** @brief Returns the singleton instance of the SQL manager. */
        static SQLManager &GetInstance();

        /** @brief Initializes the SQL manager state.
         *  @return True when SQLite global initialization succeeds.
         *  A false result signals that persistent storage is unavailable and callers should stop startup before saving/loading data.
         */
        bool init();
        /** @brief Shuts the SQL manager down and closes the active database before releasing SQLite global state. */
        void shutdown();

        /** @brief Opens a SQLite database file. */
        bool OpenDatabase(const std::string &newDbName);

        /** @brief Executes a SQL statement that does not require a row callback. */
        bool ExecuteSQL(const std::string &sql);
        /** @brief Executes a SQL query and forwards each row to the provided callback. */
        void ExecuteQuery(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

        /** @brief Creates a SQL table with the provided column definition string. */
        void CreateTable(const std::string &tableName, const std::string &columns);
        /** @brief Queries a SQL table and forwards rows to the provided callback. */
        void ConsultTable(const std::string &tableName, const std::string &columns, int (*callback)(void *, int, char **, char **), void* data = nullptr);


        /** @brief Registers SQL serialization rules for a class. */
        void RegisterSerialization(const std::string& className, std::vector<std::pair<std::string, std::string>> columns, std::function<Object*(char**)> constructor);

        /** @brief Clears the current database contents. */
        void ClearDatabase();
        /** @brief Saves objects of the requested class from the runtime into the database. */
        void SaveObjectsData(const std::string& className);
        /** @brief Loads objects of the requested class from the database into the runtime. */
        void LoadObjectsData(const std::string& className);

        /** @brief Inserts raw values into a SQL table. */
        void InsertData(const std::string &table, const std::string &columns, const std::string &values);
        /** @brief Returns the identifier of the last inserted SQL row. */
        int GetLastInsertID();

        /** @brief Closes the active database connection. */
        void CloseDatabase();

    };
}

#endif // COSMIC_SQLMANAGER_HPP
