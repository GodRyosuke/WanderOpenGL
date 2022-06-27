#include "AssimpMesh.hpp"
#include <iostream>

AssimpMesh::AssimpMesh(std::string FilePath, std::string ObjFileName, Shader* shader)
{
    if (!AssimpLoader(FilePath, ObjFileName)) {
        std::cout << "Failed to Load FBX File\n";
        return;
    }
}

bool AssimpMesh::AssimpLoader(std::string FilePath, std::string ObjFileName)
{
    std::string FilePath = FilePath + ObjFileName;

    Assimp::Importer pImporter;
    const aiScene* pScene = pImporter.ReadFile(FilePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (pScene) {
        m_Meshes.resize(pScene->mNumMeshes);
        m_Materials.resize(pScene->mNumMaterials);

        unsigned int NumVertices = 0;
        unsigned int NumIndices = 0;


        CountVerticesAndIndices(pScene, NumVertices, NumIndices);

        ReserveSpace(NumVertices, NumIndices);

        InitAllMeshes(pScene);

        if (!InitMaterials(pScene, Filename)) {
            return false;
        }

        PopulateBuffers();

        return GLCheckError();
    }
    else {
        printf("Error parsing '%s': '%s'\n", FilePath.c_str(), pImporter.GetErrorString());
    }

	return true;
}