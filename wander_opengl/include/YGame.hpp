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
#include "gtx/rotate_vector.hpp"
#include "gtx/vector_angle.hpp"
#include "Texture.hpp"
#include "json.hpp"
#include "ft2build.h"
#include "Shader.hpp"
#include "Mesh.hpp"
#include FT_FREETYPE_H


namespace nl = nlohmann;

class YGame {
public:
	YGame();
	bool Initialize();
	void RunLoop();
	void Shutdown();

private:
	struct TexChar {
		GLuint texID;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		unsigned int Advance;
	};

	void ProcessInput();
	void UpdateGame();
	void Draw();


	void ComputeWorldTransform();
	bool LoadShaders();
	bool LoadData();

	TexChar LoadChar(char c);
	TexChar LoadUTFChar(char16_t c);
	void SetSpritePos(glm::vec3 spritePos, Texture* tex, float scale = 1.0f, float rotation = 0.0f, float alpha = 1.0f);
	void RenderText(std::string text, glm::vec3 pos, float scale = 1.0f);
	void RenderText2(std::string text, glm::vec3 pos, glm::vec3 color, float scale = 1.0f);
	void DrawText(std::string text, glm::vec3 pos, glm::vec3 color, float scale = 1.0f, float rot = 0.0f);
	void DrawText2(std::string text, glm::vec3 pos, glm::vec3 color, float scale = 1.0f, float rot = 0.0f);
	void DrawUTF(std::u16string text, glm::vec3 pos, glm::vec3 color, float scale = 1.0f, float rot = 0.0f);
	void Draw3DUTF(std::u16string text, glm::vec3 pos, glm::vec3 color, float scale = 1.0f, glm::mat4 rot = glm::mat4(1.0f));

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

	glm::ivec3 mMousePos;

	//GLuint mVertexShader;
	//GLuint mFragShader;
	Shader* mMeshShaderProgram;
	Shader* mSpriteShaderProgram;
	Shader* mTextShaderProgram;
	Shader* m3DTextShaderProgram;
	Shader* mSkyBoxShaderProgram;
	Shader* mSkinningShaderProgram;


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
	std::vector<Mesh*> mMeshes;
	Mesh* mSphareMesh;
	Mesh* mCubeMesh;
	Mesh* mDonutMesh;
	Mesh* mHouseWall;
	Mesh* mDonut2Mesh;


	Texture* mCubeTexture;
	Texture* mDonutTexture;
	Texture* mTestTexture;
	Texture* mHouseWallTexture;
	Texture* mSkyBoxTexture;

	int test_w;
	int test_h;

	// Camera
	glm::vec3 mCameraPos;
	glm::vec3 mCameraUP;
	glm::vec3 mCameraOrientation;
	float mMoveSpeed;
	float mMoveSensitivity;


	// Sprite Vertices
	unsigned int mSpriteVertexArray;
	unsigned int mSpriteVertexBuffer;
	unsigned int mSpriteIndexBuffer;
	glm::vec3 mSpritePos;

	// Text Vertices
	unsigned int mTextVertexArray;
	unsigned int mTextVertexBuffer;

	// 3D Text Vertex Array
	unsigned int m3DTextVertexArray;
	unsigned int m3DTextVertexBuffer;

	// Atras Vertices
	unsigned int mAtrasVertexArray;
	unsigned int mAtrasVertexBuffer;

	// Sky Box Vertices
	unsigned int mSkyBoxVertexArray;
	unsigned int mSkyBoxVertexBuffer;
	unsigned int mSkyBoxIndexBuffer;

	TTF_Font* mFont;
	FT_Face mFontFace;
	GLuint FontTex;
	int mFontWidth;
	int mFontHeight;

	GLuint AtrasTex;
	int AtrasHeight;
	int AtrasWidth;
	struct Character {
		float ax; // advance.x
		float ay; // advance.y

		float bw; // bitmap.width;
		float bh; // bitmap.rows;

		float bl; // bitmap_left;
		float bt; // bitmap_top;

		float tx; // x offset of glyph in texture coordinates
	};
	std::map<char, Character> Characters;


	std::map<char, TexChar> mTexChars;

	std::map<std::string, Texture*> mFontMap;
	std::vector<GLuint> mTextStr;
	std::map<char16_t, TexChar> mJapanTexChars;
	std::vector<TexChar> mJapanTexVec;


	PHASE mPhase;
	clock_t last;

	FILE* fp;

	nl::json mTextData;
};

