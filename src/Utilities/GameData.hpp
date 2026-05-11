#ifndef GAMEDATA_HPP
#define GAMEDATA_HPP

#include "../Entities/LinkObject.hpp"
#include "../WandAllegroEngine/Interfaces/Definitions.hpp"
#include <vector>
#include <nlohmann/json.hpp>

namespace WandEngine
{
    struct GameData
    {
        std::string Type;
        std::vector<LinkObject* > Entities;

        GameData() : Type("LinkObject") {}
        GameData(const std::string &type, const std::vector<LinkObject* > &entities)
            : Type(type), Entities(entities) {}
    };

}

namespace nlohmann
{
    template <>
    struct adl_serializer<WandEngine::GameData>
    {
        static void to_json(json& j, const WandEngine::GameData& gameData)
        {
            j = json{
                {"Type", gameData.Type},
                //{"Entities", gameData.Entities}
                };
        }

        static void from_json(const json& j, WandEngine::GameData gameData)
        {
            j.at("Type").get_to(gameData.Type);
            //j.at("Entities").get_to(gameData.Entities);
        }
    };
}

#endif // GAMEDATA_HPP
