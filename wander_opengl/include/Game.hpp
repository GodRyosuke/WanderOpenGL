#include "SDL.h"
#include "glew.h"
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Game {
public:
	Game();
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


	SDL_Window* mWindow;
	// OpenGL context
	SDL_GLContext mContext;

	bool mIsRunning;
	bool mReComputeWorldTransform;
	Uint32 mTicksCount;

	int mWindowWidth;
	int mWindowHeight;

	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;

	Eigen::Matrix4d mView;
	Eigen::Matrix4d mProjection;

	// lighting
	Eigen::Vector3d mAmbientLightColor;
	struct DirectionalLight {
		Eigen::Vector3d direction;
		Eigen::Vector3d diffuseColor;
		Eigen::Vector3d specColor;
	};
	DirectionalLight mDirectionalLight;
	
	// cube object
	// transforms
	Eigen::Matrix4d mCubeWorldTrans;
	double mCubeScale;
	Eigen::Vector3d mCubePos;
	Eigen::Quaterniond mCubeRot;

	// vertices
	unsigned int mCubeVertexArray;
	unsigned int mCubeVertexBuffer;
	unsigned int mCubeIndexBuffer;
	int mNumCubeIndicies;


	
};