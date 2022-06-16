#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include "YGame.hpp"
#include <time.h>
#include "GLUtil.hpp"

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

//float skyboxVertices[] =
//{
//	//   Coordinates
//	-1.0f, -1.0f,  1.0f,//        7--------6
//	 1.0f, -1.0f,  1.0f,//       /|       /|
//	 1.0f, -1.0f, -1.0f,//      4--------5 |
//	-1.0f, -1.0f, -1.0f,//      | |      | |
//	-1.0f,  1.0f,  1.0f,//      | 3------|-2
//	 1.0f,  1.0f,  1.0f,//      |/       |/
//	 1.0f,  1.0f, -1.0f,//      0--------1
//	-1.0f,  1.0f, -1.0f
//};

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f, -1.0f,//      | |      | |
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f, 
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f,  1.0f,  1.0f,//      |/       |/
	-1.0f,  1.0f,  1.0f//      | 3------|-2
};



unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

//unsigned int skyboxIndices[] =
//{
//	// Right
//	6, 2, 1,
//	1, 5, 6,
//	// Left
//	0, 4, 7,
//	7, 3, 0,
//	// Front
//	4, 5, 6,
//	6, 7, 4,
//	// Back
//	0, 3, 2,
//	2, 1, 0,
//	// Top
//	0, 1, 5,
//	5, 4, 0,
//	// Bottom
//	3, 7, 6,
//	6, 2, 3
//};



//unsigned int skyboxIndices[] =
//{
//	// Right
//	1, 2, 6,
//	6, 5, 1,
//	// Left
//	0, 4, 7,
//	7, 3, 0,
//	// Top
//	0, 1, 5,
//	5, 4, 0,
//	// Bottom
//	3, 7, 6,
//	6, 2, 3,
//	// Back
//	0, 3, 2,
//	2, 1, 0,
//	// Front
//	4, 5, 6,
//	6, 7, 4
//};



//float SpriteVertices[] = {
//	-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
//	0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
//	0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
//	-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
//};
//
//unsigned int SpriteIndices[] = {
//	0, 1, 2,
//	2, 3, 0
//};

// default
//float SpriteVertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
//	-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
//	 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
//	 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
//};

// 2d view
//float SpriteVertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
//	0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
//	-0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
//	-0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
//};

//static float SpriteVertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
//	0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
//	0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
//	-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
//};
static float SpriteVertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f, // top left
	0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f,	// top right
	0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f,	// bottom right
	-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f	// bottom left
};


// uv : tetureのうち、どの領域を使うか？を指定
//static float SpriteVertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
//	0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.5f, 0.f, // top right
//	0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.5f, 1.f, // bottom right
//	-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
//};




// default
//unsigned int SpriteIndices[] =
//{
//	0, 2, 1, // Upper triangle
//	0, 3, 2 // Lower triangle
//};

static unsigned int SpriteIndices[] =
{
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
	mPhase(PHASE_IDLE),
	mMoveSpeed(0.1),
	mMoveSensitivity(100.0f)
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

	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}

	setlocale(LC_CTYPE, "");

	// FREE TYPE
	FT_Library library;
	//FT_Face face;
	FT_GlyphSlot slot;
	// Load Font
	FT_Init_FreeType(&library);
	//FT_New_Face(library, ".\\resources\\Carlito-Regular.ttf", 0, &mFontFace);
	FT_New_Face(library, ".\\resources\\arialuni.ttf", 0, &mFontFace);
	FT_Select_Charmap(mFontFace, ft_encoding_unicode);
	//FT_Select_Charmap(mFontFace, ft_encoding_sjis);
	//FT_Select_Charmap（m_face、FT_ENCODING_UNICODE）;
	FT_Set_Pixel_Sizes(mFontFace, 0, 48);
	slot = mFontFace->glyph;

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


