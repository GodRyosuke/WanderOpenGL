#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.hpp"
#include "GLUtil.hpp"
#include <iostream>

Mesh::Mesh(std::string FilePath, std::string ObjFileName, Shader* shader, glm::vec3 LightDir, bool is_fbx)
	:mShader(shader),
	mVertices(0),
	mIndices(0),
	mLightDir(LightDir)
{
	if (is_fbx) {
		if (!LoadFBXFile(FilePath, ObjFileName)) {
			std::cout << "Failed to Load FBX File\n";
			return;
		}
	}
	else {
		if (!LoadObjFile(FilePath, ObjFileName)) {
			std::cout << "Failed to Load Mesh Obj File\n";
			return;
		}
	}


	//if (!LoadMaterials(MtlFilePath)) {
	//	std::cout << "Failed to Load Material Obj File\n";
	//	return;
	//}

	//mNumIndices = mIndices.size();


	//mShader->UseProgram();
	//glGenVertexArrays(1, &mVertexArray);
	//glBindVertexArray(mVertexArray);

	//glGenBuffers(1, &mVertexBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_STATIC_DRAW);
	////glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glGenBuffers(1, &mIndexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(int), mIndices.data(), GL_STATIC_DRAW);

	//// link attribution
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	////glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	////glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	//// unbind cube vertex arrays
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::CreateVAO(std::vector<float>VertexData, std::vector<float>NormalData,
	std::vector<float>UVData, std::vector<glm::ivec3> FaceVec, std::string mtlName,
	VAO& vao)
{
	// Veretx�̍\��������������
	std::vector<unsigned int> newIndex(0);
	std::vector<float> newVertices(0);

	std::vector<glm::ivec3> TFaceVec(0);
	// FaceVec�̏d��index��r��
	for (int i = 0; i < FaceVec.size(); i++) {
		bool IsHaving = false;
		for (int j = 0; j < TFaceVec.size(); j++) {
			if (FaceVec[i] == TFaceVec[j]) {
				IsHaving = true;
				break;
			}
		}
		if (!IsHaving) {
			TFaceVec.push_back(FaceVec[i]);
		}
	}

	// �V����Index�쐬
	for (int i = 0; i < FaceVec.size(); i++) {
		int Idx = -1;
		for (int j = 0; j < TFaceVec.size(); j++) {
			if (FaceVec[i] == TFaceVec[j]) {
				Idx = j;
				break;
			}
		}
		newIndex.push_back(Idx);
	}

	// TFaceVec�ɑΉ����� vertices�쐬
	for (int i = 0; i < TFaceVec.size(); i++) {
		glm::ivec3 this_vec = TFaceVec[i];
		int vertI = 3 * (this_vec.x - 1);
		int uvI = 2 * (this_vec.y - 1);
		int normalI = 3 * (this_vec.z - 1);
		newVertices.push_back(VertexData[vertI]);
		newVertices.push_back(VertexData[vertI + 1]);
		newVertices.push_back(VertexData[vertI + 2]);
		newVertices.push_back(NormalData[normalI]);
		newVertices.push_back(NormalData[normalI + 1]);
		newVertices.push_back(NormalData[normalI + 2]);
		newVertices.push_back(UVData[uvI]);
		newVertices.push_back(UVData[uvI + 1]);
	}

	
	// VAO�쐬����
	unsigned int VertexArray;
	unsigned int VertexBuffer;
	unsigned int IndexBuffer;

	mShader->UseProgram();
	glGenVertexArrays(1, &VertexArray);
	glBindVertexArray(VertexArray);

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), newVertices.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newIndex.size() * sizeof(int), newIndex.data(), GL_STATIC_DRAW);

	// link attribution
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
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

	vao.VertexArray = VertexArray;
	vao.VertexBuffer = VertexBuffer;
	vao.IndexBuffer = IndexBuffer;
	vao.IndicesSize = newIndex.size();
	vao.MaterialName = mtlName;
}

