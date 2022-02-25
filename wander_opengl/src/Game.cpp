#define _CRT_SECURE_NO_WARNINGS

#include "Game.hpp"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

Game::Game()
	:mWindowWidth(1024),
	mWindowHeight(768),
	mIsRunning(true),
	mReComputeWorldTransform(true)
{
}

bool Game::Initialize()
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

	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	CreateSpriteVerts();

	if (!LoadData())
	{
		SDL_Log("Failed to load data.");
		return false;
	}


	mTicksCount = SDL_GetTicks();

	return true;
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

static Eigen::Matrix4d CreateLookAt(const Eigen::Vector3d& eye, const  Eigen::Vector3d& target, const  Eigen::Vector3d& up)
{
	Eigen::Vector3d zaxis = target - eye;
	zaxis.normalize();
	Eigen::Vector3d xaxis = up.cross(zaxis);
	xaxis.normalize();
	Eigen::Vector3d yaxis = zaxis.cross(xaxis);
	yaxis.normalize();
	Eigen::Vector3d trans;
	trans.x() = -xaxis.dot(eye);
	trans.y() = -yaxis.dot(eye);
	trans.z() = -zaxis.dot(eye);

	//double temp[4][4] =
	//{
	//	{ xaxis.y(), yaxis.y(), zaxis.y(), 0.0f },
	//	{ xaxis.x(), yaxis.x(), zaxis.x(), 0.0f },
	//	{ xaxis.z(), yaxis.z(), zaxis.z(), 0.0f },
	//	{ trans.x(), trans.y(), trans.z(), 1.0f }
	//};
	Eigen::Matrix4d out;
	//out <<
	//	xaxis.y(), yaxis.y(), zaxis.y(), 0.0f,
	//	xaxis.x(), yaxis.x(), zaxis.x(), 0.0f,
	//	xaxis.z(), yaxis.z(), zaxis.z(), 0.0f,
	//	trans.x(), trans.y(), trans.z(), 1.0f;
	out <<
		xaxis.x(), yaxis.x(), zaxis.x(), 0.0f,
		xaxis.y(), yaxis.y(), zaxis.y(), 0.0f,
		xaxis.z(), yaxis.z(), zaxis.z(), 0.0f,
		trans.x(), trans.y(), trans.z(), 1.0f;

	return out;
}

static Eigen::Matrix4d CreatePerspectiveFOV(double fovY, double width, double height, double near, double far)
{
	double yScale = 1 / tan(fovY / 2.0f);
	double xScale = yScale * height / width;
	//double temp[4][4] =
	//{
	//	{ xScale, 0.0f, 0.0f, 0.0f },
	//	{ 0.0f, yScale, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, far / (far - near), 1.0f },
	//	{ 0.0f, 0.0f, -near * far / (far - near), 0.0f }
	//};
	Eigen::Matrix4d out;
	out <<
		xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, far / (far - near), 1.0f,
		0.0f, 0.0f, -near * far / (far - near), 0.0f;

	return out;
}



static void SetMatrixUniform(std::string uniformName, Eigen::Matrix4d mat, GLuint shaderProgram)
{
	GLuint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glUniformMatrix4fv(loc, 1, GL_TRUE, reinterpret_cast<const float*>(mat.data()));
}

static void SetVectorUniform(std::string uniformName, Eigen::Vector3d vec, GLuint shaderProgram)
{
	GLuint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glUniform3fv(loc, 1, reinterpret_cast<const float*>(vec.data()));
}

void SetDoubleUniform(std::string uniformName, double value, GLuint shaderProgram)
{
	GLuint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glUniform1f(loc, value);
}



