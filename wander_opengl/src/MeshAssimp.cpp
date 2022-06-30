#include "MeshAssimp.hpp"
#include <iostream>
#include "GLUtil.hpp"

AssimpMesh::AssimpMesh(std::string ObjFileRoot, std::string ObjFileName, Shader* shader)
    :ObjFileRoot(ObjFileRoot),
    ObjFileName(ObjFileName),
    mShader(shader)
{
    if (!AssimpLoader(ObjFileRoot, ObjFileName)) {
        std::cout << "Failed to Load FBX File\n";
        return;
    }
}

bool AssimpMesh::AssimpLoader(std::string RootPath, std::string ObjFileName)
{
    std::string FilePath = RootPath + ObjFileName;

    m_pScene = m_Importer.ReadFile(FilePath.c_str(), ASSIMP_LOAD_FLAGS);
    //m_pScene = pScene;

    if (!m_pScene) {
        printf("Error parsing '%s': '%s'\n", FilePath.c_str(), m_Importer.GetErrorString());
        return false;
    }

    {
        GLUtil glutil;
        aiMatrix4x4 globalTransform = m_pScene->mRootNode->mTransformation;
        m_GlobalInverseTransform = glm::inverse(glutil.ToGlmMat4(globalTransform));
    }


    m_Meshes.resize(m_pScene->mNumMeshes);
    m_Materials.resize(m_pScene->mNumMaterials);

    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;

    // 頂点の総数などを読み込む
    for (unsigned int i = 0; i < m_Meshes.size(); i++) {
        m_Meshes[i].MaterialIndex = m_pScene->mMeshes[i]->mMaterialIndex; // MeshとMaterialの紐づけ
        m_Meshes[i].NumIndices = m_pScene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].BaseVertex = NumVertices;
        m_Meshes[i].BaseIndex = NumIndices;

        NumVertices += m_pScene->mMeshes[i]->mNumVertices;
        NumIndices += m_Meshes[i].NumIndices;
    }

    m_Positions.reserve(NumVertices);
    m_Normals.reserve(NumVertices);
    m_TexCoords.reserve(NumVertices);
    m_Indices.reserve(NumIndices);
    m_Bones.resize(NumVertices);

    // Mesh(頂点情報など)の読み込み
    for (int meshIdx = 0; meshIdx < m_Meshes.size(); meshIdx++) {
        const aiMesh* paiMesh = m_pScene->mMeshes[meshIdx];
        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

        // Vertex, Normal, UV取得
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

        // Index 情報取得
        for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
            const aiFace& Face = paiMesh->mFaces[i];
            //printf("num indices %d\n", Face.mNumIndices);
            assert(Face.mNumIndices == 3);
            m_Indices.push_back(Face.mIndices[0]);
            m_Indices.push_back(Face.mIndices[1]);
            m_Indices.push_back(Face.mIndices[2]);
        }

        // Boneの読み込み
        for (unsigned int i = 0; i < paiMesh->mNumBones; i++) {
            aiBone* paiBone = paiMesh->mBones[i];

            // BoneIndexの取得
            int BoneIndex = 0;
            std::string BoneName = paiBone->mName.C_Str(); 
            if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end()) {
                // Allocate an index for a new bone
                BoneIndex = (int)m_BoneNameToIndexMap.size();
                m_BoneNameToIndexMap[BoneName] = BoneIndex;
            }
            else {
                BoneIndex = m_BoneNameToIndexMap[BoneName];
            }

            if (BoneIndex == m_BoneInfo.size()) {
                GLUtil glutil;
                aiMatrix4x4 offsetMatrix = paiBone->mOffsetMatrix;
                BoneInfo bi(glutil.ToGlmMat4(offsetMatrix));
                m_BoneInfo.push_back(bi);
            }

            // BoneのWeightを取得
            for (int weightIdx = 0; weightIdx < paiBone->mNumWeights; weightIdx++) {
                const aiVertexWeight& vw = paiBone->mWeights[i];
                unsigned int GlobalVertexID = m_Meshes[meshIdx].BaseVertex + paiBone->mWeights[i].mVertexId;
                //printf("vertexID:%d, BoneID:%d, weight: %f\n", GlobalVertexID, BoneIndex, vw.mWeight);
                m_Bones[GlobalVertexID].AddBoneData(BoneIndex, vw.mWeight);
            }
        }
    }


    // MaterialとTexture読み込み
    printf("Num materials: %d\n", m_pScene->mNumMaterials);

    // Materialの読み込み
    for (int materialIdx = 0; materialIdx < m_pScene->mNumMaterials; materialIdx++) {
        const aiMaterial* pMaterial = m_pScene->mMaterials[materialIdx];

        // Diffuse Textureを読み込む
        m_Materials[materialIdx].DiffuseTexture = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string texturePathData = Path.data;

                // 読み込んだTextureのPathの分解処理
                GLUtil glutil;
                char buffer[512];
                memset(buffer, 0, 512 * sizeof(char));
                memcpy(buffer, texturePathData.c_str(), sizeof(char) * 512);
                glutil.Replace('\\', '/', buffer);
                std::vector<std::string> split_list;
                std::string replace_file_name = buffer;
                // 「/」で分解
                glutil.Split('/', buffer, split_list);

                std::string texturePath = ObjFileRoot + "Textures/" + split_list[split_list.size() - 1];
                m_Materials[materialIdx].DiffuseTexture = new Texture(texturePath);
            }
        }

        // Diffuse Specular Ambientの読み込み
        aiColor3D AmbientColor(0.0f, 0.0f, 0.0f);
        glm::vec3 AllOnes = glm::vec3(1.0f, 1.0f, 1.0f);

        int ShadingModel = 0;
        if (pMaterial->Get(AI_MATKEY_SHADING_MODEL, ShadingModel) == AI_SUCCESS) {
            printf("Shading model %d\n", ShadingModel);
        }

        if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS) {
            printf("Loaded ambient color [%f %f %f]\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);
            m_Materials[materialIdx].AmbientColor.r = AmbientColor.r;
            m_Materials[materialIdx].AmbientColor.g = AmbientColor.g;
            m_Materials[materialIdx].AmbientColor.b = AmbientColor.b;
        }
        else {
            m_Materials[materialIdx].AmbientColor = AllOnes;
        }

        aiColor3D DiffuseColor(0.0f, 0.0f, 0.0f);

        if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS) {
            printf("Loaded diffuse color [%f %f %f]\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
            m_Materials[materialIdx].DiffuseColor.r = DiffuseColor.r;
            m_Materials[materialIdx].DiffuseColor.g = DiffuseColor.g;
            m_Materials[materialIdx].DiffuseColor.b = DiffuseColor.b;
        }

        aiColor3D SpecularColor(0.0f, 0.0f, 0.0f);

        if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS) {
            printf("Loaded specular color [%f %f %f]\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);
            m_Materials[materialIdx].SpecColor.r = SpecularColor.r;
            m_Materials[materialIdx].SpecColor.g = SpecularColor.g;
            m_Materials[materialIdx].SpecColor.b = SpecularColor.b;
        }
    }

    // Vertex Array Object作成
    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        POS_VB       = 1,
        TEXCOORD_VB  = 2,
        NORMAL_VB    = 3,
        NUM_BUFFERS  = 4,  // required only for instancing
    };
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };

    unsigned int VertexArray;

    mShader->UseProgram();
    glGenVertexArrays(1, &VertexArray);
    glBindVertexArray(VertexArray);

    glGenBuffers(NUM_BUFFERS, m_Buffers);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0])* m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0])* m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0])* m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0])* m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

    // unbind cube vertex arrays
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mVertexArray = VertexArray;


    bool checkErr = (glGetError() == GL_NO_ERROR);
    return checkErr;
}


