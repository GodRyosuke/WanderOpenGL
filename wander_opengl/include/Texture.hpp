#pragma once


#include <string>
#include "glew.h"
#include "SDL_ttf.h"
#include "SDL.h"
#include "stb_image.h"
#include <vector>

class Texture {
public:
	Texture();
	// Simple Load Texture
	Texture(std::string filePath);
	// For Cube Maps
	Texture(std::vector<std::string> filePaths);

	void CreateTextTexture(std::string text_data, int* color, TTF_Font* font);

	void BindTexture();
	void BindCubeMapTexture();
	void UnBindTexture();

	int getWidth() { return width; }
	int getHeight() { return height; }

private:
	void createFromSurf(SDL_Surface* surf);

	int width;
	int height;
	GLuint texture_data;
};