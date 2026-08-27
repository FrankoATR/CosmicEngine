#ifndef NABAZU_MODELS_HPP
#define NABAZU_MODELS_HPP

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <glm/glm.hpp>

#include <array>

namespace NaBazu
{
    // VERIFIED by instrumenting CosmicEngine's Model loader and logging the loaded
    // bounding box of every ship: the engine NORMALISES models on load -- each one
    // comes out centred exactly on (0,0,0) with its largest dimension scaled to 1.0:
    //
    //   RedFighter          centre(0,0,0)  size(1.000, 0.275, 0.850)
    //   DualStriker         centre(0,0,0)  size(0.744, 0.178, 1.000)
    //   InterstellarRunner  centre(0,0,0)  size(0.607, 0.286, 1.000)
    //   Warship             centre(0,0,0)  size(0.453, 0.267, 1.000)
    //
    // Two consequences, both of which previously produced visible bugs:
    //  * No centring correction is needed or wanted. Subtracting the raw .obj's
    //    corner-origin offset (the files are authored 0..8 on X, -6.8..0 on Z) pushed
    //    every ship tens of units away from its own hitbox.
    //  * The `size` passed to Render3DModel* IS the finished world size of the
    //    model's largest dimension. Dividing by the raw .obj length made everything
    //    render ~7x too small.
    struct ShipModelInfo
    {
        const char *key;
        const char *path;
    };

    // These ships are authored nose-along +Z, while the whole game (rail frames, yaw
    // derivation, bullet bolts) uses -Z as forward. Without this offset every craft
    // renders flying tail-first. Applied at draw time only, so gameplay maths keeps
    // using the engine-wide -Z convention untouched.
    inline constexpr float kModelYawOffsetDeg = 180.0f;

    inline const ShipModelInfo &GetPlayerModel()
    {
        static const ShipModelInfo player{
            "nabazu.model.red_fighter",
            "assets/models/RedFighter/Package/RedFighter.obj"};
        return player;
    }

    // Enemy waves pick one of these at random; every enemy in a wave shares it.
    inline const std::array<ShipModelInfo, 3> &GetEnemyModels()
    {
        static const std::array<ShipModelInfo, 3> models{{
            {"nabazu.model.dual_striker",
             "assets/models/DualStriker/Package/DualStriker.obj"},
            {"nabazu.model.interstellar_runner",
             "assets/models/Interstellar Runner/Package/InterstellarRunner.obj"},
            {"nabazu.model.warship",
             "assets/models/Warship/Package/Warship.obj"}
        }};
        return models;
    }

    inline const ShipModelInfo &GetEnemyModel(int index)
    {
        const auto &models = GetEnemyModels();
        const int count = static_cast<int>(models.size());
        const int safeIndex = ((index % count) + count) % count;
        return models[safeIndex];
    }

    inline void LoadAllShipModels()
    {
        RS_MN.Load3DModel(GetPlayerModel().key, GetPlayerModel().path);
        for (const ShipModelInfo &model : GetEnemyModels())
        {
            RS_MN.Load3DModel(model.key, model.path);
        }
    }

    // worldSize is the world-space span of the model's largest dimension (see the
    // normalisation note above). Unlit on purpose: the project configures no lights,
    // so the lit shader would render these nearly black -- unlit shows the .mtl
    // palette texture as authored.
    inline void DrawShipModel(const ShipModelInfo &info, glm::vec3 position, float worldSize,
                               glm::vec3 rotationDeg, glm::vec3 color = glm::vec3(1.0f), float alpha = 1.0f)
    {
        RS_MN.Render3DModelUnlit(info.key, position, glm::vec3(worldSize), rotationDeg,
                                  color, alpha, CosmicEngine::ViewType::Projection);
    }
}

#endif
