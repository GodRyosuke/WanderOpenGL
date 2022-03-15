#include "SDL.h"
#include "glew.h"
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Texture.hpp"
#include "json.hpp"

class ZGame {
public:
	ZGame();

	bool Initialize();
	void RunLoop();
	void Shutdown();

private:
	bool LoadShaders();
	void LoadData();

	void ProcessInput();
	void UpdateGame();
	void Draw();

	SDL_Window* mWindow;
	// OpenGL context
	SDL_GLContext mContext;

	bool mIsRunning;
	Uint32 mTicksCount;

	int mWindowWidth;
	int mWindowHeight;

	GLuint mSpriteShaderProgram;
	unsigned int mSpriteVertexArray;
	unsigned int mSpriteVertexBuffer;
	unsigned int mSpriteIndexBuffer;
	Texture* mTestTexture;
	Texture* mTextTexture;

	TTF_Font* mFont;


};