bool Mesh::LoadVAO(FILE* file, VAO& vao, int& VertexOffset, int& NormalOffset, int& UVOffset)
{
	std::vector<float> VertexData;
	std::vector<float> NormalData;
	std::vector<float> UVData;
	std::vector<glm::ivec3> FaceVec;
	std::string mtlName;

	while (1) {
		char lineHeader[1000];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			VertexOffset += VertexData.size() / 3;
			UVOffset += UVData.size() / 2;
			NormalOffset += NormalData.size() / 3;
			// VAO�쐬
			CreateVAO(VertexData, NormalData, UVData, FaceVec, mtlName, vao);

			return false;
		}

		if (strcmp(lineHeader, "v") == 0) {
			char x_str[20];
			char y_str[20];
			char z_str[20];
			fscanf(file, "%s %s %s", x_str, y_str, z_str);
			VertexData.push_back(atof(x_str));
			VertexData.push_back(atof(y_str));
			VertexData.push_back(atof(z_str));
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			char x_str[20];
			char y_str[20];
			fscanf(file, "%s %s", x_str, y_str);
			UVData.push_back(atof(x_str));
			UVData.push_back(atof(y_str));
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			char x_str[20];
			char y_str[20];
			char z_str[20];
			fscanf(file, "%s %s %sn", x_str, y_str, z_str);
			NormalData.push_back(atof(x_str));
			NormalData.push_back(atof(y_str));
			NormalData.push_back(atof(z_str));
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
			a1[0] = vertexIndex.x - VertexOffset;	a1[1] = uvIndex.x - UVOffset;	a1[2] = normalIndex.x - NormalOffset;
			a2[0] = vertexIndex.y - VertexOffset;	a2[1] = uvIndex.y - UVOffset;	a2[2] = normalIndex.y - NormalOffset;
			a3[0] = vertexIndex.z - VertexOffset;	a3[1] = uvIndex.z - UVOffset;	a3[2] = normalIndex.z - NormalOffset;
			FaceVec.push_back(a1);
			FaceVec.push_back(a2);
			FaceVec.push_back(a3);
		}
		else if (strcmp(lineHeader, "usemtl") == 0) {	// mtl name
			char str[100];
			fscanf(file, "%s", str);
			mtlName = str;
		}
		else if (strcmp(lineHeader, "o") == 0) {
			VertexOffset += VertexData.size() / 3;
			UVOffset += UVData.size() / 2;
			NormalOffset += NormalData.size() / 3;
			// VAO�쐬
			CreateVAO(VertexData, NormalData, UVData, FaceVec, mtlName, vao);
			return true;
		}
	}

	return true;
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

	while (1) {
		char lineHeader[128];
		// �s�̍ŏ��̕������ǂݍ��݂܂��B
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			return false;
		}

		if (strcmp(lineHeader, "mtllib") == 0) {	// mtl file name
			char str[20];
			fscanf(file, "%s", str);
			mtlFileName = str;

			res = fscanf(file, "%s", lineHeader);
			if (strcmp(lineHeader, "o") == 0) {		// object�{�̓ǂݏo��
				int VertexOffset = 0;
				int NormalOffset = 0;
				int UVOffset = 0;
				VAO vao;
				while (LoadVAO(file, vao, VertexOffset, NormalOffset, UVOffset)) {
					mVAOs.push_back(vao);
				}
				mVAOs.push_back(vao);
				break;
			}
			else {
				return false;
			}
		}
	}

	// �}�e���A���ǂݏo��
	LoadMaterials(FilePath, mtlFileName);


	return true;
}

bool Mesh::deLoadObjFile(std::string FilePath, std::string ObjFileName)
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

void Mesh::LoadFBXMaterial(FbxMesh* mesh, Material& material, FbxSurfaceMaterial* fbxMaterial, std::string& materialName)
{
	FbxProperty prop;
	//prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (prop.IsValid()) {

	}

	if (mesh->GetElementMaterialCount() == 0) {
		materialName = "";
	}

	//FbxLayerElementMaterial* lMaterial = mesh->GetElementMaterial(0);
	//FbxSurfaceMaterial* surface_material;
	//{
	//	int index = lMaterial->GetIndexArray().GetAt(0);
	//	surface_material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);
	//}
	//if (surface_material != nullptr) {
	//	materialName = surface_material->GetName();
	//}
	//else {
	//	materialName = "";
	//}


	fbxMaterial->sAmbient[0];
	//material.AmbientColor.x = surface_material->sAmbient[0];
	//material.AmbientColor.y = surface_material->sAmbient[1];
	//material.AmbientColor.z = surface_material->sAmbient[2];

}

