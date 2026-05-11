#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <sqlite/sqlite3.h>
#include <vector>

namespace WandEngine
{

    class GameObject;

    class DataManager
    {
    private:
        DataManager();
        ~DataManager();
        DataManager(const DataManager &) = delete;
        DataManager &operator=(const DataManager &) = delete;

        sqlite3 *db;

    public:
        static DataManager &GetInstance()
        {
            static DataManager instance;
            return instance;
        }

        void CreateTable();
        void ClearGameObjects();
        void SaveData(const GameObject &obj);
        std::vector<GameObject> LoadData();
    };
}

#endif // DATAMANAGER_HPP
