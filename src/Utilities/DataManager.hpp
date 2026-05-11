#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <string>
#include <fstream>
#include <Windows.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace WandEngine
{
    class DataManager
    {
    private:
        DataManager();
        ~DataManager();
        DataManager(const DataManager &) = delete;
        DataManager &operator=(const DataManager &) = delete;

        static bool EnsureDirectoryExists(const std::wstring &dirPath);
        std::wstring GetExecutablePath();

    public:
        static DataManager &GetInstance()
        {
            static DataManager instance;
            return instance;
        }

        void LoadData(int &PositionX, int &PositionY);
        void SaveData(int PositionX, int PositionY);
    };
}

#endif // DATAMANAGER_HPP