bool Game::LoadShaders()
{
	std::string vertFilePath = "./Shaders/Phong.vert";
	std::string fragFilePath = "./Shaders/Phong.frag";

	// Compile vertex and pixel shaders
	if (!CompileShader(vertFilePath,
		GL_VERTEX_SHADER,
		mVertexShader) ||
		!CompileShader(fragFilePath,
			GL_FRAGMENT_SHADER,
			mFragShader))
	{
		return false;
	}

	// Now create a shader program that
	// links together the vertex/frag shaders
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// Verify that the program linked successfully
	GLint status;
	// Query the link status
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL Link Status:\n%s", buffer);
		return false;
	}

	// Activate Program
	glUseProgram(mShaderProgram);

	// View Projection行列を設定する
	// View行列の計算
	mView = CreateLookAt(Eigen::Vector3d::Zero(), Eigen::Vector3d::UnitX(), Eigen::Vector3d::UnitZ());
	mProjection = CreatePerspectiveFOV(70 * M_PI / 180,
		mWindowWidth, mWindowHeight, 25.0, 10000.0);


	SetMatrixUniform("uViewProj", mView * mProjection, mShaderProgram);
	//GLuint loc = glGetUniformLocation(mShaderProgram, "uViewProj");
	//// Send the matrix data to the uniform
	//Eigen::Matrix4d viewProj = mView * mProjection;
	//glUniformMatrix4fv(loc, 1, GL_TRUE, reinterpret_cast<const float*>(viewProj.data()));

	return true;
}

void Game::CreateSpriteVerts()
{

}

static bool LoadObjFile(std::string filePath, std::vector<Eigen::Vector3d>& vertices,
	std::vector<Eigen::Vector2d>& uvs, std::vector<Eigen::Vector3d>& normals,
	std::vector<Eigen::Vector3i>& vertexIndices, std::vector<Eigen::Vector3i>& uvIndices,
	std::vector<Eigen::Vector3i>& normalIndices)
{
	FILE* file = fopen(filePath.c_str(), "r");
	if (file == NULL) {
		printf("error: failed to open mesh file\n");
		return false;
	}

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
			Eigen::Vector3d vertex;
			vertex.x() = atof(x_str);
			vertex.y() = atof(y_str);
			vertex.z() = atof(z_str);
			vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			Eigen::Vector2d uv;
			char x_str[20];
			char y_str[20];
			fscanf(file, "%s %s", x_str, y_str);
			uv.x() = atof(x_str);
			uv.y() = atof(y_str);
			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			Eigen::Vector3d normal;
			char x_str[20];
			char y_str[20];
			char z_str[20];
			fscanf(file, "%s %s %sn", x_str, y_str, z_str);
			normal.x() = atof(x_str);
			normal.y() = atof(y_str);
			normal.z() = atof(z_str);
			normals.push_back(normal);
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
			vertexIndices.push_back(vertexIndex);
			uvIndices.push_back(uvIndex);
			normalIndices.push_back(normalIndex);
		}
	}

}

bool Game::LoadData()
{
	std::vector<Eigen::Vector3d> verticies;
	std::vector<Eigen::Vector2d> uvs;
	std::vector<Eigen::Vector3d> normals;
	std::vector<Eigen::Vector3i> vertexIndices;
	std::vector<Eigen::Vector3i> uvIndices;
	std::vector<Eigen::Vector3i> normalIndices;
	// objメッシュファイルを読み込む
	if (!LoadObjFile(".\\resources\\cube2.obj", verticies, uvs,
		normals, vertexIndices, uvIndices, normalIndices)) {
		return false;
	}
	// データの形を変更
	std::vector<double> vertices_data;
	for (int i = 0; i < vertexIndices.size(); i++) {
		Eigen::Vector3d this_verts[3];
		this_verts[0] = verticies[vertexIndices[i].x() - 1];
		this_verts[1] = verticies[vertexIndices[i].y() - 1];
		this_verts[2] = verticies[vertexIndices[i].z() - 1];
		for (int j = 0; j < 3; j++) {
			vertices_data.push_back(this_verts[j].x());
			vertices_data.push_back(this_verts[j].y());
			vertices_data.push_back(this_verts[j].z());
		}
	}

	// 立方体objメッシュのvertex array設定
	// Create vertex array
	glGenVertexArrays(1, &mCubeVertexArray);
	glBindVertexArray(mCubeVertexArray);



	// Create vertex buffer
	glGenBuffers(1, &mCubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mCubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_data.size() * sizeof(double), vertices_data.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Eigen::Vector3d), &verticies[0], GL_STATIC_DRAW);

	//// Create index buffer
	//glGenBuffers(1, &mCubeIndexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCubeIndexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(Eigen::Vector3i), &vertexIndices[0], GL_STATIC_DRAW);

	//mNumCubeIndicies = vertexIndices.size();

	//// Specify the vertex attributes
	//// (For now, assume one vertex format)
	//// Position is 3 floats
	//// vertex bufferの内部のデータ構造を指定
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//// Normal is 3 floats
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	//	reinterpret_cast<void*>(sizeof(float) * 3));
	//// Texture coordinates is 2 floats
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	//	reinterpret_cast<void*>(sizeof(float) * 6));

	// lighting の設定
	mAmbientLightColor << 0.2, 0.2, 0.2;
	mDirectionalLight.direction << 0, -0.707, -0.707;
	mDirectionalLight.diffuseColor << 0.78, 0.88, 1;
	mDirectionalLight.specColor << 0.8, 0.8, 0.8;



	return true;
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escapeキーを押下すると終了
	{
		mIsRunning = false;
	}
}