void AssimpMesh::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform)
{
    GLUtil glutil;
    std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = m_pScene->mAnimations[0];

    // Nodeの持つTransform
    glm::mat4 NodeTransformation;
    {
        aiMatrix4x4 trans = pNode->mTransformation;
        glm::mat4 NodeTransformation = glutil.ToGlmMat4(trans);
    }

    // 現在のNodeのAnimation Dataを読みだす
    const aiNodeAnim* pNodeAnim = NULL;
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim* nodeAnim = pAnimation->mChannels[i];

        if (std::string(nodeAnim->mNodeName.data) == NodeName) {
            pNodeAnim = nodeAnim;
        }
    }

    // そのNodeにAnimationがあれば、
    if (pNodeAnim) {
        // 現在時刻のAnimation Transformをかけあわせる。
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
        uint BoneIndex = m_BoneNameToIndexMap[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
    }

    for (uint i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation);
    }
}

void AssimpMesh::GetBoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms)
{
    int num = m_pScene->mNumAnimations;
    auto k = m_pScene->mAnimations[0];
    float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != NULL ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float Duration = 0.0f;  // AnimationのDurationの整数部分が入る
    float fraction = modf((float)m_pScene->mAnimations[0]->mDuration, &Duration);
    float AnimationTimeTicks = fmod(TimeInTicks, Duration);



    glm::mat4 Identity = glm::mat4(1);
    ReadNodeHierarchy(AnimationTimeTicks, m_pScene->mRootNode, Identity);
    Transforms.resize(m_BoneInfo.size());

    for (unsigned int i = 0; i < m_BoneInfo.size(); i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }

    int x = 0;
}

