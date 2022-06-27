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

class AssimpMesh {
public:
	AssimpMesh(std::string ObjFilePath, std::string MtlFilePath, Shader* shader);
	~AssimpMesh() {}
	bool AssimpLoader(std::string FilePath, std::string ObjFileName);


private:


};