#include "Texture.hpp"
#include "SDL_image.h"

Texture::Texture()
{

}

Texture::Texture(std::string filePath)
{
	// Load from file
	SDL_Surface* surf = IMG_Load(filePath.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", filePath.c_str());
		return;
	}

	createFromSurf(surf);
	SDL_FreeSurface(surf);
}

void Texture::CreateTextTexture(std::string text_data, int* color, TTF_Font* font)
{
	SDL_Color color_data;
	color_data.r = color[0];
	color_data.g = color[1];
	color_data.b = color[2];
	color_data.a = (Uint8)color[3];
	SDL_Surface* font_surface = TTF_RenderUTF8_Blended(font, text_data.c_str(), color_data);
	if (!font_surface)
	{
		SDL_Log("Failed to load texture file %s", text_data.c_str());
		return;
	}

	createFromSurf(font_surface);
	SDL_FreeSurface(font_surface);
}

void Texture::createFromSurf(SDL_Surface* surf)
{
	glGenTextures(1, &texture_data);
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, texture_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint texture_format;
	GLuint colors = surf->format->BytesPerPixel;
	if (colors == 4) {   // alpha
		if (surf->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}
	else {             // no alpha
		if (surf->format->Rmask == 0x000000ff)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	}

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//// Configures the way the texture repeats (if it does at all)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	width = surf->w;
	height = surf->h;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);


	glBindTexture(texture_data, 0);		// unbind
}

void Texture::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture_data);
}

void Texture::UnBindTexture()
{
	glBindTexture(texture_data, 0);
}