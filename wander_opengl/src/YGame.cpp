#define _CRT_SECURE_NO_WARNINGS

#include "YGame.hpp"
#include <time.h>

// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

float SpriteVertices[] = {
	-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
	0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
	0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
	-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
};

unsigned int SpriteIndices[] = {
	0, 1, 2,
	2, 3, 0
};

YGame::YGame()
	:mWindowWidth(1024),
	mWindowHeight(768),
	mIsRunning(true),
	mReComputeWorldTransform(true),
	mCubeRotation(0.0f),
	mCubeRotateVel(1),
	mPhase(PHASE_IDLE)
{

}

bool YGame::Initialize()
{

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Set OpenGL attributes
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


	mWindow = SDL_CreateWindow("Wander OpenGL Tutorial", 100, 100,
		mWindowWidth, mWindowHeight, SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	auto error_code = glGetError();

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		printf("IMG_Init: Failed to init required jpg and png support!\n");
		std::string error = IMG_GetError();
		SDL_Log("IMG_Init: %s\n", IMG_GetError());

	}

	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	//CreateSpriteVerts();

	if (!LoadData())
	{
		SDL_Log("Failed to load data.");
		return false;
	}


	mTicksCount = SDL_GetTicks();

	return true;
}

static void SetMatrixUniform(std::string uniformName, glm::mat4 mat, GLuint shaderProgram)
{
	GLuint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

static void SetVectorUniform(std::string uniformName, glm::vec3 vec, GLuint shaderProgram)
{
	GLuint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glUniform3fv(loc, 1, glm::value_ptr(vec));
}

static bool CompileShader(std::string filepath, GLenum shaderType, GLuint& shader)
{
	std::ifstream shaderFile(filepath);
	if (shaderFile.is_open())
	{
		// Read all the text into a string
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// Create a shader of the specified type
		shader = glCreateShader(shaderType);
		// Set the source characters and try to compile
		glShaderSource(shader, 1, &(contentsChar), nullptr);
		glCompileShader(shader);

		GLint status;
		// Query the compile status
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE)
		{
			char buffer[512];
			memset(buffer, 0, 512);
			glGetShaderInfoLog(shader, 511, nullptr, buffer);
			SDL_Log("GLSL Compile Failed:\n%s", buffer);
			return false;
		}
	}
	else
	{
		SDL_Log("Shader file not found: %s", filepath.c_str());
		return false;
	}
	return true;
}

static bool CreateShaderProgram(std::string vertFilePath, std::string fragFilePath,
	GLuint& shaderProgram)
{
	GLuint VertexShader;
	GLuint FragmentShader;
	// Compile vertex and pixel shaders
	if (!CompileShader(vertFilePath,
		GL_VERTEX_SHADER,
		VertexShader) ||
		!CompileShader(fragFilePath,
			GL_FRAGMENT_SHADER,
			FragmentShader))
	{
		return false;
	}

	// Now create a shader program that
	// links together the vertex/frag shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, VertexShader);
	glAttachShader(shaderProgram, FragmentShader);
	glLinkProgram(shaderProgram);

	// Verify that the program linked successfully
	GLint status;
	// Query the link status
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(shaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL Link Status:\n%s", buffer);
		return false;
	}

	glUseProgram(shaderProgram);
}

