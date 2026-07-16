#include "level_data.hpp"

#include "class_ids.hpp"
#include "../utilities/paths.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace PuzzleRPG
{
    namespace
    {
        void SortTileLayers(TileCell &cell)
        {
            std::sort(cell.layers.begin(), cell.layers.end(),
                [](const TileLayer &left, const TileLayer &right)
                {
                    if (left.drawLayer != right.drawLayer) return left.drawLayer < right.drawLayer;
                    return left.placementLayer < right.placementLayer;
                });
        }
    }

    TileCell LevelData::TileAt(int x, int y) const
    {
        if (x < 0 || y < 0 || y >= static_cast<int>(tiles.size())) return TileCell{};
        const auto &row = tiles[static_cast<std::size_t>(y)];
        if (x >= static_cast<int>(row.size())) return TileCell{};
        return row[static_cast<std::size_t>(x)];
    }

    void LevelData::SetTile(int x, int y, TileCell cell)
    {
        if (x < 0 || y < 0 || y >= static_cast<int>(tiles.size())) return;
        auto &row = tiles[static_cast<std::size_t>(y)];
        if (x >= static_cast<int>(row.size())) return;
        SortTileLayers(cell);
        row[static_cast<std::size_t>(x)] = cell;
    }

    TileLayer *LevelData::FindTileLayer(int x, int y, int placementLayer)
    {
        if (x < 0 || y < 0 || y >= static_cast<int>(tiles.size())) return nullptr;
        auto &row = tiles[static_cast<std::size_t>(y)];
        if (x >= static_cast<int>(row.size())) return nullptr;
        auto &layers = row[static_cast<std::size_t>(x)].layers;
        auto it = std::find_if(layers.begin(), layers.end(),
            [placementLayer](const TileLayer &layer)
            {
                return layer.placementLayer == placementLayer;
            });
        return it != layers.end() ? &(*it) : nullptr;
    }

    const TileLayer *LevelData::FindTileLayer(int x, int y, int placementLayer) const
    {
        if (x < 0 || y < 0 || y >= static_cast<int>(tiles.size())) return nullptr;
        const auto &row = tiles[static_cast<std::size_t>(y)];
        if (x >= static_cast<int>(row.size())) return nullptr;
        const auto &layers = row[static_cast<std::size_t>(x)].layers;
        auto it = std::find_if(layers.begin(), layers.end(),
            [placementLayer](const TileLayer &layer)
            {
                return layer.placementLayer == placementLayer;
            });
        return it != layers.end() ? &(*it) : nullptr;
    }

    void LevelData::SetTileLayer(int x, int y, TileLayer layer)
    {
        if (x < 0 || y < 0 || y >= static_cast<int>(tiles.size())) return;
        auto &row = tiles[static_cast<std::size_t>(y)];
        if (x >= static_cast<int>(row.size())) return;

        layer.placementLayer = std::max(0, layer.placementLayer);
        layer.drawLayer      = std::max(0, layer.drawLayer);

        auto &cell = row[static_cast<std::size_t>(x)];
        auto it = std::find_if(cell.layers.begin(), cell.layers.end(),
            [&layer](const TileLayer &existing)
            {
                return existing.placementLayer == layer.placementLayer;
            });
        if (it != cell.layers.end()) *it = layer;
        else                         cell.layers.push_back(layer);
        SortTileLayers(cell);
    }

    bool LevelData::RemoveTileLayer(int x, int y, int placementLayer)
    {
        if (x < 0 || y < 0 || y >= static_cast<int>(tiles.size())) return false;
        auto &row = tiles[static_cast<std::size_t>(y)];
        if (x >= static_cast<int>(row.size())) return false;
        auto &cell = row[static_cast<std::size_t>(x)];
        auto before = cell.layers.size();
        cell.layers.erase(std::remove_if(cell.layers.begin(), cell.layers.end(),
            [placementLayer](const TileLayer &layer)
            {
                return layer.placementLayer == placementLayer;
            }),
            cell.layers.end());
        return before != cell.layers.size();
    }

    bool LevelData::HasAnyTile(int x, int y) const
    {
        const TileCell cell = TileAt(x, y);
        return !cell.layers.empty();
    }

    bool LevelData::HasWallTile(int x, int y) const
    {
        const TileCell cell = TileAt(x, y);
        return std::any_of(cell.layers.begin(), cell.layers.end(),
            [](const TileLayer &layer)
            {
                return IsWallSheet(layer.sheet);
            });
    }

    LevelObject *LevelData::FindObject(int id)
    {
        for (auto &object : objects)
        {
            if (object.id == id) return &object;
        }
        return nullptr;
    }

    const LevelObject *LevelData::FindObject(int id) const
    {
        for (const auto &object : objects)
        {
            if (object.id == id) return &object;
        }
        return nullptr;
    }

    int LevelData::NextObjectId() const
    {
        int maxId = 0;
        for (const auto &object : objects)
        {
            if (object.id > maxId) maxId = object.id;
        }
        return maxId + 1;
    }

    nlohmann::json SerializeLevel(const LevelData &level)
    {
        nlohmann::json root;
        root["schemaVersion"] = 4;
        root["name"]   = level.name;
        root["width"]  = level.width;
        root["height"] = level.height;
        root["background"] = {
            {"r", level.backgroundColor.r},
            {"g", level.backgroundColor.g},
            {"b", level.backgroundColor.b}
        };

        // tiles[y][x] = { layers: [{ sheet, row, col, placementLayer, drawLayer }] }
        nlohmann::json tilesJson = nlohmann::json::array();
        for (const auto &rowVec : level.tiles)
        {
            nlohmann::json rowJson = nlohmann::json::array();
            for (const auto &cell : rowVec)
            {
                nlohmann::json cellJson;
                cellJson["layers"] = nlohmann::json::array();
                for (const auto &layer : cell.layers)
                {
                    cellJson["layers"].push_back({
                        {"sheet", layer.sheet},
                        {"row", layer.row},
                        {"col", layer.col},
                        {"placementLayer", layer.placementLayer},
                        {"drawLayer", layer.drawLayer}
                    });
                }
                rowJson.push_back(std::move(cellJson));
            }
            tilesJson.push_back(std::move(rowJson));
        }
        root["tiles"] = std::move(tilesJson);

        nlohmann::json spawnsJson = nlohmann::json::array();
        for (const auto &object : level.objects)
        {
            if (object.classId != Class_PlayerSpawn) continue;
            spawnsJson.push_back({{"x", object.x}, {"y", object.y}});
        }
        root["playerSpawns"] = spawnsJson;

        nlohmann::json objectsJson = nlohmann::json::array();
        for (const auto &object : level.objects)
        {
            nlohmann::json entry;
            entry["id"]        = object.id;
            entry["classId"]   = object.classId;
            entry["textureId"] = object.textureId;
            entry["x"]         = object.x;
            entry["y"]         = object.y;
            entry["targets"]   = object.targets;
            entry["activator"] = object.activator;
            entry["facing"]    = object.facing;
            entry["requiredItemTextureId"] = object.requiredItemTextureId;
            entry["pairId"]    = object.pairId;
            entry["spriteSheet"] = object.spriteSheet;
            entry["spriteRow"]   = object.spriteRow;
            entry["spriteCol"]   = object.spriteCol;
            entry["placementLayer"] = object.placementLayer;
            entry["drawLayer"]      = object.drawLayer;
            entry["dialog"]    = object.dialog;
            if (object.pixelX != 0.0f) entry["pixelX"] = object.pixelX;
            if (object.pixelY != 0.0f) entry["pixelY"] = object.pixelY;
            objectsJson.push_back(entry);
        }
        root["objects"] = objectsJson;
        root["winActivatorIds"] = level.winActivatorIds;
        return root;
    }

    bool DeserializeLevel(const nlohmann::json &data, LevelData &out)
    {
        if (!data.is_object()) return false;
        out.name   = data.value("name", std::string("Untitled"));
        out.width  = data.value("width", 16);
        out.height = data.value("height", 16);
        out.backgroundColor = LevelBackgroundColor{};
        if (data.contains("background") && data["background"].is_object())
        {
            const auto &background = data["background"];
            out.backgroundColor.r = std::clamp(background.value("r", out.backgroundColor.r), 0, 255);
            out.backgroundColor.g = std::clamp(background.value("g", out.backgroundColor.g), 0, 255);
            out.backgroundColor.b = std::clamp(background.value("b", out.backgroundColor.b), 0, 255);
        }

        out.tiles.clear();
        if (data.contains("tiles") && data["tiles"].is_array())
        {
            for (const auto &row : data["tiles"])
            {
                std::vector<TileCell> values;
                if (row.is_array())
                {
                    for (const auto &cell : row)
                    {
                        if (cell.is_object())
                        {
                            TileCell tc;
                            if (cell.contains("layers") && cell["layers"].is_array())
                            {
                                for (const auto &layerEntry : cell["layers"])
                                {
                                    TileLayer layer;
                                    layer.sheet = layerEntry.value("sheet", 0);
                                    layer.row   = layerEntry.value("row", 0);
                                    layer.col   = layerEntry.value("col", 0);
                                    layer.placementLayer = std::max(0, layerEntry.value("placementLayer",
                                        DefaultPlacementLayerForSheet(layer.sheet)));
                                    layer.drawLayer = std::max(0, layerEntry.value("drawLayer",
                                        DefaultDrawLayerForSheet(layer.sheet)));
                                    if (layer.sheet != SheetId::kNone) tc.layers.push_back(layer);
                                }
                                SortTileLayers(tc);
                            }
                            else
                            {
                                int sheet = cell.value("sheet", 0);
                                int rowValue = cell.value("row", 0);
                                int colValue = cell.value("col", 0);
                                int floorRow = cell.value("floorRow", rowValue);
                                int floorCol = cell.value("floorCol", colValue);

                                if (sheet == SheetId::kWalls1x1)
                                {
                                    if (cell.contains("floorRow") || cell.contains("floorCol"))
                                    {
                                        TileLayer floorLayer;
                                        floorLayer.sheet = SheetId::kGround;
                                        floorLayer.row = floorRow;
                                        floorLayer.col = floorCol;
                                        floorLayer.placementLayer = 0;
                                        floorLayer.drawLayer = DefaultDrawLayerForSheet(floorLayer.sheet);
                                        tc.layers.push_back(floorLayer);
                                    }

                                    TileLayer wallLayer;
                                    wallLayer.sheet = sheet;
                                    wallLayer.row = rowValue;
                                    wallLayer.col = colValue;
                                    wallLayer.placementLayer = 1;
                                    wallLayer.drawLayer = DefaultDrawLayerForSheet(wallLayer.sheet);
                                    tc.layers.push_back(wallLayer);
                                }
                                else if (sheet != SheetId::kNone)
                                {
                                    TileLayer layer;
                                    layer.sheet = sheet;
                                    layer.row   = rowValue;
                                    layer.col   = colValue;
                                    layer.placementLayer = DefaultPlacementLayerForSheet(sheet);
                                    layer.drawLayer      = DefaultDrawLayerForSheet(sheet);
                                    tc.layers.push_back(layer);
                                }
                                SortTileLayers(tc);
                            }
                            values.push_back(tc);
                        }
                        else if (cell.is_number_integer())
                        {
                            // Back-compat: formato antiguo (int por celda).
                            //  0 -> muro (walls1x1, (0,0)). !=0 -> suelo (ground, (0,0)).
                            int legacy = cell.get<int>();
                            TileCell tc;
                            TileLayer layer;
                            layer.sheet = (legacy == 0) ? SheetId::kWalls1x1 : SheetId::kGround;
                            layer.row   = 0;
                            layer.col   = 0;
                            layer.placementLayer = DefaultPlacementLayerForSheet(layer.sheet);
                            layer.drawLayer      = DefaultDrawLayerForSheet(layer.sheet);
                            tc.layers.push_back(layer);
                            values.push_back(tc);
                        }
                        else
                        {
                            values.push_back(TileCell{});
                        }
                    }
                }
                out.tiles.push_back(std::move(values));
            }
        }
        // Asegura que el grid tenga las dimensiones declaradas.
        out.tiles.resize(static_cast<std::size_t>(out.height));
        for (auto &row : out.tiles) row.resize(static_cast<std::size_t>(out.width), TileCell{});

        out.objects.clear();
        if (data.contains("objects") && data["objects"].is_array())
        {
            for (const auto &entry : data["objects"])
            {
                LevelObject object;
                object.id        = entry.value("id", 0);
                object.classId   = entry.value("classId", 0);
                object.textureId = entry.value("textureId", 0);
                object.x         = entry.value("x", 0);
                object.y         = entry.value("y", 0);
                object.activator = entry.value("activator", static_cast<int>(Plate_Both));
                object.facing    = entry.value("facing", 0);
                object.requiredItemTextureId = entry.value("requiredItemTextureId", 0);
                object.pairId    = entry.value("pairId", 0);
                object.spriteSheet = entry.value("spriteSheet", 0);
                object.spriteRow   = entry.value("spriteRow",   0);
                object.spriteCol   = entry.value("spriteCol",   0);
                object.placementLayer = std::max(0, entry.value("placementLayer",
                    DefaultPlacementLayerForClass(object.classId)));
                object.drawLayer = std::max(0, entry.value("drawLayer",
                    DefaultDrawLayerForClass(object.classId)));
                object.dialog    = entry.value("dialog", std::string());
                object.pixelX    = entry.value("pixelX", 0.0f);
                object.pixelY    = entry.value("pixelY", 0.0f);
                if (entry.contains("targets") && entry["targets"].is_array())
                {
                    for (const auto &target : entry["targets"]) object.targets.push_back(target.get<int>());
                }
                out.objects.push_back(std::move(object));
            }
        }

        out.playerSpawns.clear();
        bool hasSpawnObjects = false;
        for (const auto &object : out.objects)
        {
            if (object.classId != Class_PlayerSpawn) continue;
            hasSpawnObjects = true;
            out.playerSpawns.emplace_back(object.x, object.y);
        }

        if (!hasSpawnObjects && data.contains("playerSpawns") && data["playerSpawns"].is_array())
        {
            for (const auto &spawn : data["playerSpawns"])
            {
                LevelObject object;
                object.id        = out.NextObjectId();
                object.classId   = Class_PlayerSpawn;
                object.textureId = DefaultTextureId::kPlayerDown;
                object.x         = spawn.value("x", 0);
                object.y         = spawn.value("y", 0);
                object.placementLayer = DefaultPlacementLayerForClass(object.classId);
                object.drawLayer      = DefaultDrawLayerForClass(object.classId);
                out.objects.push_back(object);
                out.playerSpawns.emplace_back(object.x, object.y);
            }
        }

        out.winActivatorIds.clear();
        if (data.contains("winActivatorIds") && data["winActivatorIds"].is_array())
        {
            for (const auto &id : data["winActivatorIds"]) out.winActivatorIds.push_back(id.get<int>());
        }
        return true;
    }

    std::string LevelsDirectory()
    {
        return PUZZLERPG_LEVELS_DIR;
    }

    std::string LevelFilePath(const std::string &levelName)
    {
        return LevelsDirectory() + "/" + levelName + ".json";
    }

    static fs::path ToFsPath(const std::string &utf8)
    {
        return fs::path(std::u8string(utf8.begin(), utf8.end()));
    }

    bool SaveLevelToFile(const LevelData &level)
    {
        std::error_code error;
        fs::create_directories(LevelsDirectory(), error);
        std::ofstream file(ToFsPath(LevelFilePath(level.name)));
        if (!file.is_open()) return false;
        file << SerializeLevel(level).dump(2);
        return file.good();
    }

    bool LoadLevelFromFile(const std::string &levelName, LevelData &out)
    {
        std::ifstream file(ToFsPath(LevelFilePath(levelName)));
        if (!file.is_open()) return false;
        nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
        if (data.is_discarded()) return false;
        return DeserializeLevel(data, out);
    }

    std::vector<std::string> ListAvailableLevels()
    {
        std::vector<std::string> result;
        std::error_code error;
        fs::path directory(LevelsDirectory());
        if (!fs::exists(directory, error)) return result;
        for (const auto &entry : fs::directory_iterator(directory, error))
        {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() == ".json")
            {
                auto u8stem = entry.path().stem().u8string();
                result.push_back(std::string(u8stem.begin(), u8stem.end()));
            }
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    LevelData MakeEmptyLevel(const std::string &name, int width, int height)
    {
        LevelData level;
        level.name   = name;
        level.width  = std::max(4, width);
        level.height = std::max(4, height);
        level.backgroundColor = LevelBackgroundColor{};
        level.tiles.assign(static_cast<std::size_t>(level.height),
                           std::vector<TileCell>(static_cast<std::size_t>(level.width), TileCell{}));
        return level;
    }
}
