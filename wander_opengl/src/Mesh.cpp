#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.hpp"
#include "GLUtil.hpp"
#include <iostream>

Mesh::Mesh(std::string FilePath, std::string ObjFileName, Shader* shader, glm::vec3 LightDir)
	:mShader(shader),
	mVertices(0),
	mIndices(0),
	mLightDir(LightDir)
{

	if (!LoadObjFile(FilePath, ObjFileName)) {
		std::cout << "Failed to Load Mesh Obj File\n";
		return;
	}
	//if (!LoadMaterials(MtlFilePath)) {
	//	std::cout << "Failed to Load Material Obj File\n";
	//	return;
	//}

	mNumIndices = mIndices.size();


	mShader->UseProgram();
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(int), mIndices.data(), GL_STATIC_DRAW);

	// link attribution
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// unbind cube vertex arrays
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool Mesh::LoadObjFile(std::string FilePath, std::string ObjFileName)
{
	std::string ObjFilePath = FilePath + ObjFileName;
	std::string mtlFileName;

	// obj file��ǂ݂���
	FILE* file = fopen(ObjFilePath.c_str(), "r");
	if (file == NULL) {
		printf("error: failed to open mesh file\n");
		return false;
	}

	std::vector<float>CubeVertices;
	std::vector<float>CubeUV;
	std::vector<float>CubeNormals;
	std::vector<glm::ivec3>FaceVec;
	std::vector<glm::ivec3>RFaceVec(0);		// �d������index���������ꂽ�z��
	std::vector<ObjSubSet> SubSetDatas;

	bool IsFirstRead = true;
	ObjSubSet SubSetData;
	SubSetData.FaceCount = 0;
	while (1) {

		char lineHeader[128];
		// �s�̍ŏ��̕������ǂݍ��݂܂��B
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			SubSetDatas.push_back(SubSetData);
			break;
		}

		if (strcmp(lineHeader, "v") == 0) {
			char x_str[20];
			char y_str[20];
			char z_str[20];
			fscanf(file, "%s %s %s", x_str, y_str, z_str);
			CubeVertices.push_back(atof(x_str));
			CubeVertices.push_back(atof(y_str));
			CubeVertices.push_back(atof(z_str));
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			char x_str[20];
			char y_str[20];
			fscanf(file, "%s %s", x_str, y_str);
			CubeUV.push_back(atof(x_str));
			CubeUV.push_back(atof(y_str));
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			char x_str[20];
			char y_str[20];
			char z_str[20];
			fscanf(file, "%s %s %sn", x_str, y_str, z_str);
			CubeNormals.push_back(atof(x_str));
			CubeNormals.push_back(atof(y_str));
			CubeNormals.push_back(atof(z_str));
		}
		else if (strcmp(lineHeader, "f") == 0) {
			glm::ivec3 vertexIndex;
			glm::ivec3 uvIndex;
			glm::ivec3 normalIndex;
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&vertexIndex.x, &uvIndex.x, &normalIndex.x,
				&vertexIndex.y, &uvIndex.y, &normalIndex.y,
				&vertexIndex.z, &uvIndex.z, &normalIndex.z
			);
			glm::ivec3 a1, a2, a3;
			a1[0] = vertexIndex.x;	a1[1] = uvIndex.x;	a1[2] = normalIndex.x;
			a2[0] = vertexIndex.y;	a2[1] = uvIndex.y;	a2[2] = normalIndex.y;
			a3[0] = vertexIndex.z;	a3[1] = uvIndex.z;	a3[2] = normalIndex.z;
			FaceVec.push_back(a1);
			FaceVec.push_back(a2);
			FaceVec.push_back(a3);
			SubSetData.FaceCount++;
		}
		else if (strcmp(lineHeader, "mtllib") == 0) {
			char str[20];
			fscanf(file, "%s", str);
			mtlFileName = str;
		}
		else if (strcmp(lineHeader, "usemtl") == 0) {
			if (!IsFirstRead) {
				SubSetDatas.push_back(SubSetData);
			}
			char str[100];
			fscanf(file, "%s", str);

			SubSetData.MaterialName = str;
			SubSetData.FaceStart = SubSetData.FaceCount;
			SubSetData.FaceCount = 0;


			IsFirstRead = false;
		}
	}


	// �f�[�^�\����ύX���鏈��
	std::vector<unsigned int> newIndex(0);
	std::vector<float>newVertices(0);
	int IdxOffset = 0;

	for (int k = 0; k < SubSetDatas.size(); k++) {
		std::vector<glm::ivec3> SFaceVec;
		SFaceVec.resize(SubSetDatas[k].FaceCount * 3);
		unsigned int IdxCount = 0;
		ObjSubSet subset;		// ���ёւ����subset
		subset.MaterialName = SubSetDatas[k].MaterialName;

		for (int m = SubSetDatas[k].FaceStart;			// usemtl �ɏ�����Ă���͈͂�؂�o��
			m < SubSetDatas[k].FaceStart + SubSetDatas[k].FaceCount;
			m++) {
			SFaceVec[3 * (m - SubSetDatas[k].FaceStart)] = FaceVec[3 * m];
			SFaceVec[3 * (m - SubSetDatas[k].FaceStart) + 1] = FaceVec[3 * m + 1];
			SFaceVec[3 * (m - SubSetDatas[k].FaceStart) + 2] = FaceVec[3 * m + 2];
		}

		std::vector<glm::ivec3> TFaceVec(0);
		// FaceVec�̏d��index��r��
		for (int i = 0; i < SFaceVec.size(); i++) {
			bool IsHaving = false;
			for (int j = 0; j < TFaceVec.size(); j++) {
				if (SFaceVec[i] == TFaceVec[j]) {
					IsHaving = true;
					break;
				}
			}
			if (!IsHaving) {
				TFaceVec.push_back(SFaceVec[i]);
			}
		}

		// �V����Index�쐬
		subset.FaceStart = newIndex.size();
		for (int i = 0; i < SFaceVec.size(); i++) {
			int Idx = -1;
			for (int j = 0; j < TFaceVec.size(); j++) {
				if (SFaceVec[i] == TFaceVec[j]) {
					Idx = j;
					break;
				}
			}
			newIndex.push_back(Idx + IdxOffset);
			IdxCount++;
		}
		subset.FaceCount = IdxCount;

		// TFaceVec�ɑΉ����� vertices�쐬
		for (int i = 0; i < TFaceVec.size(); i++) {
			glm::ivec3 this_vec = TFaceVec[i];
			int vertI = 3 * (this_vec.x - 1);
			int uvI = 2 * (this_vec.y - 1);
			int normalI = 3 * (this_vec.z - 1);
			newVertices.push_back(CubeVertices[vertI]);
			newVertices.push_back(CubeVertices[vertI + 1]);
			newVertices.push_back(CubeVertices[vertI + 2]);
			newVertices.push_back(CubeNormals[normalI]);
			newVertices.push_back(CubeNormals[normalI + 1]);
			newVertices.push_back(CubeNormals[normalI + 2]);
			newVertices.push_back(CubeUV[uvI]);
			newVertices.push_back(CubeUV[uvI + 1]);
		}

		mObjSubSets.push_back(subset);
		IdxOffset += TFaceVec.size();
	}


	mVertices = newVertices;
	mIndices = newIndex;

	LoadMaterials(FilePath, mtlFileName);


	return true;
}

