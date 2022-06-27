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
       aiMatrix4x4  m_GlobalInverseTransform = pScene->mRootNode->mTransformation;
        m_GlobalInverseTransform = m_GlobalInverseTransform.Inverse();
        Ret = InitFromScene(m_pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", FilePath.c_str(), pImporter.GetErrorString());
    }

	return true;
}