bool YGame::LoadShaders()
{
	std::string spriteVertFile = "./Shaders/Sprite.vert";
	std::string spriteFragFile = "./Shaders/Sprite.frag";

	if (!CreateShaderProgram(spriteVertFile, spriteFragFile, mSpriteShaderProgram)) {
		return false;
	}

	// Sprite
	glm::mat4 spriteViewProj = glm::mat4(1.0f);
	spriteViewProj[0][0] = 2.0f / (float)mWindowWidth;
	spriteViewProj[1][1] = 2.0f / (float)mWindowHeight;
	spriteViewProj[3][2] = 1.0f;
	SetMatrixUniform("uViewProj", spriteViewProj, mSpriteShaderProgram);

	std::string meshVertFile = "./Shaders/YPhong.vert";
	std::string meshFragFile = "./Shaders/YPhong.frag";

	if (!CreateShaderProgram(meshVertFile, meshFragFile, mMeshShaderProgram)) {
		return false;
	}



	// View Projection行列を設定する
	// View行列の計算
	mView = glm::mat4(1.0f);
	mProjection = glm::mat4(1.0f);
	mView = glm::translate(mView, glm::vec3(0.0f, 0, 0));
	mProjection = glm::perspective(glm::radians(45.0f), (float)mWindowWidth / mWindowHeight, 0.1f, 100.0f);
	mCubeWorldTrans = glm::mat4(1.0);
	mCubeWorldTrans = glm::translate(mCubeWorldTrans, glm::vec3(0.0f, 0.0f, -15.0f));


	//mView = CreateLookAt(Eigen::Vector3d::Zero(), Eigen::Vector3d::UnitX(), Eigen::Vector3d::UnitZ());
	//mProjection = CreatePerspectiveFOV(70 * M_PI / 180,
	//	mWindowWidth, mWindowHeight, 25.0, 10000.0);

	// Uniform の初期値
	// Mesh
	SetMatrixUniform("view", mView, mMeshShaderProgram);
	SetMatrixUniform("proj", mProjection, mMeshShaderProgram);
	SetMatrixUniform("model", mCubeWorldTrans, mMeshShaderProgram);


	
	//{
	//{ 2.0f / width, 0.0f, 0.0f, 0.0f },
	//{ 0.0f, 2.0f / height, 0.0f, 0.0f },
	//{ 0.0f, 0.0f, 1.0f, 0.0f },
	//{ 0.0f, 0.0f, 1.0f, 1.0f }
	//};

	//GLuint loc = glGetUniformLocation(mMeshShaderProgram, "uViewProj");
	//// Send the matrix data to the uniform
	//Eigen::Matrix4d viewProj = mView * mProjection;
	//glUniformMatrix4fv(loc, 1, GL_TRUE, reinterpret_cast<const float*>(viewProj.data()));

	return true;
}

static void LoadTexture(std::string texFilePath, GLuint& texture)
{
	// Load from file
	SDL_Surface* surf = IMG_Load(texFilePath.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", texFilePath.c_str());
		return;
	}

	// Create texture from surface
	//tex = SDL_CreateTextureFromSurface(mCommonData->mRenderer, surf);

	//if (!tex)
	//{
	//	SDL_Log("Failed to convert surface to texture for %s", filePath.c_str());
	//	return false;
	//}
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(surf);
	glBindTexture(texture, 0);		// unbind
}

