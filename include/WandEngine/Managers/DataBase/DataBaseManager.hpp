#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <iostream>
#include <sqlite/sqlite3.h>
#include <string>

namespace WandEngine
{
    class DataBaseManager
    {
    private:
        sqlite3 *db;
        std::string dbName;

        DataBaseManager();
        ~DataBaseManager();

        DataBaseManager(const DataBaseManager &) = delete;
        DataBaseManager &operator=(const DataBaseManager &) = delete;

    public:
        static DataBaseManager &GetInstance()
        {
            static DataBaseManager instance;
            return instance;
        }

        bool OpenDatabaseForSaving(const std::string &newDbName);
        bool OpenDatabaseForLoading(const std::string &newDbName);

        bool ExecuteSQL(const std::string &sql);
        void ExecuteQuery(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

        void CreateTable(const std::string &tableName, const std::string &columns);


        void ClearDatabase();
        void SaveObjectsData();
        void LoadObjectsData();

        void InsertData(const std::string &table, const std::string &columns, const std::string &values);
        int GetLastInsertID();
        void CloseDatabase();

    };
}

#endif // DATABASEMANAGER_HPP
