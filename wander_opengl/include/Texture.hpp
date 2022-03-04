#include <string>
#include "glew.h"
#include "SDL_ttf.h"
#include "SDL.h"

class Texture {
public:
	Texture();
	Texture(std::string filePath);

	void CreateTextTexture(std::string text_data, int* color, TTF_Font* font);

	void BindTexture();
	void UnBindTexture();

	int getWidth() { return width; }
	int getHeight() { return height; }

private:
	void createFromSurf(SDL_Surface* surf);

	int width;
	int height;
	GLuint texture_data;
};