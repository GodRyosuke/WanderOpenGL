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
	// Veretxの構造書き換え処理
	std::vector<unsigned int> newIndex(0);
	std::vector<float> newVertices(0);

	std::vector<glm::ivec3> TFaceVec(0);
	// FaceVecの重複indexを排除
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

	// 新しいIndex作成
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

	// TFaceVecに対応する vertices作成
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

	
	// VAO作成処理
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
			// VAO作成
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
			// VAO作成
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

	// obj fileを読みだす
	FILE* file = fopen(ObjFilePath.c_str(), "r");
	if (file == NULL) {
		printf("error: failed to open mesh file\n");
		return false;
	}

	while (1) {
		char lineHeader[128];
		// 行の最初の文字列を読み込みます。
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			return false;
		}

		if (strcmp(lineHeader, "mtllib") == 0) {	// mtl file name
			char str[20];
			fscanf(file, "%s", str);
			mtlFileName = str;

			res = fscanf(file, "%s", lineHeader);
			if (strcmp(lineHeader, "o") == 0) {		// object本体読み出し
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

	// マテリアル読み出し
	LoadMaterials(FilePath, mtlFileName);


	return true;
}

bool Mesh::deLoadObjFile(std::string FilePath, std::string ObjFileName)
{
	std::string ObjFilePath = FilePath + ObjFileName;
	std::string mtlFileName;

	// obj fileを読みだす
	FILE* file = fopen(ObjFilePath.c_str(), "r");
	if (file == NULL) {
		printf("error: failed to open mesh file\n");
		return false;
	}

	std::vector<float>CubeVertices;
	std::vector<float>CubeUV;
	std::vector<float>CubeNormals;
	std::vector<glm::ivec3>FaceVec;
	std::vector<glm::ivec3>RFaceVec(0);		// 重複したindexが除去された配列
	std::vector<ObjSubSet> SubSetDatas;

	bool IsFirstRead = true;
	ObjSubSet SubSetData;
	SubSetData.FaceCount = 0;
	while (1) {

		char lineHeader[128];
		// 行の最初の文字列を読み込みます。
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


	// データ構造を変更する処理
	std::vector<unsigned int> newIndex(0);
	std::vector<float>newVertices(0);
	int IdxOffset = 0;

	for (int k = 0; k < SubSetDatas.size(); k++) {
		std::vector<glm::ivec3> SFaceVec;
		SFaceVec.resize(SubSetDatas[k].FaceCount * 3);
		unsigned int IdxCount = 0;
		ObjSubSet subset;		// 並び替え後のsubset
		subset.MaterialName = SubSetDatas[k].MaterialName;

		for (int m = SubSetDatas[k].FaceStart;			// usemtl に書かれている範囲を切り出す
			m < SubSetDatas[k].FaceStart + SubSetDatas[k].FaceCount;
			m++) {
			SFaceVec[3 * (m - SubSetDatas[k].FaceStart)] = FaceVec[3 * m];
			SFaceVec[3 * (m - SubSetDatas[k].FaceStart) + 1] = FaceVec[3 * m + 1];
			SFaceVec[3 * (m - SubSetDatas[k].FaceStart) + 2] = FaceVec[3 * m + 2];
		}

		std::vector<glm::ivec3> TFaceVec(0);
		// FaceVecの重複indexを排除
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

		// 新しいIndex作成
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

		// TFaceVecに対応する vertices作成
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
	// Materialを読み込む
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
		// 行の最初の文字列を読み込みます。
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			mtl.DirLight.direction = glm::vec3(0, 0, 1.0f);
			mMaterials.insert(std::make_pair(mtlName, mtl));
			break;
		}

		if (strcmp(lineHeader, "newmtl") == 0) {
			if (!IsFirstRead) {			// 初回以降はnewmtlをサインにmMaterialsの要素追加
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
					// mesh作成処理
					FbxMesh* lMesh = lNode->GetMesh();
					converter.SplitMeshPerMaterial(lMesh, true);

					const int lVertexCount = lMesh->GetControlPointsCount();
					std::vector<unsigned int>indices;
					struct customVert {
						glm::vec3 vert;
						glm::vec3 normal;
						glm::vec2 uv;
					};
					std::vector<customVert>vertices;

					// Index取得
					for (int i = 0; i < lMesh->GetPolygonCount(); i++) {
						indices.push_back(i * 3);
						indices.push_back(i * 3 + 1);
						indices.push_back(i * 3 + 2);
					}

					// Vertex取得
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

					// 法線取得
					FbxArray<FbxVector4> lNormals;
					lMesh->GetPolygonVertexNormals(lNormals);
					for (int i = 0; i < lNormals.Size(); i++) {
						vertices[i].normal.x = lNormals[i][0];
						vertices[i].normal.y = lNormals[i][1];
						vertices[i].normal.z = lNormals[i][2];
					}

					// UV取得
					FbxStringList lUVNames;
					lMesh->GetUVSetNames(lUVNames);
					FbxArray<FbxVector2> lUVs;
					lMesh->GetPolygonVertexUVs(lUVNames.GetStringAt(0), lUVs);
					for (int i = 0; i < lUVs.Size(); i++) {
						vertices[i].uv.x = lUVs[i][0];
						vertices[i].uv.y = lUVs[i][1];
					}

					// マテリアル取得


					// VAO作成
					// データを作り変える
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

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(manager, "");
	if (!importer->Initialize(fbxFilePath.c_str(), -1, manager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
		exit(-1);
	}


	FbxScene* scene = FbxScene::Create(manager, "wander_scene");
	if (!scene) {
		std::cerr << "failed to create FbxScene." << std::endl;
		manager->Destroy();
		return false;
	}

	importer->Import(scene);
	importer->Destroy();

	// 3角形ポリゴンにする
	FbxGeometryConverter converter(manager);
	converter.Triangulate(scene, true);

	// ポリゴンごとに、Meshを分割
	converter.SplitMeshesPerMaterial(scene, true);

	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode) {
		searchNode(scene, converter, rootNode);
		//for (int i = 0; i < rootNode->GetChildCount(); i++) {
		//	FbxNode* node = rootNode->GetChild(i);
		//	FbxNodeAttribute* attr = node->GetNodeAttribute();

		//	if (attr) {
		//		if (attr->GetAttributeType() == FbxNodeAttribute::eMesh) {
		//			// mesh作成処理
		//			FbxMesh* lMesh = node->GetMesh();
		//			const int lVertexCount = lMesh->GetControlPointsCount();
		//			std::vector<unsigned int>indices;
		//			struct customVert {
		//				glm::vec3 vert;
		//				glm::vec3 normal;
		//				glm::vec2 uv;
		//			};
		//			std::vector<customVert>vertices;

		//			// Index取得
		//			for (int i = 0; i < lMesh->GetPolygonCount(); i++) {
		//				indices.push_back(i * 3);
		//				indices.push_back(i * 3 + 1);
		//				indices.push_back(i * 3 + 2);
		//			}

		//			// Vertex取得
		//			FbxVector4* lVertexArray = NULL;
		//			lVertexArray = new FbxVector4[lVertexCount];
		//			memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

		//			int* polygonIndices = lMesh->GetPolygonVertices();
		//			for (int i = 0; i < lMesh->GetPolygonVertexCount(); i++) {
		//				int index = polygonIndices[i];
		//				customVert point;
		//				point.vert.x = lVertexArray[index][0];
		//				point.vert.y = lVertexArray[index][1];
		//				point.vert.z = lVertexArray[index][2];
		//				vertices.push_back(point);
		//			}

		//			// 法線取得
		//			FbxArray<FbxVector4> lNormals;
		//			lMesh->GetPolygonVertexNormals(lNormals);
		//			for (int i = 0; i < lNormals.Size(); i++) {
		//				vertices[i].normal.x = lNormals[i][0];
		//				vertices[i].normal.y = lNormals[i][1];
		//				vertices[i].normal.z = lNormals[i][2];
		//			}

		//			// UV取得
		//			FbxStringList lUVNames;
		//			lMesh->GetUVSetNames(lUVNames);
		//			FbxArray<FbxVector2> lUVs;
		//			lMesh->GetPolygonVertexUVs(lUVNames.GetStringAt(0), lUVs);
		//			for (int i = 0; i < lUVs.Size(); i++) {
		//				vertices[i].uv.x = lUVs[i][0];
		//				vertices[i].uv.y = lUVs[i][1];
		//			}

		//			// マテリアル取得


		//			// VAO作成
		//			// データを作り変える
		//			std::vector<float> vertices_data;
		//			for (int i = 0; i < vertices.size(); i++) {
		//				vertices_data.push_back(vertices[i].vert.x);
		//				vertices_data.push_back(vertices[i].vert.y);
		//				vertices_data.push_back(vertices[i].vert.z);
		//				vertices_data.push_back(vertices[i].normal.x);
		//				vertices_data.push_back(vertices[i].normal.y);
		//				vertices_data.push_back(vertices[i].normal.z);
		//				vertices_data.push_back(vertices[i].uv.x);
		//				vertices_data.push_back(vertices[i].uv.y);
		//			}

		//			unsigned int VertexArray;
		//			unsigned int VertexBuffer;
		//			unsigned int IndexBuffer;

		//			mShader->UseProgram();
		//			glGenVertexArrays(1, &VertexArray);
		//			glBindVertexArray(VertexArray);

		//			glGenBuffers(1, &VertexBuffer);
		//			glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		//			glBufferData(GL_ARRAY_BUFFER, vertices_data.size() * sizeof(float), vertices_data.data(), GL_STATIC_DRAW);
		//			//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//			glGenBuffers(1, &IndexBuffer);
		//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
		//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		//			// link attribution
		//			glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		//			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//			//glEnableVertexAttribArray(0);
		//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		//			glEnableVertexAttribArray(0);
		//			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//			glEnableVertexAttribArray(1);
		//			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//			glEnableVertexAttribArray(2);

		//			// unbind cube vertex arrays
		//			glBindVertexArray(0);
		//			glBindBuffer(GL_ARRAY_BUFFER, 0);
		//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//			VAO vao;
		//			vao.VertexArray = VertexArray;
		//			vao.VertexBuffer = VertexBuffer;
		//			vao.IndexBuffer = IndexBuffer;
		//			vao.IndicesSize = indices.size();
		//			vao.MaterialName = "FBX";
		//			mVAOs.push_back(vao);

		//			int x = 0;
		//		}
		//	}
		//}
	}


	//if (lNodeAttribute)
	//{
	//	// All lights has been processed before the whole scene because they influence every geometry.
	//	if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMarker)
	//	{
	//		DrawMarker(pGlobalPosition);
	//	}
	//	else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	//	{
	//		DrawSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
	//	}
	//	// NURBS and patch have been converted into triangluation meshes.
	//	else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	//	{
	//		DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose, pShadingMode);
	//	}
	//	else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
	//	{
	//		DrawCamera(pNode, pTime, pAnimLayer, pGlobalPosition);
	//	}
	//	else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
	//	{
	//		DrawNull(pGlobalPosition);
	//	}
	//}
	//else
	//{
	//	// Draw a Null for nodes without attribute.
	//	DrawNull(pGlobalPosition);
	//}


	manager->Destroy();
	return true;


	//if (!importer->Initialize(fbxFilePath.c_str(), -1, manager->GetIOSettings())) {
	//	std::cerr << "failed to initialize FbxImporter." << std::endl;
	//	importer->Destroy();
	//	manager->Destroy();
	//	scene->Destroy();
	//	return false;
	//}

	//if (!importer->Import(scene))
	//{
	//	std::cerr << "failed to import: " << fbxFilePath << std::endl;
	//	importer->Destroy();
	//	manager->Destroy();
	//	scene->Destroy();
	//	return false;
	//}

	//importer->Destroy();

	// ポリゴンを三角形に変換
	FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, true);

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
	for (auto vao = mVAOs.begin(); vao != mVAOs.end(); vao++) {
		glBindVertexArray(vao->VertexArray);

		if (vao->MaterialName == "FBX") {
			glDrawElements(GL_TRIANGLES, vao->IndicesSize, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			break;
			continue;
		}

		Material material = mMaterials[vao->MaterialName];
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

		glDrawElements(GL_TRIANGLES, vao->IndicesSize, GL_UNSIGNED_INT, 0);

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