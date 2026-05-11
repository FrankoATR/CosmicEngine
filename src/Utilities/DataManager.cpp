#include "DataManager.hpp"
#include <Windows.h>
#include <ShlObj.h>
#include <iostream>

namespace WandEngine
{
    DataManager::DataManager()
    {
    }

    DataManager::~DataManager()
    {
    }

    bool DataManager::EnsureDirectoryExists(const std::wstring &dirPath)
    {
        // SHCreateDirectoryExW creates all intermediate directories as needed
        int result = SHCreateDirectoryExW(NULL, dirPath.c_str(), NULL);
        return result == ERROR_SUCCESS || result == ERROR_ALREADY_EXISTS;
    }

    std::wstring DataManager::GetExecutablePath()
    {
        wchar_t buffer[MAX_PATH];
        GetModuleFileNameW(NULL, buffer, MAX_PATH);
        std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
        return std::wstring(buffer).substr(0, pos + 1);
    }

    void DataManager::LoadData(int &PositionX, int &PositionY)
    {
        PositionX = 0;
        PositionY = 0.5f;

        PWSTR path = NULL;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &path);
        if (SUCCEEDED(hr) && path != NULL)
        {
            std::wstring savedGamesPath(path);
            CoTaskMemFree(path);

            std::wstring dataDirPath = GetExecutablePath() + L"\\saves";
            std::wstring jsonFilePath = dataDirPath + L"\\save.json";

            FILE *file = nullptr;
            if (_wfopen_s(&file, jsonFilePath.c_str(), L"rb") == 0 && file)
            {
                try
                {
                    // Obtener el tamaño del archivo
                    fseek(file, 0, SEEK_END);
                    long fileSize = ftell(file);
                    rewind(file);

                    // Leer el contenido del archivo en un std::string
                    std::string fileContents(fileSize, '\0');
                    size_t bytesRead = fread(&fileContents[0], 1, fileSize, file);
                    fclose(file);

                    if (bytesRead != fileSize)
                    {
                        std::cerr << "Error reading file." << std::endl;
                        _wremove(jsonFilePath.c_str());
                        return;
                    }

                    // Parsear el JSON desde fileContents
                    json j = json::parse(fileContents);

                    if (j.contains("PositionX"))
                    {
                        PositionX = j["PositionX"].get<int>();
                    }

                    if (j.contains("PositionY"))
                    {
                        PositionY = j["PositionY"].get<int>();
                    }
                }
                catch (const json::exception &e)
                {
                    std::cerr << "Error parsing JSON: " << e.what() << std::endl;
                    _wremove(jsonFilePath.c_str());
                }
            }
            else
            {
                // Archivo no existe o no se pudo abrir; usar valores por defecto
            }
        }
        else
        {
            std::cerr << "Failed to get Documents folder path." << std::endl;
        }
    }

    void DataManager::SaveData(int PositionX, int PositionY)
    {
        PWSTR path = NULL;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &path);
        if (SUCCEEDED(hr) && path != NULL)
        {
            std::wstring savedGamesPath(path);
            CoTaskMemFree(path);

            std::wstring dataDirPath = GetExecutablePath() + L"\\saves";
            std::wstring jsonFilePath = dataDirPath + L"\\save.json";

            // Asegurarse de que el directorio existe
            if (!EnsureDirectoryExists(dataDirPath))
            {
                std::cerr << "Failed to create directory." << std::endl;
                return;
            }

            json j;
            j["PositionX"] = PositionX;
            j["PositionY"] = PositionY;

            std::string jsonString = j.dump(4);

            FILE *file = nullptr;
            if (_wfopen_s(&file, jsonFilePath.c_str(), L"wb") == 0 && file)
            {
                size_t bytesWritten = fwrite(jsonString.c_str(), 1, jsonString.size(), file);
                fclose(file);

                if (bytesWritten != jsonString.size())
                {
                    std::cerr << "Error writing file." << std::endl;
                }
            }
            else
            {
                std::cerr << "Failed to open file for writing." << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to get Documents folder path." << std::endl;
        }
    }
}
