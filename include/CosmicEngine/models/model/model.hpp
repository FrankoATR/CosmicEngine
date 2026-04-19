#ifndef COSMIC_MODEL_HPP
#define COSMIC_MODEL_HPP

/**
 * @file model.hpp
 * @brief Declares the 3D model loader used by the engine renderer.
 */

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include "../shader/shader.hpp"
#include "../mesh/mesh.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

namespace CosmicEngine
{
    /**
     * @brief Loads a 3D model file and exposes its meshes for rendering.
     *
     * Model uses the Assimp library to import common 3D formats (OBJ, FBX, glTF,
     * etc.) and converts them into Mesh instances ready for GPU rendering.  Models
     * are typically loaded through the ResourceManager and drawn with a Shader.
     *
     * @par Example — loading and drawing a model (internal to ResourceManager)
     * @code
     * auto* model = new CosmicEngine::Model("assets/models/house.obj");
     * // Inside a draw loop:
     * shader->Use();
     * model->draw(*shader);
     * shader->EndUse();
     * @endcode
     */
    class Model
    {
    public:
        /** @brief Cache of textures already loaded while parsing the model. */
        vector<Texture> textures_loaded;
        /** @brief Meshes extracted from the loaded model. */
        vector<Mesh> meshes;
        /** @brief Directory containing the source model file. */
        string directory;
        /** @brief Indicates whether gamma correction should be applied to loaded textures. */
        bool gammaCorrection;

        /**
         * @brief Loads a model from disk.
         * @param path Source model file path.
         * @param gamma True to enable gamma-correct texture loading.
         */
        Model(string const &path, bool gamma = false);

        /**
         * @brief Draws every mesh in the loaded model.
         * @param shader Shader used for rendering.
         */
        void draw(Shader &shader);

    private:
        void loadModel(string const &path);

        void processNode(aiNode *node, const aiScene *scene);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
        unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
    };

}

#endif // COSMIC_MODEL_HPP