bool Mesh::LoadMaterials(std::string FilePath, std::string MtlFileName)
{
	// Material��ǂݍ���
	std::string mtlPath = FilePath + MtlFileName;
	FILE* file = fopen(mtlPath.c_str(), "r");
	if (file == NULL) {
		printf("error: failed to open mesh file\n");
		return false;
	}


	bool IsNewMtl = false;
	bool IsFirstRead = true;

	Material mtl;
	std::string mtlName;
	mtl.tex = nullptr;
	while (1) {
		char lineHeader[1000];
		// �s�̍ŏ��̕������ǂݍ��݂܂��B
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			mtl.DirLight.direction = glm::vec3(0, 0, 1.0f);
			mMaterials.insert(std::make_pair(mtlName, mtl));
			break;
		}

		if (strcmp(lineHeader, "newmtl") == 0) {
			if (!IsFirstRead) {			// ����ȍ~��newmtl���T�C����mMaterials�̗v�f�ǉ�
				mtl.DirLight.direction = glm::vec3(0, 0, 1.0f);
				mMaterials.insert(std::make_pair(mtlName, mtl));
				mtl.tex = nullptr;
			}

			char str[200];
			fscanf(file, "%s", str);
			mtlName = str;
			IsFirstRead = false;
		}
		else if (strcmp(lineHeader, "Ns") == 0) {		// specular power
			char str[20];
			fscanf(file, "%s", str);
			mSpecPower = atof(str);
			mtl.SpecPower = atof(str);
		}
		else if (strcmp(lineHeader, "Ka") == 0) {	// Ambient Color
			char r[20];
			char g[20];
			char b[20];

			fscanf(file, "%s %s %s", r, g, b);
			mAmbientLightColor.x = atof(r);
			mAmbientLightColor.y = atof(g);
			mAmbientLightColor.z = atof(b);
			mtl.AmbientColor.x = atof(r);
			mtl.AmbientColor.y = atof(g);
			mtl.AmbientColor.z = atof(b);
		}
		else if (strcmp(lineHeader, "Kd") == 0) {	// Diffuse Color
			char r[20];
			char g[20];
			char b[20];

			fscanf(file, "%s %s %s", r, g, b);
			mDirectionalLight.diffuseColor.x = atof(r);
			mDirectionalLight.diffuseColor.y = atof(g);
			mDirectionalLight.diffuseColor.z = atof(b);
			mtl.DirLight.diffuseColor.x = atof(r);
			mtl.DirLight.diffuseColor.y = atof(g);
			mtl.DirLight.diffuseColor.z = atof(b);
		}
		else if (strcmp(lineHeader, "Ks") == 0) {	// Specular Color
			char r[20];
			char g[20];
			char b[20];

			fscanf(file, "%s %s %s", r, g, b);
			mDirectionalLight.specColor.x = atof(r);
			mDirectionalLight.specColor.y = atof(g);
			mDirectionalLight.specColor.z = atof(b);
			mtl.DirLight.specColor.x = atof(r);
			mtl.DirLight.specColor.y = atof(g);
			mtl.DirLight.specColor.z = atof(b);
		}
		else if (strcmp(lineHeader, "map_Kd") == 0) {
			char str[100];
			fscanf(file, "%s", str);
			std::string tex_path = FilePath + str;
			Texture* tex = new Texture(tex_path);
			mtl.tex = tex;
		}
	}

	fclose(file);
	return true;
}