void AssimpMesh::SetMeshTransforms()
{
    glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(mMeshScale, mMeshScale, mMeshScale));
    glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0f), mMeshPos);
    glm::mat4 TransformMat = TranslateMat * mMeshRotate * ScaleMat;

    mShader->SetMatrixUniform("model", TransformMat);
}

void AssimpMesh::Draw()
{
    std::vector<glm::mat4>temp;
    GetBoneTransform(1.0, temp);
    mShader->UseProgram();
    SetMeshTransforms();
    glBindVertexArray(mVertexArray);

    for (unsigned int i = 0; i < m_Meshes.size(); i++) {
        unsigned int MaterialIndex = m_Meshes[i].MaterialIndex;
        assert(MaterialIndex < m_Materials.size());

        mShader->SetVectorUniform("uAmbientLight", m_Materials[MaterialIndex].AmbientColor);
        mShader->SetVectorUniform("uDirLight.mDirection", glm::vec3(0, -0.707, -0.707));
        mShader->SetVectorUniform("uDirLight.mDiffuseColor", m_Materials[MaterialIndex].DiffuseColor);
        mShader->SetVectorUniform("uDirLight.mSpecColor", m_Materials[MaterialIndex].SpecColor);
        mShader->SetFloatUniform("uSpecPower", 0.3f);

        if (m_Materials[MaterialIndex].DiffuseTexture) {
            m_Materials[MaterialIndex].DiffuseTexture->BindTexture();
        }

        //if (m_Materials[MaterialIndex].pSpecularExponent) {
        //    m_Materials[MaterialIndex].pSpecularExponent->Bind(SPECULAR_EXPONENT_UNIT);

        //    if (pRenderCallbacks) {
        //        pRenderCallbacks->ControlSpecularExponent(true);
        //    }
        //}
        //else {
        //    if (pRenderCallbacks) {
        //        pRenderCallbacks->ControlSpecularExponent(false);
        //    }
        //}

        //if (pRenderCallbacks) {
        //    pRenderCallbacks->DrawStartCB(i);
        //    pRenderCallbacks->SetMaterial(m_Materials[MaterialIndex]);
        //}

        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Meshes[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex),
            m_Meshes[i].BaseVertex);
    }

    glBindVertexArray(0);
}