void Game::ComputeWorldTransform()
{
	if (mReComputeWorldTransform == true) {
		mReComputeWorldTransform = false;

		Eigen::Translation3d trans = Eigen::Translation3d(mCubePos);
		Eigen::Matrix3d scaleMat = Eigen::Matrix3d::Identity();
		scaleMat *= mCubeScale;
		Eigen::Affine3d mat;
		mat = trans * mCubeRot * scaleMat;
		mCubeWorldTrans =  mat.matrix();
		
		Eigen::Vector3d pos;
		pos << 200, 0, 0;
		trans = Eigen::Translation3d(pos);
		mat = trans;
		mCubeWorldTrans = mat.matrix();

		mat = mCubeWorldTrans.transpose();
		mCubeWorldTrans = mat.matrix();
	}
}

void Game::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	ComputeWorldTransform();	// Cubeのtransformを計算
}

void Game::Draw()
{
	// Set the clear color to light grey
	//glClearColor(0.6f, 0.2f, 0.7f, 1.0f);
	glClearColor(0, 0, 0, 1.0f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// Enable depth buffering/disable alpha blend
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	// draw meshes
	glUseProgram(mShaderProgram);	// activate

	// set view projection matrix
	SetMatrixUniform("uViewProj", mView * mProjection, mShaderProgram);

	// set lightings
	Eigen::Vector3d camera;
	{
		Eigen::Matrix4d mat = mView.inverse();
		Eigen::Affine3d aff;
		aff = mat;
		Eigen::Vector3d trans = aff.translation();
		camera << trans.x(), trans.y(), trans.z();
	}
	SetVectorUniform("uCameraPos", camera, mShaderProgram);
	// set lighting
	SetVectorUniform("uAmbientLight", mAmbientLightColor, mShaderProgram);
	SetVectorUniform("uDirLight.mDirection", mDirectionalLight.direction, mShaderProgram);
	SetVectorUniform("uDirLight.mDiffuseColor", mDirectionalLight.diffuseColor, mShaderProgram);
	SetVectorUniform("uDirLight.mSpecColor", mDirectionalLight.specColor, mShaderProgram);

	// draw cube
	// cubeの配置を設定
	SetMatrixUniform("uWorldTransform", mCubeWorldTrans, mShaderProgram);
	SetDoubleUniform("uSpecPower", 100.0, mShaderProgram);	// cubeの反射度合い設定
	// cubeのvertex array有効化
	glBindVertexArray(mCubeVertexArray);
	// 描画
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	//glDrawElements(GL_TRIANGLES, mNumCubeIndicies * 3, GL_UNSIGNED_INT, nullptr);



	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	
	// draw sprites

	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		Draw();
	}
}

void Game::Shutdown()
{

}