void Mesh::LoadFBXMaterial(FbxSurfaceMaterial* material)
{
	FBXMaterial materialData;
	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);

	if (material->GetClassId().Is(FbxSurfacePhong::ClassId)) {
		// Ambient
		prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);
		if (prop.IsValid()) {
			materialData.AmbientColor.x = prop.Get<FbxDouble3>()[0];
			materialData.AmbientColor.y = prop.Get<FbxDouble3>()[1];
			materialData.AmbientColor.z = prop.Get<FbxDouble3>()[2];
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
		if (prop.IsValid()) {
			float AmbientFactor = prop.Get<FbxDouble>();
			materialData.AmbientColor *= AmbientFactor;
		}

		// Diffuse
		prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (prop.IsValid()) {
			materialData.DiffuseColor.x = prop.Get<FbxDouble3>()[0];
			materialData.DiffuseColor.y = prop.Get<FbxDouble3>()[1];
			materialData.DiffuseColor.z = prop.Get<FbxDouble3>()[2];
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
		if (prop.IsValid()) {
			float DiffuseFactor = prop.Get<FbxDouble>();
			materialData.DiffuseColor *= DiffuseFactor;
		}
		
		// Specular
		prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
		if (prop.IsValid()) {
			materialData.SpecColor.x = prop.Get<FbxDouble3>()[0];
			materialData.SpecColor.y = prop.Get<FbxDouble3>()[1];
			materialData.SpecColor.z = prop.Get<FbxDouble3>()[2];
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
		if (prop.IsValid()) {
			float specFactor = prop.Get<FbxDouble>();
			materialData.SpecColor *= specFactor;
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sShininess);
		if (prop.IsValid()) {
			materialData.SpecPower = prop.Get<FbxDouble>();
		}

		// Transparency
		prop = material->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
		if (prop.IsValid()) {
			materialData.Alpha = prop.Get<FbxDouble>();
		}

		// Bump
		prop = material->FindProperty(FbxSurfaceMaterial::sBump);
		if (prop.IsValid()) {
			materialData.Bump.x = prop.Get<FbxDouble3>()[0];
			materialData.Bump.y = prop.Get<FbxDouble3>()[1];
			materialData.Bump.z = prop.Get<FbxDouble3>()[2];
		}
		prop = material->FindProperty(FbxSurfaceMaterial::sBumpFactor);
		if (prop.IsValid()) {
			materialData.Bump *= prop.Get<FbxDouble>();
		}

		// NormalMap
		prop = material->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (prop.IsValid()) {
			materialData.NormalMap.x = prop.Get<FbxDouble3>()[0];
			materialData.NormalMap.y = prop.Get<FbxDouble3>()[1];
			materialData.NormalMap.z = prop.Get<FbxDouble3>()[2];
		}

		std::string materialName = material->GetName();
		mFBXMaterials.insert(std::make_pair(materialName, materialData));
	}
	else {
		std::cout << "error: This surface is not allowed Phong Reflection model" << std::endl;
	}
}

void Mesh::LoadFBXMeshData(FbxMesh* lMesh)
{
	const int lVertexCount = lMesh->GetControlPointsCount();
	std::vector<unsigned int>indices;
	struct customVert {
		glm::vec3 vert;
		glm::vec3 normal;
		glm::vec2 uv;
	};
	std::vector<customVert>vertices;

	// Index�擾
	for (int i = 0; i < lMesh->GetPolygonCount(); i++) {
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// Vertex�擾
	FbxVector4* lVertexArray = NULL;
	lVertexArray = new FbxVector4[lVertexCount];
	memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

	int* polygonIndices = lMesh->GetPolygonVertices();
	for (int i = 0; i < lMesh->GetPolygonVertexCount(); i++) {
		int index = polygonIndices[i];
		customVert point;
		point.vert.x = lVertexArray[index][0];
		point.vert.y = lVertexArray[index][1];
		point.vert.z = lVertexArray[index][2];
		vertices.push_back(point);
	}

	// �@���擾
	FbxArray<FbxVector4> lNormals;
	lMesh->GetPolygonVertexNormals(lNormals);
	for (int i = 0; i < lNormals.Size(); i++) {
		vertices[i].normal.x = lNormals[i][0];
		vertices[i].normal.y = lNormals[i][1];
		vertices[i].normal.z = lNormals[i][2];
	}

	// UV�擾
	FbxStringList lUVNames;
	lMesh->GetUVSetNames(lUVNames);
	FbxArray<FbxVector2> lUVs;
	lMesh->GetPolygonVertexUVs(lUVNames.GetStringAt(0), lUVs);
	for (int i = 0; i < lUVs.Size(); i++) {
		vertices[i].uv.x = lUVs[i][0];
		vertices[i].uv.y = lUVs[i][1];
	}

	// �}�e���A�����擾
	std::string MaterialName;
	if (lMesh->GetElementMaterialCount() == 0)
	{
		MaterialName = "";
		return;
	}

	// Mesh���̃}�e���A�������擾
	FbxLayerElementMaterial* material = lMesh->GetElementMaterial(0);
	int index = material->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* surface_material = lMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);

	if (surface_material != nullptr)
	{
		MaterialName = surface_material->GetName();
	}
	else
	{
		MaterialName = "";
	}


	// VAO�쐬
	// �f�[�^�����ς���
	std::vector<float> vertices_data;
	for (int i = 0; i < vertices.size(); i++) {
		vertices_data.push_back(vertices[i].vert.x);
		vertices_data.push_back(vertices[i].vert.y);
		vertices_data.push_back(vertices[i].vert.z);
		vertices_data.push_back(vertices[i].normal.x);
		vertices_data.push_back(vertices[i].normal.y);
		vertices_data.push_back(vertices[i].normal.z);
		vertices_data.push_back(vertices[i].uv.x);
		vertices_data.push_back(vertices[i].uv.y);
	}

	unsigned int VertexArray;
	unsigned int VertexBuffer;
	unsigned int IndexBuffer;

	mShader->UseProgram();
	glGenVertexArrays(1, &VertexArray);
	glBindVertexArray(VertexArray);

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_data.size() * sizeof(float), vertices_data.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// link attribution
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
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

	VAO vao;
	vao.VertexArray = VertexArray;
	vao.VertexBuffer = VertexBuffer;
	vao.IndexBuffer = IndexBuffer;
	vao.IndicesSize = indices.size();
	vao.MaterialName = MaterialName;
	mVAOs.push_back(vao);
}

void Mesh::searchNode(FbxScene* scene, FbxGeometryConverter converter, FbxNode* node)
{
	if (node->GetChildCount() == 0) {
		return;
	}

	for (int no = 0; no < node->GetChildCount(); no++) {
		FbxNode* lNode = node->GetChild(no);

		for (int attrIdx = 0; attrIdx < lNode->GetNodeAttributeCount(); attrIdx++) {
			FbxNodeAttribute* attr = lNode->GetNodeAttributeByIndex(attrIdx);
			if (attr) {
				if (attr->GetAttributeType() == FbxNodeAttribute::eMesh) {
					// mesh�쐬����

					FbxMesh* lMesh = lNode->GetMesh();
					//converter.SplitMeshPerMaterial(lMesh, true);

					const int lVertexCount = lMesh->GetControlPointsCount();
					std::vector<unsigned int>indices;
					struct customVert {
						glm::vec3 vert;
						glm::vec3 normal;
						glm::vec2 uv;
					};
					std::vector<customVert>vertices;

					// Index�擾
					for (int i = 0; i < lMesh->GetPolygonCount(); i++) {
						indices.push_back(i * 3);
						indices.push_back(i * 3 + 1);
						indices.push_back(i * 3 + 2);
					}

					// Vertex�擾
					FbxVector4* lVertexArray = NULL;
					lVertexArray = new FbxVector4[lVertexCount];
					memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

					int* polygonIndices = lMesh->GetPolygonVertices();
					for (int i = 0; i < lMesh->GetPolygonVertexCount(); i++) {
						int index = polygonIndices[i];
						customVert point;
						point.vert.x = lVertexArray[index][0];
						point.vert.y = lVertexArray[index][1];
						point.vert.z = lVertexArray[index][2];
						vertices.push_back(point);
					}

					// �@���擾
					FbxArray<FbxVector4> lNormals;
					lMesh->GetPolygonVertexNormals(lNormals);
					for (int i = 0; i < lNormals.Size(); i++) {
						vertices[i].normal.x = lNormals[i][0];
						vertices[i].normal.y = lNormals[i][1];
						vertices[i].normal.z = lNormals[i][2];
					}

					// UV�擾
					FbxStringList lUVNames;
					lMesh->GetUVSetNames(lUVNames);
					FbxArray<FbxVector2> lUVs;
					lMesh->GetPolygonVertexUVs(lUVNames.GetStringAt(0), lUVs);
					for (int i = 0; i < lUVs.Size(); i++) {
						vertices[i].uv.x = lUVs[i][0];
						vertices[i].uv.y = lUVs[i][1];
					}

					// �}�e���A���擾
					Material material;
					std::string materialName;
					for (int i = 0; i < scene->GetMaterialCount(); i++) {
						FbxSurfaceMaterial* surfaceMaterial = scene->GetMaterial(i);
						//material.AmbientColor.x = surfaceMaterial->sAmbient[0];
						//material.AmbientColor.y = surfaceMaterial->sAmbient[1];
						//material.AmbientColor.z = surfaceMaterial->sAmbient[2];
						//LoadFBXMaterial(lMesh, material, scene->GetMaterial(i), materialName);
					}
					//for (int i = 0; i < scene->GetSrcObjectCount<FbxSurfaceMaterial>(); i++) {
					//	LoadFBXMaterial(lMesh, material, scene->GetSrcObject<FbxSurfaceMaterial>(i), materialName);
					//}



					// VAO�쐬
					// �f�[�^�����ς���
					std::vector<float> vertices_data;
					for (int i = 0; i < vertices.size(); i++) {
						vertices_data.push_back(vertices[i].vert.x);
						vertices_data.push_back(vertices[i].vert.y);
						vertices_data.push_back(vertices[i].vert.z);
						vertices_data.push_back(vertices[i].normal.x);
						vertices_data.push_back(vertices[i].normal.y);
						vertices_data.push_back(vertices[i].normal.z);
						vertices_data.push_back(vertices[i].uv.x);
						vertices_data.push_back(vertices[i].uv.y);
					}

					unsigned int VertexArray;
					unsigned int VertexBuffer;
					unsigned int IndexBuffer;

					mShader->UseProgram();
					glGenVertexArrays(1, &VertexArray);
					glBindVertexArray(VertexArray);

					glGenBuffers(1, &VertexBuffer);
					glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
					glBufferData(GL_ARRAY_BUFFER, vertices_data.size() * sizeof(float), vertices_data.data(), GL_STATIC_DRAW);
					//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

					glGenBuffers(1, &IndexBuffer);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

					// link attribution
					glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
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

					VAO vao;
					vao.VertexArray = VertexArray;
					vao.VertexBuffer = VertexBuffer;
					vao.IndexBuffer = IndexBuffer;
					vao.IndicesSize = indices.size();
					vao.MaterialName = "FBX";
					mVAOs.push_back(vao);

					int x = 0;
				}
			}

		}

		searchNode(scene, converter, lNode);
	}
}

bool Mesh::LoadFBXFile(std::string FilePath, std::string FBXFileName)
{
	std::string fbxFilePath = FilePath + FBXFileName;

	FbxManager* manager = FbxManager::Create();
	if (!manager) {
		std::cerr << "failed to create FbxManager." << std::endl;
		manager->Destroy();
		return false;
	}

	// FbxImporter�쐬
	FbxImporter* fbx_importer = FbxImporter::Create(manager, "");
	if (fbx_importer == nullptr)
	{
		manager->Destroy();
		return false;
	}

	// FbxScene�쐬
	FbxScene* fbx_scene = FbxScene::Create(manager, "");
	if (fbx_scene == nullptr)
	{
		fbx_importer->Destroy();
		manager->Destroy();
		return false;
	}

	// File��������
	fbx_importer->Initialize(fbxFilePath.c_str());
	// scene�ɃC���|�[�g
	fbx_importer->Import(fbx_scene);

	FbxGeometryConverter converter(manager);
	// ���b�V���Ɏg���Ă���}�e���A���P�ʂŃ��b�V���𕪊�����
	converter.SplitMeshesPerMaterial(fbx_scene, true);
	// �|���S�����O�p�`�ɂ���
	converter.Triangulate(fbx_scene, true);

	// Material�ǂݍ���
	int material_num = fbx_scene->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int i = 0; i < material_num; i++)
	{
		LoadFBXMaterial(fbx_scene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	// Mesh�ǂݍ���
	for (int i = 0; i < fbx_scene->GetSrcObjectCount<FbxMesh>(); i++) {
		FbxMesh* mesh = fbx_scene->GetSrcObject<FbxMesh>(i);

	}

	//FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	//manager->SetIOSettings(ios);

	//FbxImporter* importer = FbxImporter::Create(manager, "");
	//if (!importer->Initialize(fbxFilePath.c_str(), -1, manager->GetIOSettings())) {
	//	printf("Call to FbxImporter::Initialize() failed.\n");
	//	printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
	//	exit(-1);
	//}


	//FbxScene* scene = FbxScene::Create(manager, "wander_scene");
	//if (!scene) {
	//	std::cerr << "failed to create FbxScene." << std::endl;
	//	manager->Destroy();
	//	return false;
	//}

	//importer->Import(scene);
	//importer->Destroy();

	//// 3�p�`�|���S���ɂ���
	//FbxGeometryConverter converter(manager);
	//converter.Triangulate(scene, true);

	//// �|���S�����ƂɁAMesh�𕪊�
	//converter.SplitMeshesPerMaterial(scene, true);

	//FbxNode* rootNode = scene->GetRootNode();
	//if (rootNode) {
	//	searchNode(scene, converter, rootNode);
	//}



	//manager->Destroy();
	return true;




	// �|���S�����O�p�`�ɕϊ�
	//FbxGeometryConverter geometry_converter(manager);
	//geometry_converter.Triangulate(scene, true);

	//FbxMesh* mesh = scene->GetSrcObject<FbxMesh>();
	//if (!mesh) {
	//	std::cerr << "failed to load mesh." << " (" << fbxFilePath << ")" << std::endl;
	//	scene->Destroy();
	//	manager->Destroy();
	//	return false;
	//}

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
	SetMeshTransforms();

	for (int i = 0; i < mVAOs.size(); i++) {
		VAO vao = mVAOs[i];
		glBindVertexArray(vao.VertexArray);

		if (vao.MaterialName == "FBX") {
			glDrawElements(GL_TRIANGLES, vao.IndicesSize, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			continue;
		}

		Material material = mMaterials[vao.MaterialName];
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

		glDrawElements(GL_TRIANGLES, vao.IndicesSize, GL_UNSIGNED_INT, 0);

		if (material.tex != nullptr) {
			material.tex->UnBindTexture();
		}
		glBindVertexArray(0);
	}
}

void Mesh::deDraw()
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
		//glDrawElements(GL_TRIANGLES, subset->FaceCount, GL_UNSIGNED_INT, &indices_data[subset->FaceStart]);
		glDrawElements(GL_TRIANGLES, subset->FaceCount, GL_UNSIGNED_INT, 0);

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