bool YGame::LoadShaders()
{
	// Sprite
	{
		std::string vertFile = "./Shaders/Sprite.vert";
		std::string fragFile = "./Shaders/Sprite.frag";
		mSpriteShaderProgram = new Shader();
		if (!mSpriteShaderProgram->CreateShaderProgram(vertFile, fragFile)) {
			return false;
		}
	}


	// Text
	{
		std::string vert_file = "./Shaders/Text.vert";
		std::string frag_file = "./Shaders/Text.frag";
		mTextShaderProgram = new Shader();
		if (!mTextShaderProgram->CreateShaderProgram(vert_file, frag_file)) {
			return false;
		}
	}

	glm::mat4 spriteViewProj = glm::mat4(1.0f);
	spriteViewProj[0][0] = 2.0f / (float)mWindowWidth;
	spriteViewProj[1][1] = 2.0f / (float)mWindowHeight;
	spriteViewProj[3][2] = 1.0f;

	mSpriteShaderProgram->UseProgram();
	mSpriteShaderProgram->SetMatrixUniform("uViewProj", spriteViewProj);
	mTextShaderProgram->UseProgram();
	mTextShaderProgram->SetMatrixUniform("uViewProj", spriteViewProj);


	// Mesh
	{
		std::string vert_file = "./Shaders/YPhong.vert";
		std::string frag_file = "./Shaders/YPhong.frag";
		mMeshShaderProgram = new Shader();
		if (!mMeshShaderProgram->CreateShaderProgram(vert_file, frag_file)) {
			return false;
		}
	}



	// View Projection行列を設定する
	// View行列の計算
	glm::mat4 mProjection = glm::mat4(1.0f);
	//mView = glm::translate(mView, glm::vec3(0.0f, 0, 0));
	mProjection = glm::perspective(glm::radians(45.0f), (float)mWindowWidth / mWindowHeight, 0.1f, 100.0f);
	mCubeWorldTrans = glm::mat4(1.0);
	mCubeWorldTrans = glm::translate(mCubeWorldTrans, glm::vec3(0.0f, 35.0f, 0.0f));

	glm::mat4 view2 = glm::lookAt(
		mCameraPos,
		mCameraPos + mCameraOrientation,
		mCameraUP);


	// Uniform の初期値
	// Mesh
	mMeshShaderProgram->SetMatrixUniform("view", view2);
	mMeshShaderProgram->SetMatrixUniform("proj", mProjection);
	mMeshShaderProgram->SetMatrixUniform("model", mCubeWorldTrans);


	// 3DText Shader読み込み
	{
		std::string vert_file = "./Shaders/3DText.vert";
		std::string frag_file = "./Shaders/3DText.frag";
		m3DTextShaderProgram = new Shader();
		if (!m3DTextShaderProgram->CreateShaderProgram(vert_file, frag_file)) {
			return false;
		}
	}
	m3DTextShaderProgram->UseProgram();
	m3DTextShaderProgram->SetMatrixUniform("view", view2);
	m3DTextShaderProgram->SetMatrixUniform("proj", mProjection);
	m3DTextShaderProgram->SetMatrixUniform("uWorldTransform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 35.0f, 0.0f)));
	m3DTextShaderProgram->SetMatrixUniform("uRotate", glm::mat4(1.0f));


	// Sky Box Shader読み込み
	{
		std::string vert_file = "./Shaders/SkyBox.vert";
		std::string frag_file = "./Shaders/SkyBox.frag";
		mSkyBoxShaderProgram = new Shader();
		if (!mSkyBoxShaderProgram->CreateShaderProgram(vert_file, frag_file)) {
			return false;
		}
	}
	mSkyBoxShaderProgram->UseProgram();
	mSkyBoxShaderProgram->SetMatrixUniform("uVew", view2);
	mSkyBoxShaderProgram->SetMatrixUniform("uProj", mProjection);
	{
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		mSkyBoxShaderProgram->SetMatrixUniform("uRot", rot);
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
		mSkyBoxShaderProgram->SetMatrixUniform("uTrans", trans);
	}


	
	return true;
}

bool YGame::LoadData()
{
	// CubeのTextureを設定
	// textureを読みだす。
	mCubeTexture = new Texture(".\\resources\\brick.png");
	//LoadTexture(".\\resources\\brick.png", mCubeTexture, cube_w, cube_h);

	// lightingの設定
	mAmbientLightColor = glm::vec3(0.5, 0.5, 0.5);
	mDirectionalLight.direction = glm::vec3(0, -0.707, -0.707);
	mDirectionalLight.diffuseColor = glm::vec3(0.78, 0.88, 1);
	mDirectionalLight.specColor = glm::vec3(0.8, 0.2, 0.8);


	mMeshShaderProgram->SetVectorUniform("uAmbientLight", mAmbientLightColor);
	mMeshShaderProgram->SetVectorUniform("uDirLight.mDirection", mDirectionalLight.direction);
	mMeshShaderProgram->SetVectorUniform("uDirLight.mDiffuseColor", mDirectionalLight.diffuseColor);
	mMeshShaderProgram->SetVectorUniform("uDirLight.mSpecColor", mDirectionalLight.specColor);

	// Meshを読み込む
	{
		Mesh* mesh = new Mesh("./resources/", "cube2.obj", mMeshShaderProgram, glm::vec3(0, -0.707, -0.707));
		mesh->SetMeshPos(glm::vec3(10.0f, 35.0f, 0.0f));
		mesh->SetMeshRotate(glm::mat4(1.0f));
		mesh->SetMeshScale(1.0f);
		mMeshes.push_back(mesh);
	}

	{
		Mesh* mesh = new Mesh("./resources/SimpleObj2/", "cubes.obj", mMeshShaderProgram, glm::vec3(0, -0.707, -0.707));
		mesh->SetMeshPos(glm::vec3(-10.0f, 35.0f, 0.0f));
		mesh->SetMeshRotate(glm::mat4(1.0f));
		mesh->SetMeshScale(1.0f);
		mMeshes.push_back(mesh);
	}

	{
		Mesh* mesh = new Mesh("./resources/SimpleObj3/", "cubes.obj", mMeshShaderProgram, glm::vec3(0, -0.707, -0.707));
		mesh->SetMeshPos(glm::vec3(-20.0f, 35.0f, 0.0f));
		mesh->SetMeshRotate(glm::mat4(1.0f));
		mesh->SetMeshScale(1.0f);
		mMeshes.push_back(mesh);
	}

	{
		Mesh* mesh = new Mesh("./resources/shredder/", "shredder.obj", mMeshShaderProgram, glm::vec3(0, -0.707, -0.707));
		mesh->SetMeshPos(glm::vec3(20.0f, 35.0f, 0.0f));
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(1.0, 0.0f, 0.0f));
		mesh->SetMeshRotate(rotMat);
		mesh->SetMeshScale(1.0f);
		mMeshes.push_back(mesh);
	}
	{
		Mesh* mesh = new Mesh("./resources/TreasureBox/", "TreasureBox.obj", mMeshShaderProgram, glm::vec3(0, -0.707, -0.707));
		mesh->SetMeshPos(glm::vec3(30.0f, 35.0f, 0.0f));
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(1.0, 0.0f, 0.0f));
		mesh->SetMeshRotate(rotMat);
		mesh->SetMeshScale(1.0f);
		mMeshes.push_back(mesh);
	}



	//mCubeMesh = new Mesh("./resources/", "cube2.obj", mMeshShaderProgram);
	//mCubeMesh->SetMeshPos(glm::vec3(10.0f, 35.0f, 0.0f));
	//mCubeMesh->SetMeshRotate(glm::mat4(1.0f));
	//mCubeMesh->SetMeshScale(1.0f);
	//mCubeMesh->SetMeshLightDirection(glm::vec3(0, -0.707, -0.707));

	//mDonutMesh = new Mesh("./resources/Donut/testDonut.obj", "./resources/Donut/testDonut.mtl", mMeshShaderProgram);
	//mDonutMesh->SetMeshPos(glm::vec3(-10.0f, 35.0f, 0.0f));
	//mDonutMesh->SetMeshRotate(glm::mat4(1.0f));
	//mDonutMesh->SetMeshScale(1.0f);
	//mDonutMesh->SetMeshLightDirection(glm::vec3(0, -0.707, -0.707));

	//mSphareMesh = new Mesh("./resources/sphare.obj", "./resources/sphare.mtl", mMeshShaderProgram);
	//mSphareMesh->SetMeshPos(glm::vec3(0.0f, 35.0f, 0.0f));
	//mSphareMesh->SetMeshRotate(glm::mat4(1.0f));
	//mSphareMesh->SetMeshScale(2.0f);
	//mSphareMesh->SetMeshLightDirection(glm::vec3(0, -0.707, -0.707));

	//mHouseWall = new Mesh("./resources/house2/", "HouseWall.obj", mMeshShaderProgram);
	//mHouseWall->SetMeshPos(glm::vec3(20.0f, 35.0f, 0.0f));
	//mHouseWall->SetMeshRotate(glm::mat4(1.0f));
	//mHouseWall->SetMeshScale(0.01f);
	//mHouseWall->SetMeshLightDirection(glm::vec3(0, -0.707, -0.707));
	//mHouseWallTexture = new Texture("./resources/house2/wall.png");

	//mDonut2Mesh = new Mesh("./resources/Donut2/", "Donut23.obj", mMeshShaderProgram);
	//mDonut2Mesh->SetMeshPos(glm::vec3(-20.0f, 35.0f, 0.0f));
	//mDonut2Mesh->SetMeshRotate(glm::mat4(1.0f));
	//mDonut2Mesh->SetMeshScale(10.0f);
	//mDonut2Mesh->SetMeshLightDirection(glm::vec3(0, -0.707, -0.707));






	// ドーナツのtextureを読み込む
	mDonutTexture = new Texture("./resources/Donut/testDonut.png");

	
	// Camera Settings
	mCameraPos = glm::vec3(0.0f);
	mCameraUP = glm::vec3(0.0f, 0.0f, 1.0f);
	mCameraOrientation = glm::vec3(0, 0.5f, 0);


	// spriteのvertex arrayの設定
	mSpriteShaderProgram->UseProgram();
	glGenVertexArrays(1, &mSpriteVertexArray);
	glBindVertexArray(mSpriteVertexArray);

	glGenBuffers(1, &mSpriteVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mSpriteVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertices), SpriteVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mSpriteIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSpriteIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SpriteIndices), SpriteIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mSpriteVertexBuffer);
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


	// Load Text Vertex Array
	mTextShaderProgram->UseProgram();
	glGenVertexArrays(1, &mTextVertexArray);
	glBindVertexArray(mTextVertexArray);

	glGenBuffers(1, &mTextVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mTextVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	// Load Atras Vertex Array
	mTextShaderProgram->UseProgram();
	glGenVertexArrays(1, &mAtrasVertexArray);
	glBindVertexArray(mAtrasVertexArray);

	glGenBuffers(1, &mAtrasVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mAtrasVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Load 3D Text VAO
	m3DTextShaderProgram->UseProgram();
	glGenVertexArrays(1, &m3DTextVertexArray);
	glBindVertexArray(m3DTextVertexArray);

	glGenBuffers(1, &m3DTextVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m3DTextVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	//mTestTexture = new Texture(".\\resources\\forest23.jpg");
	mTestTexture = new Texture(".\\resources\\forest23.jpg");
	//LoadTexture(".\\resources\\test_picture.png", mTestTexture, test_w, test_h);



	// Font の読み出し
	//mFont = TTF_OpenFont(".\\resources\\Carlito-Regular.ttf", 128);
	mFont = TTF_OpenFont(".\\resources\\VL-Gothic-Regular.ttf", 128);
	// 文字列とtextureのmap作成
	int font_color[] = {
		0x00, 0xdd, 0xdd, 255
	};

	// 文字列を取り出す
	 {
	 	std::string filePath = ".\\resources\\text_data.json";
	 	std::ifstream ifs(filePath.c_str());
	 	if (ifs.good())
	 	{
	 		ifs >> mTextData;
	 		/*for (const auto& elem : mTextData.items())
	 		{
	 			std::cout << elem.value() << std::endl;
	 		}*/
	 		//std::cout << j["project name"] << std::endl;
	 		//std::cout << j["intrinsics"]["cx"] << std::endl;
	 	}
	 }

	{
		Texture* tex = new Texture();
		std::string str;
		mTextData["Title"]["TitleText"]["ja"].get_to(str);
		tex->CreateTextTexture("PHASE_IDLE", font_color, mFont);
		mFontMap.insert(std::make_pair("PHASE_IDLE", tex));
	}
	{
		Texture* tex = new Texture();
		tex->CreateTextTexture("PHASE_MOVE", font_color, mFont);
		mFontMap.insert(std::make_pair("PHASE_MOVE", tex));
	}


	// Atras Textureを生成
	{
		std::string hello_str = "hello, world!";

		// 生成する必要のある文字を導出
		std::string create_str;
		for (int i = 0; i < hello_str.length(); i++) {
			char c = hello_str[i];
			bool is_have = false;
			for (int j = 0; j < create_str.length(); j++) {
				if (c == create_str[j]) {
					is_have = true;
					break;
				}
			}
			if (!is_have) {
				create_str += c;
			}
		}

		// Atras Textureの枠を生成
		// Atras Textureの大きさを導出
		for (int i = 0; i < create_str.length(); i++) {
			char c = create_str[i];
			if (FT_Load_Char(mFontFace, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				exit(-1);
			}
			AtrasWidth += mFontFace->glyph->bitmap.width;
			AtrasHeight = (AtrasHeight < mFontFace->glyph->bitmap.rows) ? mFontFace->glyph->bitmap.rows : AtrasHeight;
		}
		// Texture 生成
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &AtrasTex);
		glBindTexture(GL_TEXTURE_2D, AtrasTex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, AtrasWidth, AtrasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);


		// Atras Textureに文字データを書き込んでいく
		int x = 0;
		for (int i = 0; i < create_str.length(); i++) {
			char c = create_str[i];
			if (FT_Load_Char(mFontFace, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				exit(-1);
			}
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, mFontFace->glyph->bitmap.buffer);

			Character Char;
			auto glyph = mFontFace->glyph;
			Char.ax = glyph->advance.x;
			Char.ay = glyph->advance.y;
			Char.bw = glyph->bitmap.width;
			Char.bh = glyph->bitmap.rows;
			Char.bl = glyph->bitmap_left;
			Char.bt = glyph->bitmap_top;
			Char.tx = (float)x / (float)AtrasWidth;
			Characters.insert(std::make_pair(c, Char));

			x += mFontFace->glyph->bitmap.width;
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(AtrasTex, 0);		// unbind
	}

	{
		//const char* str = "BBC";
		//char c = str[0];
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		////FT_Load_Glyph(mFontFace, FT_Get_Char_Index(mFontFace, str[0]), FT_LOAD_RENDER);
		//if (FT_Load_Char(mFontFace, c, FT_LOAD_RENDER))
		//{
		//	std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		//	exit(-1);
		//}

		//
		//glGenTextures(1, &FontTex);
		//glActiveTexture(GL_TEXTURE);
		//glBindTexture(GL_TEXTURE_2D, FontTex);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows,
		//	0, GL_RED, GL_UNSIGNED_BYTE, mFontFace->glyph->bitmap.buffer);
		//mFontWidth = mFontFace->glyph->bitmap.width;
		//mFontHeight = mFontFace->glyph->bitmap.rows;

		//glGenerateMipmap(GL_TEXTURE_2D);
		//glBindTexture(FontTex, 0);		// unbind
	}


	// Load Text Data
	{
		// アルファベットすべて読み込む
		for (char c = 'a'; c <= 'z'; c++) {
			{	// 小文字
				GLuint tex;
				TexChar ch = LoadChar(c);
				mTexChars.insert(std::make_pair(c, ch));
			} 
			{	// 大文字
				GLuint tex;
				char c2 = (char)(c - 'a' + 'A');
				TexChar ch = LoadChar(c2);
				mTexChars.insert(std::make_pair(c2, ch));
			}
		}
		mTexChars.insert(std::make_pair(',', LoadChar(',')));
		mTexChars.insert(std::make_pair(' ', LoadChar(' ')));
		mTexChars.insert(std::make_pair('_', LoadChar('_')));
		mTexChars.insert(std::make_pair('!', LoadChar('!')));
		mTexChars.insert(std::make_pair('?', LoadChar('?')));
	}

	// 日本語表示
	{
		std::string str;
		mTextData["WanderOpenGL"]["test"].get_to(str);
		FT_ULong Char = wchar_t(str.c_str());
		FT_UInt char_index = FT_Get_Char_Index(mFontFace, Char);
		if (FT_Load_Glyph(mFontFace, char_index, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			exit(-1);
		}
		glGenTextures(1, &FontTex);
		glActiveTexture(GL_TEXTURE);
		glBindTexture(GL_TEXTURE_2D, FontTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, mFontFace->glyph->bitmap.buffer);
		mFontWidth = mFontFace->glyph->bitmap.width;
		mFontHeight = mFontFace->glyph->bitmap.rows;

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(FontTex, 0);		// unbind
	}

	// 日本語文字列textures作成
	{
		std::u16string u16str = u"あいうえお";
		//u16str << "abc";
		//char16_t str[] = u"あいうえお";
		const char16_t str[] = u"楠本崚介";
		//const char16_t* str = u16str.c_str();
		//std::string str;
		//mTextData["WanderOpenGL"]["test"].get_to(str);

		for (int i = 0; str[i] != '\0'; i++) {
			//if (FT_Load_Glyph(mFontFace, FT_Get_Char_Index(mFontFace, str[i]), FT_LOAD_RENDER)) {
			//	std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			//	exit(-1);
			//}
			//GLuint tex;
			//glGenTextures(1, &tex);
			//glActiveTexture(GL_TEXTURE);
			//glBindTexture(GL_TEXTURE_2D, tex);
			//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows,
			//	0, GL_RED, GL_UNSIGNED_BYTE, mFontFace->glyph->bitmap.buffer);
			//mFontWidth = mFontFace->glyph->bitmap.width;
			//mFontHeight = mFontFace->glyph->bitmap.rows;

			//TexChar	 tc = {
			//	tex,
			//	glm::ivec2(mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows),
			//	glm::ivec2(mFontFace->glyph->bitmap_left, mFontFace->glyph->bitmap_top),
			//	mFontFace->glyph->advance.x
			//};

			//glGenerateMipmap(GL_TEXTURE_2D);
			//glBindTexture(tex, 0);		// unbind

			TexChar tc = LoadUTFChar(str[i]);

			mJapanTexVec.push_back(tc);
			mJapanTexChars.insert(std::make_pair(str[i], tc));
			//mTextStr.push_back(tex);
		}
	}
	//char16_t si[] = u"あいうえお";


	// SkyBox
	// Sky BoxのVertex Arrayを読み込む
	glGenVertexArrays(1, &mSkyBoxVertexArray);
	glBindVertexArray(mSkyBoxVertexArray);

	glGenBuffers(1, &mSkyBoxVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mSkyBoxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mSkyBoxIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSkyBoxIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mSkyBoxVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// unbind cube vertex arrays
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	
	// Sky BoxのTextureを読み込む
	{
		//std::vector<std::string> facesCubemap =
		//{
		//	"./resources/skybox/right.jpg",
		//	"./resources/skybox/left.jpg",
		//	"./resources/skybox/top.jpg",
		//	"./resources/skybox/bottom.jpg",
		//	"./resources/skybox/front.jpg",
		//	"./resources/skybox/back.jpg"
		//};
		//std::vector<std::string> facesCubemap =
		//{
		//	"./resources/starbox/right.png",
		//	"./resources/starbox/left.png",
		//	"./resources/starbox/top.png",
		//	"./resources/starbox/bottom.png",
		//	"./resources/starbox/front.png",
		//	"./resources/starbox/back.png"
		//};

		//std::vector<std::string> facesCubemap =
		//{
		//	"./resources/mountainBox/right.png",
		//	"./resources/mountainBox/left.png",
		//	"./resources/mountainBox/top.png",
		//	"./resources/mountainBox/bottom.png",
		//	"./resources/mountainBox/front.png",
		//	"./resources/mountainBox/back.png"
		//};
		std::vector<std::string> facesCubemap =
		{
			"./resources/StarWarsSkyBox/right.png",
			"./resources/StarWarsSkyBox/left.png",
			"./resources/StarWarsSkyBox/top.png",
			"./resources/StarWarsSkyBox/bottom.png",
			"./resources/StarWarsSkyBox/front.png",
			"./resources/StarWarsSkyBox/back.png"
		};


		//std::vector<std::string> facesCubemap =
		//{
		//	"./resources/skybox/right.jpg",
		//	"./resources/skybox/left.jpg",
		//	"./resources/skybox/front.jpg",
		//	"./resources/skybox/back.jpg",
		//	"./resources/skybox/top.jpg",
		//	"./resources/skybox/bottom.jpg"
		//};


		mSkyBoxTexture = new Texture(facesCubemap);
	}

	//unsigned int skyboxIndices[] =
	//{
	//	// Right
	//	1, 2, 6,
	//	6, 5, 1,
	//	// Left
	//	0, 4, 7,
	//	7, 3, 0,
	//	// Front
	//	4, 5, 6,
	//	6, 7, 4,
	//	// Back
	//	0, 3, 2,
	//	2, 1, 0,
	//	// Top
	//	0, 1, 5,
	//	5, 4, 0,
	//	// Bottom
	//	3, 7, 6,
	//	6, 2, 3
	//};



	return true;
}

YGame::TexChar YGame::LoadChar(char c)
{
	if (FT_Load_Char(mFontFace, c, FT_LOAD_RENDER))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		exit(-1);
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows,
		0, GL_RED, GL_UNSIGNED_BYTE, mFontFace->glyph->bitmap.buffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(tex, 0);		// unbind

	TexChar	 tc = {
		tex,
		glm::ivec2(mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows),
		glm::ivec2(mFontFace->glyph->bitmap_left, mFontFace->glyph->bitmap_top),
		mFontFace->glyph->advance.x
	};

	return tc;
}

YGame::TexChar YGame::LoadUTFChar(char16_t c)
{
	if (FT_Load_Glyph(mFontFace, FT_Get_Char_Index(mFontFace, c), FT_LOAD_RENDER)) {
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		exit(-1);
	}
	GLuint tex;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows,
		0, GL_RED, GL_UNSIGNED_BYTE, mFontFace->glyph->bitmap.buffer);
	mFontWidth = mFontFace->glyph->bitmap.width;
	mFontHeight = mFontFace->glyph->bitmap.rows;

	TexChar	 tc = {
		tex,
		glm::ivec2(mFontFace->glyph->bitmap.width, mFontFace->glyph->bitmap.rows),
		glm::ivec2(mFontFace->glyph->bitmap_left, mFontFace->glyph->bitmap_top),
		mFontFace->glyph->advance.x
	};

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(tex, 0);		// unbind

	return tc;
}

void YGame::ProcessInput()
{
	SDL_Point mouse_position = { mWindowWidth / 2, mWindowHeight / 2 };
	SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
	mMousePos.x = mouse_position.x;
	mMousePos.y = mouse_position.y;
	//printf("%d %d\n", mMousePos.x, mMousePos.y);

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
				
				SDL_WarpMouseInWindow(mWindow, mWindowWidth / 2, mWindowHeight / 2);
				mMousePos.x = mWindowWidth / 2;
				mMousePos.y = mWindowHeight / 2;
				SDL_ShowCursor(SDL_DISABLE);
				std::cout << "----------------------------------------------called\n";
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
				SDL_ShowCursor(SDL_ENABLE);
			}
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escapeキーを押下すると終了
	{
		mIsRunning = false;
	}

	if (keyState[SDL_SCANCODE_W]) {	
		mCameraPos += (float)mMoveSpeed * mCameraOrientation;
	}
	if (keyState[SDL_SCANCODE_S]) {
		mCameraPos -= (float)mMoveSpeed * mCameraOrientation;
	}
	if (keyState[SDL_SCANCODE_A]) {
		mCameraPos -= (float)mMoveSpeed * glm::normalize(glm::cross(mCameraOrientation, mCameraUP));
	}
	if (keyState[SDL_SCANCODE_D]) {
		mCameraPos += (float)mMoveSpeed * glm::normalize(glm::cross(mCameraOrientation, mCameraUP));
	}
}

void YGame::ComputeWorldTransform()
{
	if (mReComputeWorldTransform == true) {
		//mReComputeWorldTransform = false;

		//mCubeRotation += 0.001f;
		mCubeWorldTrans = glm::rotate(mCubeWorldTrans, glm::radians(mCubeRotation), glm::vec3(0.0f, 1.0f, 1.0f));
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

	last = clock();
	ComputeWorldTransform();	// Cubeのtransformを計算

	if (mPhase == PHASE_MOVE) {
		//printf("%d %d\n", mMousePos.x, mMousePos.y);

		float rotX = mMoveSensitivity * (float)((float)mMousePos.y - ((float)mWindowHeight / 2.0f)) / (float)mWindowHeight;
		float rotY = mMoveSensitivity * (float)((float)mMousePos.x - ((float)mWindowWidth / 2.0f)) / (float)mWindowWidth;
		printf("rotX: %f rotY: %f\t", rotX, rotY);
		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(mCameraOrientation, glm::radians(-rotX), glm::normalize(glm::cross(mCameraOrientation, mCameraUP)));

		// Decides whether or not the next vertical Orientation is legal or not
		int rad = abs(glm::angle(newOrientation, mCameraUP) - glm::radians(90.0f));
		std::cout << rad * 180 / M_PI << std::endl;
		if (abs(glm::angle(newOrientation, mCameraUP) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			mCameraOrientation = newOrientation;
		}

		// Rotates the Orientation left and right
		mCameraOrientation = glm::rotate(mCameraOrientation, glm::radians(-rotY), mCameraUP);

		if ((mMousePos.x != mWindowWidth / 2) || (mMousePos.y != mWindowHeight / 2)) {
			SDL_WarpMouseInWindow(mWindow, mWindowWidth / 2, mWindowHeight / 2);
		}
	}
}


void YGame::SetSpritePos(glm::vec3 spritePos, Texture* tex, float scale, float rotation, float alpha)
{
	glm::mat4 spriteViewProj = glm::mat4(1.0f);
	spriteViewProj[0][0] = 2.0f / (float)mWindowWidth;
	spriteViewProj[1][1] = 2.0f / (float)mWindowHeight;
	spriteViewProj[3][2] = 1.0f;

	//mSpriteShaderProgram->UseProgram();
	mSpriteShaderProgram->SetMatrixUniform("uViewProj", spriteViewProj);

	// Sprite Translation Matrix
	glm::mat4 SpriteTrans = glm::mat4(1.0f);
	SpriteTrans = glm::translate(glm::mat4(1.0f), spritePos);

	// spriteのscaling matrix
	glm::vec3 sprite_scale_vec = glm::vec3((float)tex->getWidth(), (float)tex->getHeight(), 1.0f);
	sprite_scale_vec *= scale;
	glm::mat4 SpriteScaling = glm::scale(glm::mat4(1.0f), sprite_scale_vec);

	// spriteのrotation matrix
	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1.0f));
	//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

	// Textureのalpha値を設定
	mSpriteShaderProgram->SetFloatUniform("uSpriteAlpha", alpha);

	mSpriteShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);	// cubeの座標を反映
	mSpriteShaderProgram->SetMatrixUniform("uScaling", SpriteScaling);	// cubeの座標を反映
	mSpriteShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);	// cubeの座標を反映
}

void YGame::RenderText(std::string text, glm::vec3 pos, float scale)
{
	unsigned int vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	struct point {
		GLfloat x;
		GLfloat y;
		GLfloat s;
		GLfloat t;
	};
	int len = text.length();
	std::cout << len << std::endl;
	point* coords = new point[len];
	int n = 0;

	for (int i = 0; i < text.length(); i++) {
		char c = text[i];
		float x2 = pos.x + Characters[c].bl * scale;
		float y2 = -pos.y - Characters[c].bt * scale;
		float w = Characters[c].bw * scale;
		float h = Characters[c].bh * scale;

		pos.x += Characters[c].ax * scale;
		pos.y += Characters[c].ay * scale;

		if (!w || !h)
			continue;

		coords[n++] = { x2,     -y2    , Characters[c].tx,                                            0 };
		coords[n++] = { x2 + w, -y2    , Characters[c].tx + Characters[c].bw / AtrasWidth,   0 };
		coords[n++] = { x2,     -y2 - h, Characters[c].tx,                                          Characters[c].bh / AtrasHeight }; //remember: each glyph occupies a different amount of vertical space
		coords[n++] = { x2 + w, -y2    , Characters[c].tx + Characters[c].bw / AtrasWidth,   0 };
		coords[n++] = { x2,     -y2 - h, Characters[c].tx,                                          Characters[c].bh / AtrasHeight };
		coords[n++] = { x2 + w, -y2 - h, Characters[c].tx + Characters[c].bw / AtrasWidth,                 Characters[c].bh / AtrasHeight };
	}
	unsigned int vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, n);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	//glGenBuffers(1, &mSpriteVertexBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, mSpriteVertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertices), SpriteVertices, GL_STATIC_DRAW);
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	coords = nullptr;
	//free(coords);

	//delete(coords);
	
}

