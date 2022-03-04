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
#include "ft2build.h"
#include FT_FREETYPE_H


class YGame {
public:
	YGame();
	bool Initialize();
	void RunLoop();
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void Draw();


	void ComputeWorldTransform();
	bool LoadShaders();
	bool LoadData();
	void SetSpritePos(glm::vec3 spritePos, Texture* tex, float scale = 1.0f, float rotation = 0.0f);

	enum PHASE{
		PHASE_IDLE,
		PHASE_MOVE,
		PHASE_MAX
	};

	SDL_Window* mWindow;
	// OpenGL context
	SDL_GLContext mContext;

	bool mIsRunning;
	bool mReComputeWorldTransform;
	Uint32 mTicksCount;

	int mWindowWidth;
	int mWindowHeight;

	//GLuint mVertexShader;
	//GLuint mFragShader;
	GLuint mMeshShaderProgram;
	GLuint mSpriteShaderProgram;

	glm::mat4 mView;
	glm::mat4 mProjection;

	// lighting
	glm::vec3 mAmbientLightColor;
	struct DirectionalLight {
		glm::vec3 direction;
		glm::vec3 diffuseColor;
		glm::vec3 specColor;
	};
	DirectionalLight mDirectionalLight;

	// cube object
	// transforms
	glm::mat4 mCubeWorldTrans;
	double mCubeScale;
	Eigen::Vector3d mCubePos;
	Eigen::Quaterniond mCubeRot;

	float mCubeRotation;
	double mCubeRotateVel;

	// Mesh Vertices
	unsigned int mCubeVertexArray;
	unsigned int mCubeVertexBuffer;
	unsigned int mCubeIndexBuffer;
	int mNumCubeIndicies;
	Texture* mCubeTexture;
	Texture* mTestTexture;
	int test_w;
	int test_h;


	// Sprite Vertices
	unsigned int mSpriteVertexArray;
	unsigned int mSpriteVertexBuffer;
	unsigned int mSpriteIndexBuffer;
	int mNumSpriteIndicies;
	glm::vec3 mSpritePos;

	TTF_Font* mFont;
	FT_Face mFontFace;
	GLuint FontTex;
	int mFontWidth;
	int mFontHeight;

	std::map<std::string, Texture*> mFontMap;

	PHASE mPhase;
	clock_t last;

	FILE* fp;
};

