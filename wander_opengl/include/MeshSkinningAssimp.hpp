#pragma once

#include "MeshAssimp.hpp"

class MeshSkinningAssimp : public MeshAssimp {
public:
    MeshSkinningAssimp(Shader* shader);
    ~MeshSkinningAssimp(){}

private:
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
                //if ((BoneIDs[i] == BoneID) && (Weights[i] != 0.0)) { // すでに所持していたら追加しない
                //    return;
                //}
                if (Weights[i] == 0.0) {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    //printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, i);
                    return;
                }
            }

            // should never get here - more bones than we have space for
            //assert(0);
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

    virtual void ReserveVertexSpace() override;
    virtual void PopulateBuffers() override;
    virtual void LoadMesh(const aiMesh* pMesh, unsigned int meshIdx) override;
    virtual void GetGlobalInvTrans() override;


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

    virtual void UpdateTransform(float timeInSeconds) override;



    std::map<std::string, unsigned int> m_BoneNameToIndexMap;
    std::vector<VertexBoneData> m_Bones;
    std::vector<BoneInfo> m_BoneInfo;

    glm::mat4 m_GlobalInverseTransform;
};