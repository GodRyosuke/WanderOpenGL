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
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

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
	void CreateSpriteVerts();

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
	GLuint mCubeTexture;

	// Sprite Vertices
	unsigned int mSpriteVertexArray;
	unsigned int mSpriteVertexBuffer;
	unsigned int mSpriteIndexBuffer;
	int mNumSpriteIndicies;
	GLuint mSpriteTexture;
	glm::vec3 mSpritePos;

	PHASE mPhase;
	clock_t last;

	FILE* fp;
};