bool YGame::LoadData()
{
	// obj fileを読みだす
	FILE* file = fopen(".\\resources\\cube2.obj", "r");
	if (file == NULL) {
		printf("error: failed to open mesh file\n");
		return false;
	}

	std::vector<float>CubeVertices;
	std::vector<float>CubeUV;
	std::vector<float>CubeNormals;
	std::vector<int>CubeVertexI;
	std::vector<int>CubeUVI;
	std::vector<int>CubeNormalI;
	std::vector<glm::ivec3>FaceVec;
	std::vector<glm::ivec3>RFaceVec(0);		// 重複したindexが除去された配列

	while (1) {

		char lineHeader[128];
		// 行の最初の文字列を読み込みます。
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

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
			Eigen::Vector3i vertexIndex;
			Eigen::Vector3i uvIndex;
			Eigen::Vector3i normalIndex;
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&vertexIndex.x(), &uvIndex.x(), &normalIndex.x(),
				&vertexIndex.y(), &uvIndex.y(), &normalIndex.y(),
				&vertexIndex.z(), &uvIndex.z(), &normalIndex.z()
			);
			CubeVertexI.push_back(vertexIndex.x() - 1);
			CubeVertexI.push_back(vertexIndex.y() - 1);
			CubeVertexI.push_back(vertexIndex.z() - 1);
			CubeUVI.push_back(uvIndex.x() - 1);
			CubeUVI.push_back(uvIndex.y() - 1);
			CubeUVI.push_back(uvIndex.z() - 1);
			CubeNormalI.push_back(normalIndex.x() - 1);
			CubeNormalI.push_back(normalIndex.y() - 1);
			CubeNormalI.push_back(normalIndex.z() - 1);
			glm::ivec3 a1, a2, a3;
			a1[0] = vertexIndex.x();	a1[1] = uvIndex.x();	a1[2] = normalIndex.x();
			a2[0] = vertexIndex.y();	a2[1] = uvIndex.y();	a2[2] = normalIndex.y();
			a3[0] = vertexIndex.z();	a3[1] = uvIndex.z();	a3[2] = normalIndex.z();
			FaceVec.push_back(a1);
			FaceVec.push_back(a2);
			FaceVec.push_back(a3);
		}
	}

	// データ構造を変更する処理
	// FaceVecの重複indexを排除
	for (int i = 0; i < FaceVec.size(); i++) {
		bool IsHaving = false;
		for (int j = 0; j < RFaceVec.size(); j++) {
			if (FaceVec[i] == RFaceVec[j]) {
				IsHaving = true;
			}
		}
		if (!IsHaving) {
			RFaceVec.push_back(FaceVec[i]);
		}
	}

	// 各indexに対応する新しいindex作成
	std::map<glm::ivec3, int> vec_i_map;
	std::vector<int> newIndex;
	for (int i = 0; i < FaceVec.size(); i++) {
		int Idx = -1;
		for (int j = 0; j < RFaceVec.size(); j++) {
			if (FaceVec[i] == RFaceVec[j]) {
				Idx = j;
				break;
			}
		}
		newIndex.push_back(Idx);
	}

	// RFaceVecに対応する vertices作成
	std::vector<float>newVertices;
	for (int i = 0; i < RFaceVec.size(); i++) {
		glm::ivec3 this_vec = RFaceVec[i];
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


	//mNumCubeIndicies = CubeVertexI.size();
	mNumCubeIndicies = newIndex.size();


	// Cubeのvertex arrayを設定
	glUseProgram(mMeshShaderProgram);
	glGenVertexArrays(1, &mCubeVertexArray);
	glBindVertexArray(mCubeVertexArray);

	glGenBuffers(1, &mCubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mCubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), newVertices.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mCubeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCubeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newIndex.size() * sizeof(int), newIndex.data(), GL_STATIC_DRAW);

	// link attribution
	glBindBuffer(GL_ARRAY_BUFFER, mCubeVertexBuffer);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	// unbind cube vertex arrays
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// CubeのTextureを設定
	// textureを読みだす。
	LoadTexture(".\\resources\\brick.png", mCubeTexture);
	//SDL_Texture* tex = nullptr;
	//std::string filePath = ".\\resources\\brick.png";
	//// Load from file
	//SDL_Surface* surf = IMG_Load(filePath.c_str());
	//if (!surf)
	//{
	//	SDL_Log("Failed to load texture file %s", filePath.c_str());
	//	return false;
	//}

	//// Create texture from surface
	////tex = SDL_CreateTextureFromSurface(mCommonData->mRenderer, surf);
	//
	////if (!tex)
	////{
	////	SDL_Log("Failed to convert surface to texture for %s", filePath.c_str());
	////	return false;
	////}
	//glGenTextures(1, &mCubeTexture);
	//glActiveTexture(GL_TEXTURE);
	//glBindTexture(GL_TEXTURE_2D, mCubeTexture);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//// Configures the way the texture repeats (if it does at all)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
	//// Generates MipMaps
	//glGenerateMipmap(GL_TEXTURE_2D);

	//SDL_FreeSurface(surf);
	//glBindTexture(mCubeTexture, 0);		// unbind

	// lightingの設定
	mAmbientLightColor = glm::vec3(0.5, 0.5, 0.5);
	mDirectionalLight.direction = glm::vec3(0, -0.707, -0.707);
	mDirectionalLight.diffuseColor = glm::vec3(0.78, 0.88, 1);
	mDirectionalLight.specColor = glm::vec3(0.8, 0.2, 0.8);


	// spriteのvertex arrayの設定
	glUseProgram(mSpriteShaderProgram);
	glGenVertexArrays(1, &mSpriteVertexArray);
	glBindVertexArray(mSpriteVertexArray);

	glGenBuffers(1, &mSpriteVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mSpriteVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertices), SpriteVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mSpriteIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSpriteIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SpriteIndices), SpriteIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mCubeVertexBuffer);
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

	mSpritePos = glm::vec3(0, 0, 0);


	return true;
}

void YGame::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:	// マウスのボタンが押されたら
		{
			if (mPhase == PHASE_IDLE) {
				//mLastMousePos = mMousePos;
				//mSwipingDropPos = mMousePos / GRID_SIZE;
				mPhase = PHASE_MOVE;
			}
		}
		break;
		case SDL_MOUSEBUTTONUP:		// マウスを離したら
			if (mPhase == PHASE_MOVE) {
				mPhase = PHASE_IDLE;

				/*if (EraseDrops()) {
					phase = PHASE_ERASE;
				}
				else {
					phase = PHASE_IDLE;
				}*/
			}
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escapeキーを押下すると終了
	{
		mIsRunning = false;
	}
}

