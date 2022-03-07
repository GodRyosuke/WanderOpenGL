#include "ZGame.hpp"


static float SpriteVertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
	0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
	0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
	-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
};



static unsigned int SpriteIndices[] =
{
	0, 1, 2,
	2, 3, 0
};

ZGame::ZGame()
	:mWindowWidth(1024),
	mWindowHeight(768),
	mIsRunning(true)
{

}

bool ZGame::Initialize()
{

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

	glGetError();

	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}


	LoadData();

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

static void SetMatrixUniform(std::string uniformName, glm::mat4 mat, GLuint shaderProgram)
{
	GLuint loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ZGame::LoadData()
{
	// Load Font
	mFont = nullptr;
	mFont = TTF_OpenFont("./resources/Carlito-Regular.ttf", 128);

	// Load Texture
	//mTestTexture = new Texture(".\\resources\\test_picture.png");

	int font_color[] = {
		0x00, 0xdd, 0xdd, 255
	};
	mTextTexture = new Texture();
	mTextTexture->CreateTextTexture("abc", font_color, mFont);
}

bool ZGame::LoadShaders()
{
	// sprite shaderのcompile
	// Sprite
	std::string spriteVertFile = "./Shaders/Sprite.vert";
	std::string spriteFragFile = "./Shaders/Sprite.frag";

	if (!CreateShaderProgram(spriteVertFile, spriteFragFile, mSpriteShaderProgram)) {
		return false;
	}

	// 2Dのview proj matを設定
	glm::mat4 spriteViewProj = glm::mat4(1.0f);
	spriteViewProj[0][0] = 2.0f / (float)mWindowWidth;
	spriteViewProj[1][1] = 2.0f / (float)mWindowHeight;
	spriteViewProj[3][2] = 1.0f;
	//spriteViewProj = glm::ortho(0.0f, (float)mWindowWidth, (float)mWindowHeight, 0.0f, -1.0f, 1.0f);
	SetMatrixUniform("uViewProj", spriteViewProj, mSpriteShaderProgram);


	// Vertex Arrayの設定
	glGenVertexArrays(1, &mSpriteVertexArray);
	glBindVertexArray(mSpriteVertexArray);

	// Create vertex buffer
	glGenBuffers(1, &mSpriteVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mSpriteVertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertices), SpriteVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(float), SpriteVertices, GL_STATIC_DRAW);


	// Create index buffer
	glGenBuffers(1, &mSpriteIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSpriteIndexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SpriteIndices), SpriteIndices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), SpriteIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	// Normal is 3 floats
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		reinterpret_cast<void*>(sizeof(float) * 3));
	// Texture coordinates is 2 floats
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		reinterpret_cast<void*>(sizeof(float) * 6));


	return true;
}


void ZGame::ProcessInput()
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
			//if (mPhase == PHASE_IDLE) {
			//	//mLastMousePos = mMousePos;
			//	//mSwipingDropPos = mMousePos / GRID_SIZE;
			//	mPhase = PHASE_MOVE;
			//}
		}
		break;
		case SDL_MOUSEBUTTONUP:		// マウスを離したら
			//if (mPhase == PHASE_MOVE) {
			//	mPhase = PHASE_IDLE;

			//	/*if (EraseDrops()) {
			//		phase = PHASE_ERASE;
			//	}
			//	else {
			//		phase = PHASE_IDLE;
			//	}*/
			//}
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE] || keyState[SDL_SCANCODE_Q])	// escapeキーを押下すると終了
	{
		mIsRunning = false;
	}
}


void ZGame::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	//mCubeRotation = deltaTime * mCubeRotateVel * 10;
	///*if (deltaTime > 0.015) {
	//	mCubeRotation += 0.0001f;
	//}*/
	////std::string out_str = std::to_string(clock()) + "\t" + std::to_string(mCubeRotation) + "\n";
	////fprintf(fp, out_str.c_str());

	////std::cout << clock() - last << std::endl;
	//last = clock();
	//ComputeWorldTransform();	// Cubeのtransformを計算
}

static void SetSpritePos(glm::vec3 spritePos, Texture* tex, float scale, float rotation, GLuint shaderProgram)
{
	// Sprite Translation Matrix
	glm::mat4 SpriteTrans = glm::mat4(1.0f);
	SpriteTrans = glm::translate(glm::mat4(1.0f), spritePos);

	// spriteのscaling matrix
	glm::vec3 sprite_scale_vec = glm::vec3((float)tex->getWidth(), (float)tex->getHeight(), 1.0f);
	sprite_scale_vec *= 0.25;
	glm::mat4 SpriteScaling = glm::scale(glm::mat4(1.0f), sprite_scale_vec);

	// spriteのrotation matrix
	glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1.0f));
	//glm::mat4 SpriteRotate = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(0, 0, 1.0f));

	SetMatrixUniform("uWorldTransform", SpriteTrans, shaderProgram);	// cubeの座標を反映
	SetMatrixUniform("uScaling", SpriteScaling, shaderProgram);	// cubeの座標を反映
	SetMatrixUniform("uRotate", SpriteRotate, shaderProgram);	// cubeの座標を反映
}

void ZGame::Draw()
{
	// Set the clear color to light grey
	//glClearColor(0.6f, 0.2f, 0.7f, 1.0f);
	glClearColor(0, 0.5, 0.7, 1.0f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	glUseProgram(mSpriteShaderProgram);
	glBindVertexArray(mSpriteVertexArray);

	// spriteの描画位置を設定
	 //Draw Test Texture
	//SetSpritePos(glm::vec3((float)mWindowWidth / 4.0f, 0, 0), mTestTexture, 0.5f, 0, mSpriteShaderProgram);
	//mTestTexture->BindTexture();
	//glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	//mTestTexture->UnBindTexture();

	// Draw a Text Texture
	SetSpritePos(glm::vec3(-(float)mWindowWidth / 4.0f, 0, 0), mTextTexture, 0.5f, 0, mSpriteShaderProgram);
	mTextTexture->BindTexture();
	//glDrawElements(GL_TRIANGLES, sizeof(SpriteIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	mTextTexture->UnBindTexture();


	glBindVertexArray(0);

	SDL_GL_SwapWindow(mWindow);
}


void ZGame::RunLoop()
{
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

void ZGame::Shutdown()
{
}