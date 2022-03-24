#include "Texture.hpp"
#include "SDL_image.h"
#include "GLUtil.hpp"

Texture::Texture()
{

}

Texture::Texture(std::string filePath)
{
	// Load from file
	int numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* PictureData = stbi_load(filePath.c_str(), &width, &height, &numColCh, 0);

	glGenTextures(1, &texture_data);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto colorCh = GL_RGBA;
	if (numColCh == 3) {
		colorCh = GL_RGB;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, colorCh, width, height, 0, colorCh, GL_UNSIGNED_BYTE, PictureData);
	// Generates MipMaps
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(texture_data, 0);		// unbind


	//SDL_Surface* surf = IMG_Load(filePath.c_str());
	//if (!surf)
	//{
	//	SDL_Log("Failed to load texture file %s", filePath.c_str());
	//	return;
	//}

	//createFromSurf(surf);
	//SDL_FreeSurface(surf);
}

Texture::Texture(std::vector<std::string> filePaths)
{
	glGenTextures(1, &texture_data);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filePaths[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << filePaths[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glBindTexture(texture_data, 0);
}


void Texture::CreateTextTexture(std::string text_data, int* color, TTF_Font* font)
{
	SDL_Color color_data;
	color_data.r = static_cast<Uint8>(color[0]);
	color_data.g = static_cast<Uint8>(color[1]);
	color_data.b = static_cast<Uint8>(color[2]);
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
	glActiveTexture(GL_TEXTURE0);
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
	//glGenerateMipmap(GL_TEXTURE_2D);


	glBindTexture(texture_data, 0);		// unbind
}

void Texture::BindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_data);
}

void Texture::BindCubeMapTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_data);
}

void Texture::UnBindTexture()
{
	glBindTexture(texture_data, 0);
}