void Mesh::SetMeshLightings()
{
	mShader->SetVectorUniform("uAmbientLight", mAmbientLightColor);
	mShader->SetVectorUniform("uDirLight.mDirection", mDirectionalLight.direction);
	mShader->SetVectorUniform("uDirLight.mDiffuseColor", mDirectionalLight.diffuseColor);
	mShader->SetVectorUniform("uDirLight.mSpecColor", mDirectionalLight.specColor);
	mShader->SetFloatUniform("uSpecPower", mSpecPower);
}

void Mesh::SetMeshTransforms()
{
	glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(mMeshScale, mMeshScale, mMeshScale));
	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0f), mMeshPos);
	glm::mat4 TransformMat = TranslateMat * mMeshRotate * ScaleMat;

	mShader->SetMatrixUniform("model", TransformMat);
}

void Mesh::BindVertexArray()
{
	glBindVertexArray(mVertexArray);
}


void Mesh::Draw()
{
	mShader->UseProgram();
	BindVertexArray();
	SetMeshTransforms();

	for (auto subset = mObjSubSets.begin(); subset != mObjSubSets.end(); subset++) {
		Material material = mMaterials[subset->MaterialName];

		// Set Lightings
		mShader->SetVectorUniform("uAmbientLight", material.AmbientColor);
		mShader->SetVectorUniform("uDirLight.mDirection", mLightDir);
		mShader->SetVectorUniform("uDirLight.mDiffuseColor", material.DirLight.diffuseColor);
		mShader->SetVectorUniform("uDirLight.mSpecColor", material.DirLight.specColor);
		mShader->SetFloatUniform("uSpecPower", material.SpecPower);

		// Set Texture
		if (material.tex != nullptr) {
			material.tex->BindTexture();
		}

		std::vector<unsigned int> indices(subset->FaceCount);
		for (int i = subset->FaceStart; i < subset->FaceCount + subset->FaceStart; i++) {
			indices[i - subset->FaceStart] = mIndices[i];
		}
		unsigned int* indices_data = mIndices.data();


		//glDrawElements(GL_TRIANGLES, subset->FaceCount, GL_UNSIGNED_INT, mIndices.data());
		glDrawElements(GL_TRIANGLES, subset->FaceCount, GL_UNSIGNED_INT, &indices_data[subset->FaceStart]);
		//glDrawElements(GL_TRIANGLES, subset->FaceCount, GL_UNSIGNED_INT, 0);

		if (material.tex != nullptr) {
			material.tex->UnBindTexture();
		}
	}

	//SetMeshLightings();
	//mDonutTexture->BindTexture();
	//glDrawElements(GL_TRIANGLES, mDonut2Mesh->GetNumIndices(), GL_UNSIGNED_INT, 0);
	//mDonutTexture->UnBindTexture();
	glBindVertexArray(0);
}