void YGame::ComputeWorldTransform()
{
	if (mReComputeWorldTransform == true) {
		//mReComputeWorldTransform = false;

		//mCubeRotation += 0.001f;
		mCubeWorldTrans = glm::rotate(mCubeWorldTrans, glm::radians(mCubeRotation), glm::vec3(0.0f, 1.0f, 0.0f));
		

		//Eigen::Translation3d trans = Eigen::Translation3d(mCubePos);
		//Eigen::Matrix3d scaleMat = Eigen::Matrix3d::Identity();
		//scaleMat *= mCubeScale;
		//Eigen::Affine3d mat;
		//mat = trans * mCubeRot * scaleMat;
		//mCubeWorldTrans = mat.matrix();

		//Eigen::Vector3d pos;
		//pos << 200, 0, 0;
		//trans = Eigen::Translation3d(pos);
		//mat = trans;
		//mCubeWorldTrans = mat.matrix();

		//mat = mCubeWorldTrans.transpose();
		//mCubeWorldTrans = mat.matrix();
	}
}


void YGame::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	mCubeRotation = deltaTime * mCubeRotateVel * 10;
	/*if (deltaTime > 0.015) {
		mCubeRotation += 0.0001f;
	}*/
	//std::string out_str = std::to_string(clock()) + "\t" + std::to_string(mCubeRotation) + "\n";
	//fprintf(fp, out_str.c_str());

	//std::cout << clock() - last << std::endl;
	last = clock();
	ComputeWorldTransform();	// Cubeのtransformを計算
}

void YGame::Draw()
{
	// Set the clear color to light grey
	//glClearColor(0.6f, 0.2f, 0.7f, 1.0f);
	glClearColor(0, 0.5, 0.7, 1.0f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// Enable depth buffering/disable alpha blend
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// draw meshes
	glUseProgram(mMeshShaderProgram);	// activate
	// bindcube vertex array
	glBindVertexArray(mCubeVertexArray);

	SetMatrixUniform("model", mCubeWorldTrans, mMeshShaderProgram);	// cubeの座標を反映

	// カメラの位置を反映
	glm::vec3 camera_pos = glm::vec3(mView[3].x, mView[3].y, mView[3].z);
	SetVectorUniform("uCameraPos", camera_pos, mMeshShaderProgram);
	// set lighting
	SetVectorUniform("uAmbientLight", mAmbientLightColor, mMeshShaderProgram);
	SetVectorUniform("uDirLight.mDirection", mDirectionalLight.direction, mMeshShaderProgram);
	SetVectorUniform("uDirLight.mDiffuseColor", mDirectionalLight.diffuseColor, mMeshShaderProgram);
	SetVectorUniform("uDirLight.mSpecColor", mDirectionalLight.specColor, mMeshShaderProgram);

	// cubeのspecular power
	{
		GLuint loc = glGetUniformLocation(mMeshShaderProgram, "uSpecPower");
		glUniform1f(loc, 100.0f);
	}

	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(mMeshShaderProgram, "tex0");
	// Sets the value of the uniform
	glUniform1i(texUni, 0);

	// bind cube texture
	glBindTexture(GL_TEXTURE_2D, mCubeTexture);

	// draw
	glDrawElements(GL_TRIANGLES, mNumCubeIndicies, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);


	// --- draw sprites ---
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	glUseProgram(mSpriteShaderProgram);
	glBindVertexArray(mSpriteVertexArray);
	
	// spriteの描画位置を設定
	glm::mat4 SpritePos = glm::translate(glm::mat4(1.0f), mSpritePos);
	SetMatrixUniform("uWorldTransform", SpritePos, mSpriteShaderProgram);	// cubeの座標を反映
	glBindTexture(GL_TEXTURE_2D, mCubeTexture);
	glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

	SDL_GL_SwapWindow(mWindow);
}

void YGame::RunLoop()
{
	last = clock();
	//if ((fp = fopen("out.txt", "w")) == NULL) {
	//	printf("error: failed open file\n");
	//	exit(-1);
	//}

	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		Draw();
	}
	//fclose(fp);
}

void YGame::Shutdown()
{

}