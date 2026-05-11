#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <iostream>
#include <sqlite/sqlite3.h>
#include <string>
#include <unordered_map>
#include <functional>

namespace WandEngine
{
    class GameObject;

    struct SerializedObjects
    {
        std::vector<std::pair<std::string, std::string>> Columns;
        std::function<GameObject*(char**)> Constructor;
    };

    class DataBaseManager
    {
    private:
        DataBaseManager();
        ~DataBaseManager();
        DataBaseManager(const DataBaseManager &) = delete;
        DataBaseManager &operator=(const DataBaseManager &) = delete;

        sqlite3 *db;
        std::string dbName;
        std::unordered_map<std::string, SerializedObjects> serialization_resources;


    public:
        static DataBaseManager &GetInstance();

        void Init();
        void Shutdown();

        bool OpenDatabase(const std::string &newDbName);

        bool ExecuteSQL(const std::string &sql);
        void ExecuteQuery(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

        void CreateTable(const std::string &tableName, const std::string &columns);
        void ConsultTable(const std::string &tableName, const std::string &columns, int (*callback)(void *, int, char **, char **), void* data = nullptr);


        void RegisterSerialization(const std::string& className, std::vector<std::pair<std::string, std::string>> columns, std::function<GameObject*(char**)> constructor);

        void ClearDatabase();
        void SaveObjectsData(const std::string& className);
        void LoadObjectsData(const std::string& className);

        void InsertData(const std::string &table, const std::string &columns, const std::string &values);
        int GetLastInsertID();

        void CloseDatabase();

    };
}

#endif // DATABASEMANAGER_HPP
