#pragma once

#include <string>
#include <vector>
#include "glm.hpp"
#include "Shader.hpp"
#include <map>
#include "Texture.hpp"
#include "fbxsdk.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags


#define MAX_NUM_BONES_PER_VERTEX 4
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices )
#define INVALID_MATERIAL 0xFFFFFFFF

class AssimpMesh {
public:
	AssimpMesh(std::string ObjFilePath, std::string MtlFilePath, Shader* shader);
	~AssimpMesh() {}
	bool AssimpLoader(std::string FilePath, std::string ObjFileName);
    void Draw();

    void SetMeshPos(glm::vec3 pos) { mMeshPos = pos; }
    void SetMeshRotate(glm::mat4 rot) { mMeshRotate = rot; }
    void SetMeshScale(float scale) { mMeshScale = scale; }



private:
    struct BasicMeshEntry {
        BasicMeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    struct Material {
        glm::vec3 AmbientColor;
        glm::vec3 DiffuseColor;
        glm::vec3 SpecColor;
        float SpecPower;
        float Alpha;
        glm::vec3 Reflection;
        glm::vec3 Emissive;
        glm::vec3 Bump;
        glm::vec3 NormalMap;
        Texture* DiffuseTexture;
    };

    struct VertexBoneData
    {
        unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
        float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

        VertexBoneData()
        {
        }

        void AddBoneData(unsigned int BoneID, float Weight)
        {
            for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
                if ((BoneIDs[i] == BoneID) && (Weights[i] != 0.0)) { // すでに所持していたら追加しない
                    return;
                }
                if (Weights[i] == 0.0) {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, i);
                    return;
                }
            }

            // should never get here - more bones than we have space for
            assert(0);
        }
    };

    struct BoneInfo
    {
        glm::mat4 OffsetMatrix;
        glm::mat4 FinalTransformation;

        BoneInfo(const glm::mat4& Offset)
        {
            OffsetMatrix = Offset;
            FinalTransformation = glm::mat4(0.0f);
        }
    };

    void SetMeshTransforms();

    // 時刻が変化するにしたがってBoneの行列を更新する
    void UpdateBoneTransform(float TimeInSeconds);
    // 時刻TimeInSecondsにおける各ボーンのTransformを求める
    void GetBoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms);
    // Nodeの階層構造を読みだす
    void ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform);
    // AnimationTimeTicks時刻におけるAnimationを求める
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);


    unsigned int mVertexArray;
    Shader* mShader;

    const aiScene* m_pScene;
    Assimp::Importer m_Importer;    // Importer保持せんかったら、Sceneも保持できない!!

    std::vector<BasicMeshEntry> m_Meshes;
    std::vector<Material> m_Materials;
    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<unsigned int> m_Indices;
    std::map<std::string, unsigned int> m_BoneNameToIndexMap;
    std::vector<VertexBoneData> m_Bones;
    std::vector<BoneInfo> m_BoneInfo;

    glm::mat4 m_GlobalInverseTransform;


    glm::vec3 mMeshPos;
    glm::mat4 mMeshRotate;
    float mMeshScale;

    std::string ObjFileRoot;
    std::string ObjFileName;
};