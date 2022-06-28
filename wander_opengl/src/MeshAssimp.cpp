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

        for (unsigned int i = 0; i < m_Meshes.size(); i++) {
            m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
            m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
            m_Meshes[i].BaseVertex = NumVertices;
            m_Meshes[i].BaseIndex = NumIndices;

            NumVertices += pScene->mMeshes[i]->mNumVertices;
            NumIndices += m_Meshes[i].NumIndices;
        }

        m_Positions.reserve(NumVertices);
        m_Normals.reserve(NumVertices);
        m_TexCoords.reserve(NumVertices);
        m_Indices.reserve(NumIndices);

        // Mesh‰Šú‰»ˆ—
        for (int meshIdx = 0; meshIdx < m_Meshes.size(); meshIdx++) {
            const aiMesh* paiMesh = pScene->mMeshes[meshIdx];
            const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

            // Vertex, Normal, UVæ“¾
            for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {

                const aiVector3D& pPos = paiMesh->mVertices[i];
                m_Positions.push_back(glm::vec3(pPos.x, pPos.y, pPos.z));

                if (paiMesh->mNormals) {
                    const aiVector3D& pNormal = paiMesh->mNormals[i];
                    m_Normals.push_back(glm::vec3(pNormal.x, pNormal.y, pNormal.z));
                }
                else {
                    aiVector3D Normal(0.0f, 1.0f, 0.0f);
                    m_Normals.push_back(glm::vec3(Normal.x, Normal.y, Normal.z));
                }

                const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
                m_TexCoords.push_back(glm::vec2(pTexCoord.x, pTexCoord.y));
            }

            // Index î•ñæ“¾
            for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
                const aiFace& Face = paiMesh->mFaces[i];
                //        printf("num indices %d\n", Face.mNumIndices);
                //        assert(Face.mNumIndices == 3);
                m_Indices.push_back(Face.mIndices[0]);
                m_Indices.push_back(Face.mIndices[1]);
                m_Indices.push_back(Face.mIndices[2]);
            }
        }


        // Material‚ÆTexture“Ç‚İ‚İ
        string Dir = GetDirFromFilename(Filename);

        bool Ret = true;

        printf("Num materials: %d\n", pScene->mNumMaterials);

        // Initialize the materials
        for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
            const aiMaterial* pMaterial = pScene->mMaterials[i];

            LoadTextures(Dir, pMaterial, i);

            LoadColors(pMaterial, i);
        }


        PopulateBuffers();

        return GLCheckError();
    }
    else {
        printf("Error parsing '%s': '%s'\n", FilePath.c_str(), pImporter.GetErrorString());
    }

	return true;
}