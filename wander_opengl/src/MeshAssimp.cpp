#include "MeshAssimp.hpp"
#include <iostream>
#include "GLUtil.hpp"

MeshAssimp::MeshAssimp(std::string ObjFileRoot, std::string ObjFileName, Shader* shader)
    :ObjFileRoot(ObjFileRoot),
    ObjFileName(ObjFileName),
    mShader(shader)
{
    ticksCount = 0.0f;

    if (!AssimpLoader(ObjFileRoot, ObjFileName)) {
        std::cout << "Failed to Load FBX File\n";
        return;
    }
}

bool MeshAssimp::AssimpLoader(std::string RootPath, std::string ObjFileName)
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
                const aiVertexWeight& vw = paiBone->mWeights[weightIdx];
                unsigned int GlobalVertexID = m_Meshes[meshIdx].BaseVertex + paiBone->mWeights[weightIdx].mVertexId;
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
    GLuint m_boneBuffer;

    unsigned int VertexArray;

    mShader->UseProgram();
    glGenVertexArrays(1, &VertexArray);
    glBindVertexArray(VertexArray);

    glGenBuffers(NUM_BUFFERS, m_Buffers);

    // Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0])* m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal Map
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0])* m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UV
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0])* m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bone and Weights
    glGenBuffers(1, &m_boneBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_boneBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0])* m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
        (const GLvoid*)(MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));   // 後半4ByteがWeight


    // Indef Buffer
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

void MeshAssimp::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    assert(pNodeAnim->mNumRotationKeys > 0);

    // 現在のフレームのRtation Matrixを読みだす
    unsigned int RotationIndex = 0;
    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            RotationIndex = i;
            break;
        }
    }
    unsigned int NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    }
    else {
        float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
        float DeltaTime = t2 - t1;
        float Factor = (AnimationTimeTicks - t1) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = StartRotationQ;
    }

    Out.Normalize();
}

void MeshAssimp::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    assert(pNodeAnim->mNumScalingKeys > 0);

    // 現在のフレームのScaling Matrixを読みだす
    unsigned int ScalingIndex = 0;
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            ScalingIndex = i;
            break;
        }
    }

    unsigned int NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    }
    else {
        // 現在時刻がフレームとフレームの間であれば補完処理
        float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
        float DeltaTime = t2 - t1;
        float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
}

void MeshAssimp::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }


    // 現在のフレームのPosition Matrixを読みだす
    unsigned int PositionIndex = 0;
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            PositionIndex = i;
            break;
        }
    }
    unsigned int NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    }
    else {
        float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
        float DeltaTime = t2 - t1;
        float Factor = (AnimationTimeTicks - t1) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
}


void MeshAssimp::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform)
{
    GLUtil glutil;
    std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = m_pScene->mAnimations[0];

    // Nodeの持つTransform
    glm::mat4 NodeTransformation;
    {
        aiMatrix4x4 trans = pNode->mTransformation;
        NodeTransformation = glutil.ToGlmMat4(trans);
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
        glm::vec3 scaleVec = glm::vec3(Scaling.x, Scaling.y, Scaling.z);
        glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scaleVec);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
        //Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
        aiMatrix3x3 rotationMat = RotationQ.GetMatrix();
        glm::mat4 RotationM = glutil.ToGlmMat4(rotationMat);
        //std::cout << RotationM[0][0] << '\t' << RotationM[0][1] << '\t' << RotationM[0][2] << '\t' << RotationM[0][3] << '\t' << std::endl;

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
        glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), glm::vec3(Translation.x, Translation.y, Translation.z));

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
        unsigned int BoneIndex = m_BoneNameToIndexMap[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation);
    }
}

void MeshAssimp::GetBoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms)
{
    int num = m_pScene->mNumAnimations;
    auto k = m_pScene->mAnimations[0];
    float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != NULL ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float Duration = 0.0f;  // AnimationのDurationの整数部分が入る
    float fraction = modf((float)m_pScene->mAnimations[0]->mDuration, &Duration);
    float AnimationTimeTicks = fmod(TimeInTicks, Duration);



    glm::mat4 Identity = glm::mat4(1);
    // Nodeの階層構造にしたがって、AnimationTicks時刻における各BoneのTransformを求める
    ReadNodeHierarchy(AnimationTimeTicks, m_pScene->mRootNode, Identity);
    Transforms.resize(m_BoneInfo.size());

    for (unsigned int i = 0; i < m_BoneInfo.size(); i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

void MeshAssimp::UpdateBoneTransform(float TimeInSeconds)
{
    // 現在時刻のBone Transformを取得
    std::vector<glm::mat4> BoneMatrixPalete;
    GetBoneTransform(TimeInSeconds, BoneMatrixPalete);

    // Shaderに渡す
    for (int i = 0; i < BoneMatrixPalete.size(); i++) {
        std::string uniformName = "uMatrixPalette[" + std::to_string(i) + ']';
        mShader->SetMatrixUniform(uniformName, BoneMatrixPalete[i]);
    }
}

void MeshAssimp::SetMeshTransforms()
{
    glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(mMeshScale, mMeshScale, mMeshScale));
    glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0f), mMeshPos);
    glm::mat4 TransformMat = TranslateMat * mMeshRotate * ScaleMat;

    mShader->SetMatrixUniform("model", TransformMat);
}


void MeshAssimp::Draw(float timeInSeconds)
{
    mShader->UseProgram();
    UpdateBoneTransform(timeInSeconds);
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