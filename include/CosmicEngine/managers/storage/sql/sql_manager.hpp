#ifndef COSMIC_SQLMANAGER_HPP
#define COSMIC_SQLMANAGER_HPP

#include <iostream>
#include <sqlite/sqlite3.h>
#include <string>
#include <unordered_map>
#include <functional>

namespace CosmicEngine
{
    class Object;

    struct SerializedObjects
    {
        std::vector<std::pair<std::string, std::string>> Columns;
        std::function<Object*(char**)> Constructor;
    };

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
        static SQLManager &GetInstance();

        void init();
        void shutdown();

        bool OpenDatabase(const std::string &newDbName);

        bool ExecuteSQL(const std::string &sql);
        void ExecuteQuery(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

        void CreateTable(const std::string &tableName, const std::string &columns);
        void ConsultTable(const std::string &tableName, const std::string &columns, int (*callback)(void *, int, char **, char **), void* data = nullptr);


        void RegisterSerialization(const std::string& className, std::vector<std::pair<std::string, std::string>> columns, std::function<Object*(char**)> constructor);

        void ClearDatabase();
        void SaveObjectsData(const std::string& className);
        void LoadObjectsData(const std::string& className);

        void InsertData(const std::string &table, const std::string &columns, const std::string &values);
        int GetLastInsertID();

        void CloseDatabase();

    };
}

#endif // COSMIC_SQLMANAGER_HPP