void YGame::RenderText2(std::string text, glm::vec3 pos, glm::vec3 color, float scale)
{
	//glUniform3f(glGetUniformLocation(mTextShaderProgram, "textColor"), color.x, color.y, color.z);
	mTextShaderProgram->SetVectorUniform("textColor", color);
	glActiveTexture(GL_TEXTURE0);

	for (auto c = text.begin(); c != text.end(); c++) {
		TexChar ch = mTexChars[*c];

		float xpos = pos.x + ch.Bearing.x * scale;
		float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float textVertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.texID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mTextVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		pos.x += (ch.Advance >> 6) * scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void YGame::DrawText(std::string text, glm::vec3 pos, glm::vec3 color, float scale, float rot)
{
	mTextShaderProgram->UseProgram();
	glBindVertexArray(mTextVertexArray);

	// 文字のtexcharの大きさを取得
	int TexWidth = 0;
	int TexHeight = 0;
	for (auto c = text.begin(); c != text.end(); c++) {
		auto itr = mTexChars.find(*c);
		if (itr == mTexChars.end()) {
			std::cout << "error: Tex Char hasnt been loaded yet\n";
			exit(-1);
		}
		TexChar ch = mTexChars[*c];
		TexWidth += ch.Size.x;
		TexHeight = (TexHeight < ch.Size.y) ? ch.Size.y : TexHeight;
	}

	glm::mat4 SpriteTrans = glm::mat4(1.0f);
	glm::vec3 TexOffset = glm::vec3((float)TexWidth / 2.0f, (float)TexHeight / 2.0f, 0);
	SpriteTrans = glm::translate(glm::mat4(1.0f), pos - TexOffset);


	// spriteのrotation matrix
	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 0.0f, 1.0f));
	//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

	//SetMatrixUniform("uWorldTransform", SpriteTrans, mTextShaderProgram);	// cubeの座標を反映
	mTextShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);
	//SetMatrixUniform("uScaling", SpriteScaling, mTextShaderProgram);	// cubeの座標を反映
	//SetMatrixUniform("uRotate", SpriteRotate, mTextShaderProgram);	// cubeの座標を反映
	mTextShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);

	//glUniform3f(glGetUniformLocation(mTextShaderProgram, "textColor"), color.x, color.y, color.z);
	mTextShaderProgram->SetVectorUniform("textColor", color);
	glActiveTexture(GL_TEXTURE0);

	int x2 = 0;
	int y2 = 0;
	for (auto c = text.begin(); c != text.end(); c++) {
		TexChar ch = mTexChars[*c];

		float xpos = x2 + ch.Bearing.x * scale;
		float ypos = y2 - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float textVertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.texID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mTextVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x2 += (ch.Advance >> 6) * scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void YGame::DrawText2(std::string text, glm::vec3 pos, glm::vec3 color, float scale, float rot)
{
	mTextShaderProgram->UseProgram();
	glBindVertexArray(mAtrasVertexArray);

	glm::mat4 SpriteTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0.0f, 1.0f));

	mTextShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);
	mTextShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);
	mTextShaderProgram->SetVectorUniform("textColor", color);

	pos = glm::vec3(0.0f);
	for (int i = 0; i < text.length(); i++) {
		char c = text[i];
		float x2 = pos.x + Characters[c].bl * scale;
		float y2 = -pos.y - Characters[c].bt * scale;
		float w = Characters[c].bw * scale;
		float h = Characters[c].bh * scale;

		pos.x += Characters[c].ax * scale;
		pos.y += Characters[c].ay * scale;

		if (!w || !h)
			continue;

		float textVertices[6][4] = {
			{ x2,     -y2    , Characters[c].tx, 0 },
			{ x2 + w, -y2    , Characters[c].tx + Characters[c].bw / AtrasWidth,   0 },
			{ x2,     -y2 - h, Characters[c].tx,                                          Characters[c].bh / AtrasHeight }, //remember: each glyph occupies a different amount of vertical space
			{ x2 + w, -y2    , Characters[c].tx + Characters[c].bw / AtrasWidth,   0 },
			{ x2,     -y2 - h, Characters[c].tx,                                          Characters[c].bh / AtrasHeight },
			{ x2 + w, -y2 - h, Characters[c].tx + Characters[c].bw / AtrasWidth,                 Characters[c].bh / AtrasHeight }
		};

		glBindTexture(GL_TEXTURE_2D, AtrasTex);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mAtrasVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void YGame::DrawUTF(std::u16string text, glm::vec3 pos, glm::vec3 color, float scale, float rot)
{
	mTextShaderProgram->UseProgram();
	glBindVertexArray(mTextVertexArray);

	glm::vec3 FontCenter = glm::vec3(0.0f);
	// 文字のtexcharの大きさを取得
	{
		int TexWidth = 0;
		int width = (mJapanTexChars.begin()->second.Advance >> 6) * scale;
		FontCenter.x = (width * text.length()) / 2.0f;
		FontCenter.y = width / 2.0f;
	}

	glm::mat4 SpriteTrans = glm::translate(glm::mat4(1.0f), pos - FontCenter);
	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0.0f, 1.0f));
	mTextShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);
	mTextShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);
	mTextShaderProgram->SetVectorUniform("textColor", color);


	glActiveTexture(GL_TEXTURE0);

	int x2 = 0;
	int y2 = 0;
	//float scale = 1.0f;
	const char16_t* str = text.c_str();
	for (int i = 0; str[i] != '\0'; i++) {
		auto itr = mJapanTexChars.find(str[i]);
		TexChar ch;
		if (itr == mJapanTexChars.end()) {		// まだ読み込まれていない文字なら
			ch = LoadUTFChar(str[i]);
			mJapanTexChars.insert(std::make_pair(str[i], ch));
		}
		else {
			ch = itr->second;
		}

		float xpos = x2 + ch.Bearing.x * scale;
		float ypos = y2 - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;


		float textVertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.texID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mTextVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x2 += (ch.Advance >> 6) * scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}


