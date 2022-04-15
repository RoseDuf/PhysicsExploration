#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

using namespace std;

class Model
{
    public: 
        inline Model(const char* path)
        {
            loadModel(path);
        }
        void Draw(Shader& shader);
    private:
        // model data
        vector<Mesh> meshes;
        string directory;
        vector<Texture> textures_loaded;

        void loadModel(string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
        Material loadMaterial(aiMaterial* mat);
        unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};

#endif