void YGame::Draw3DUTF(std::u16string text, glm::vec3 pos, glm::vec3 color, float scale, glm::mat4 rot)
{
	m3DTextShaderProgram->UseProgram();
	glBindVertexArray(m3DTextVertexArray);

	glm::vec3 FontCenter = glm::vec3(0.0f);
	// 文字のtexcharの大きさを取得
	{
		int TexWidth = 0;
		int width = (mJapanTexChars.begin()->second.Advance >> 6) * scale;
		FontCenter.x = (width * text.length()) / 2.0f;
		FontCenter.y = width / 2.0f;
	}

	glm::mat4 SpriteTrans = glm::translate(glm::mat4(1.0f), pos);
	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0.0f, 1.0f));
	m3DTextShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);
	m3DTextShaderProgram->SetMatrixUniform("uRotate", rot);
	m3DTextShaderProgram->SetVectorUniform("textColor", color);


	glActiveTexture(GL_TEXTURE0);

	int x2 = 0;
	int y2 = 0;
	//float scale = 1.0f;
	const char16_t* str = text.c_str();
	for (int i = 0; str[i] != '\0'; i++) {
		auto itr = mJapanTexChars.find(str[i]);
		TexChar ch;
		if (itr == mJapanTexChars.end()) {		// まだ読み込まれていない文字なら
			ch = LoadUTFChar(str[i]);
			mJapanTexChars.insert(std::make_pair(str[i], ch));
		}
		else {
			ch = itr->second;
		}

		float xpos = x2 + ch.Bearing.x * scale;
		float ypos = y2 - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;


		float textVertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.texID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m3DTextVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x2 += (ch.Advance >> 6) * scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
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
	mMeshShaderProgram->UseProgram();

	// カメラの位置情報を設定
	mMeshShaderProgram->SetVectorUniform("uCameraPos", mCameraPos);
	//glm::mat4 CameraView = glm::mat4(glm::mat3(glm::lookAt(
	//	mCameraPos,
	//	mCameraPos + mCameraOrientation,
	//	mCameraUP)));
	glm::mat4 CameraView = glm::lookAt(
		mCameraPos,
		mCameraPos + mCameraOrientation,
		mCameraUP
	);
	mMeshShaderProgram->SetMatrixUniform("view", CameraView);

	// bindcube vertex array

	// cubeを描画
	for (int i = 0; i < mMeshes.size(); i++) {
		mMeshShaderProgram->UseProgram();
		mMeshes[i]->Draw();
	}

	//glBindVertexArray(mCubeVertexArray);
	//mMeshShaderProgram->SetMatrixUniform("model", mCubeWorldTrans);
	//mMeshShaderProgram->SetFloatUniform("uSpecPower", 100.0f);
	//mCubeTexture->BindTexture();
	////glDrawElements(GL_TRIANGLES, mNumCubeIndicies, GL_UNSIGNED_INT, 0);
	//mCubeTexture->UnBindTexture();
	//glBindVertexArray(0);
	//{
	//	mMeshShaderProgram->UseProgram();
	//	mCubeMesh->BindVertexArray();
	//	mCubeMesh->SetMeshTransforms();
	//	mCubeMesh->SetMeshLightings();
	//	mCubeTexture->BindTexture();
	//	glDrawElements(GL_TRIANGLES, mCubeMesh->GetNumIndices(), GL_UNSIGNED_INT, 0);
	//	mCubeTexture->UnBindTexture();
	//	glBindVertexArray(0);
	//}


	//// Donutを描画
	//{
	//	mMeshShaderProgram->UseProgram();
	//	mDonutMesh->BindVertexArray();
	//	mDonutMesh->SetMeshTransforms();
	//	mDonutMesh->SetMeshLightings();
	//	mDonutTexture->BindTexture();
	//	glDrawElements(GL_TRIANGLES, mDonutMesh->GetNumIndices(), GL_UNSIGNED_INT, 0);
	//	mDonutTexture->UnBindTexture();
	//	glBindVertexArray(0);
	//}

	//// sphareを描画
	//{
	//	mMeshShaderProgram->UseProgram();
	//	mSphareMesh->BindVertexArray();
	//	mSphareMesh->SetMeshTransforms();
	//	mSphareMesh->SetMeshLightings();
	//	mCubeTexture->BindTexture();
	//	glDrawElements(GL_TRIANGLES, mSphareMesh->GetNumIndices(), GL_UNSIGNED_INT, 0);
	//	mCubeTexture->UnBindTexture();
	//	glBindVertexArray(0);
	//}

	//// house wallを描画
	//{
	//	mMeshShaderProgram->UseProgram();
	//	mHouseWall->BindVertexArray();
	//	mHouseWall->SetMeshTransforms();
	//	mHouseWall->SetMeshLightings();
	//	mHouseWallTexture->BindTexture();
	//	glDrawElements(GL_TRIANGLES, mHouseWall->GetNumIndices(), GL_UNSIGNED_INT, 0);
	//	mHouseWallTexture->UnBindTexture();
	//	glBindVertexArray(0);
	//}

	//{
	//	mMeshShaderProgram->UseProgram();
	//	mDonut2Mesh->BindVertexArray();
	//	mDonut2Mesh->SetMeshTransforms();
	//	mDonut2Mesh->SetMeshLightings();
	//	mDonutTexture->BindTexture();
	//	glDrawElements(GL_TRIANGLES, mDonut2Mesh->GetNumIndices(), GL_UNSIGNED_INT, 0);
	//	mDonutTexture->UnBindTexture();
	//	glBindVertexArray(0);
	//}


	{
		//mMeshShaderProgram->UseProgram();
		//glBindVertexArray(mSphareVertexArray);
		//glm::mat4 sphareTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 35.0f, 0.0f));
		//mMeshShaderProgram->SetMatrixUniform("model", sphareTrans);
		//mMeshShaderProgram->SetFloatUniform("uSpecPower", 100.0f);
		//mCubeTexture->BindTexture();
		//glDrawElements(GL_TRIANGLES, mNumSphareIndicies, GL_UNSIGNED_INT, 0);
		//mCubeTexture->UnBindTexture();
		//glBindVertexArray(0);
	}



	// カメラの位置を反映
	//glm::vec3 camera_pos = glm::vec3(mView[3].x, mView[3].y, mView[3].z);
	//glm::vec3 camera_pos = glm::vec3(0);

	// set lighting
	// cubeのspecular power

	// Gets the location of the uniform
	//GLuint texUni = glGetUniformLocation(mMeshShaderProgram, "tex0");
	// Sets the value of the uniform
	//glUniform1i(texUni, 0);

	// bind cube texture


	// draw sky boxes
	glDepthFunc(GL_LEQUAL);
	{
		mSkyBoxShaderProgram->UseProgram();
		glm::mat4 skyView = glm::mat4(glm::mat3(glm::lookAt(mCameraPos, mCameraPos + mCameraOrientation, mCameraUP)));
		mSkyBoxShaderProgram->SetMatrixUniform("uView", skyView);
		glm::mat4 CameraTrans = glm::translate(glm::mat4(1.0f), mCameraPos);
		//mSkyBoxShaderProgram->SetMatrixUniform("uTrans", CameraTrans);
		glBindVertexArray(mSkyBoxVertexArray);
		mSkyBoxTexture->BindCubeMapTexture();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		mSkyBoxTexture->UnBindTexture();
		glBindVertexArray(0);
	}
	glDepthFunc(GL_LESS);



	// --- draw sprites ---
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	mSpriteShaderProgram->UseProgram();
	glBindVertexArray(mSpriteVertexArray);
	
	// spriteの描画位置を設定
	 //Draw Test Texture
	SetSpritePos(glm::vec3((float)mWindowWidth / 4.0f, 0, 0), mTestTexture, 0.25f, 0.5, 0.2f);
	mTestTexture->BindTexture();
	glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	mTestTexture->UnBindTexture();

	// Draw Text Texture
	{
		//Texture* tex = mFontMap["PHASE_IDLE"];
		////Texture* tex = mFontMap["PHASE_MOVE"];
		//SetSpritePos(glm::vec3(-(float)mWindowWidth / 4.0f, 0, 0), tex);
		//tex->BindTexture();
		//glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		//tex->UnBindTexture();
	}

	// Draw Font
	{
		//mTextShaderProgram->UseProgram();
		//glBindVertexArray(mTextVertexArray);

		//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0.0f, 1.0f));
		////glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

		////SetMatrixUniform("uWorldTransform", SpriteTrans, mTextShaderProgram);	// cubeの座標を反映
		//mTextShaderProgram->SetMatrixUniform("uWorldTransform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
		////SetMatrixUniform("uScaling", SpriteScaling, mTextShaderProgram);	// cubeの座標を反映
		////SetMatrixUniform("uRotate", SpriteRotate, mTextShaderProgram);	// cubeの座標を反映
		//mTextShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);

		////glUniform3f(glGetUniformLocation(mTextShaderProgram, "textColor"), color.x, color.y, color.z);
		//mTextShaderProgram->SetVectorUniform("textColor", glm::vec3(0.2f, 1.0f, 0.2f));
		//glActiveTexture(GL_TEXTURE0);

		// Sprite Translation Matrix
		mSpriteShaderProgram->UseProgram();
		glBindVertexArray(mSpriteVertexArray);
		glm::mat4 SpriteTrans = glm::mat4(1.0f);
		SpriteTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)mWindowWidth / 4.0f, (float)mWindowHeight / 4.0f, 0));

		// spriteのscaling matrix
		glm::vec3 sprite_scale_vec = glm::vec3((float)mFontWidth, (float)mFontHeight, 1.0f);
		sprite_scale_vec *= 1.0;
		glm::mat4 SpriteScaling = glm::scale(glm::mat4(1.0f), sprite_scale_vec);

		// spriteのrotation matrix
		glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1.0f));
		//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

		mSpriteShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);	// cubeの座標を反映
		mSpriteShaderProgram->SetMatrixUniform("uScaling", SpriteScaling);	// cubeの座標を反映
		mSpriteShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);	// cubeの座標を反映
		mSpriteShaderProgram->SetFloatUniform("uSpriteAlpha", 1.0f);


		glBindTexture(GL_TEXTURE_2D, FontTex);
		glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindTexture(FontTex, 0);
	}

	{
		// 日本語文字列表示
		//for (auto tex = mTextStr.begin(); tex != mTextStr.end(); tex++) {
		//	mSpriteShaderProgram->UseProgram();
		//	glm::mat4 SpriteTrans = glm::mat4(1.0f);
		//	SpriteTrans = glm::translate(glm::mat4(1.0f), glm::vec3((float)mWindowWidth / 4.0f, (float)mWindowHeight / 4.0f, 0));

		//	// spriteのscaling matrix
		//	glm::vec3 sprite_scale_vec = glm::vec3((float)mFontWidth, (float)mFontHeight, 1.0f);
		//	sprite_scale_vec *= 1.0;
		//	glm::mat4 SpriteScaling = glm::scale(glm::mat4(1.0f), sprite_scale_vec);

		//	// spriteのrotation matrix
		//	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1.0f));
		//	//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

		//	mSpriteShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);	// cubeの座標を反映
		//	mSpriteShaderProgram->SetMatrixUniform("uScaling", SpriteScaling);	// cubeの座標を反映
		//	mSpriteShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);	// cubeの座標を反映
		//	mSpriteShaderProgram->SetFloatUniform("uSpriteAlpha", 1.0f);

		//	glBindTexture(GL_TEXTURE_2D, *tex);
		//	glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		//	glBindTexture(*tex, 0);
		//}

	}

	{
		// 日本語文字列表示
		mTextShaderProgram->UseProgram();
		glBindVertexArray(mTextVertexArray);

		// 文字のtexcharの大きさを取得
		//int TexWidth = 0;
		//int TexHeight = 0;
		//for (auto c = text.begin(); c != text.end(); c++) {
		//	auto itr = mTexChars.find(*c);
		//	if (itr == mTexChars.end()) {
		//		std::cout << "error: Tex Char hasnt been loaded yet\n";
		//		exit(-1);
		//	}
		//	TexChar ch = mTexChars[*c];
		//	TexWidth += ch.Size.x;
		//	TexHeight = (TexHeight < ch.Size.y) ? ch.Size.y : TexHeight;
		//}

		glm::mat4 SpriteTrans = glm::mat4(1.0f);
		//glm::vec3 TexOffset = glm::vec3((float)TexWidth / 2.0f, (float)TexHeight / 2.0f, 0);
		SpriteTrans = glm::translate(glm::mat4(1.0f), glm::vec3((float)mWindowWidth / 4.0f, (float)mWindowHeight / 4.0f, 0.0f));


		// spriteのrotation matrix
		glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0.0f, 1.0f));
		//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

		//SetMatrixUniform("uWorldTransform", SpriteTrans, mTextShaderProgram);	// cubeの座標を反映
		mTextShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);
		//SetMatrixUniform("uScaling", SpriteScaling, mTextShaderProgram);	// cubeの座標を反映
		//SetMatrixUniform("uRotate", SpriteRotate, mTextShaderProgram);	// cubeの座標を反映
		mTextShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);

		//glUniform3f(glGetUniformLocation(mTextShaderProgram, "textColor"), color.x, color.y, color.z);
		glm::vec3 color = glm::vec3(0.2f, 1.0f, 0.2f);
		mTextShaderProgram->SetVectorUniform("textColor", color);
		glActiveTexture(GL_TEXTURE0);

		int x2 = 0;
		int y2 = 0;
		float scale = 1.0f;
		for (auto c = mJapanTexVec.begin(); c != mJapanTexVec.end(); c++) {
			TexChar ch = *c;

			float xpos = x2 + ch.Bearing.x * scale;
			float ypos = y2 - (ch.Size.y - ch.Bearing.y) * scale;
			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			float textVertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			glBindTexture(GL_TEXTURE_2D, ch.texID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, mTextVertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x2 += (ch.Advance >> 6) * scale;
		}

		glBindTexture(GL_TEXTURE_2D, 0);

	}

	// Draw AtrasTexture
	{
		// Sprite Translation Matrix
		mSpriteShaderProgram->UseProgram();
		glBindVertexArray(mSpriteVertexArray);
		glm::mat4 SpriteTrans = glm::mat4(1.0f);
		SpriteTrans = glm::translate(glm::mat4(1.0f), glm::vec3((float)mWindowWidth / 4.0f, (float)mWindowHeight / 4.0f, 0));

		// spriteのscaling matrix
		glm::vec3 sprite_scale_vec = glm::vec3((float)AtrasWidth, (float)AtrasHeight, 1.0f);
		sprite_scale_vec *= 1.0;
		glm::mat4 SpriteScaling = glm::scale(glm::mat4(1.0f), sprite_scale_vec);

		// spriteのrotation matrix
		glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1.0f));
		//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

		mSpriteShaderProgram->SetMatrixUniform("uWorldTransform", SpriteTrans);	// cubeの座標を反映
		mSpriteShaderProgram->SetMatrixUniform("uScaling", SpriteScaling);	// cubeの座標を反映
		mSpriteShaderProgram->SetMatrixUniform("uRotate", SpriteRotate);	// cubeの座標を反映
		mSpriteShaderProgram->SetFloatUniform("uSpriteAlpha", 1.0f);


		glBindTexture(GL_TEXTURE_2D, AtrasTex);
		glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindTexture(AtrasTex, 0);
	}


	// text 表示
	DrawText("hello, world!", glm::vec3(0.0f), glm::vec3(1.0f, 0.3f, 0.2f));
	DrawText2("hello", glm::vec3(0.0f), glm::vec3(0.2f, 1.0f, 0.2f));
	DrawUTF(u"初めてのOpenGL", glm::vec3((float)mWindowWidth / 4.0f, -(float)mWindowHeight / 4.0f, 0.0f), glm::vec3(0.2f, 1.0f, 0.2f));


	// 現在のフェーズ表示
	if (mPhase == PHASE_IDLE) {
		DrawText("PHASE_IDLE", glm::vec3(-(float)mWindowWidth * 3.0f / 8.0f, (float)mWindowHeight * 3.0f / 8.0f, 0.0f), glm::vec3(0.2f, 1.0f, 0.2f));
	}
	else if (mPhase == PHASE_MOVE) {
		DrawText("PHASE_MOVE", glm::vec3(-(float)mWindowWidth * 3.0f / 8.0f, (float)mWindowHeight * 3.0f / 8.0f, 0.0f), glm::vec3(0.2f, 1.0f, 0.2f));
	}


	{
		m3DTextShaderProgram->UseProgram();
		m3DTextShaderProgram->SetMatrixUniform("view", CameraView);
		glm::mat4 textRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(1.0, 0.0f, 0.0f));
		Draw3DUTF(u"3Dの文字列", glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.2f), 0.25f, textRotate);
	}


	glBindVertexArray(0);

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
	FT_Done_Face(mFontFace);
}