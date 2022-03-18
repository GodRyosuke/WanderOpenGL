#pragma once

#include "glew.h"
#include <iostream>

class GLUtil {
public:
	GLUtil() {}
	void GetErr() {
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err) {
			case GL_INVALID_ENUM:
				std::cout << "GL_INVALID_ENUM\n";
				break;
			case GL_INVALID_VALUE:
				std::cout << "GL_INVALID_VALUE\n";
				break;
			case GL_INVALID_OPERATION:
				std::cout << "GL_INVALID_OPERATION\n";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
				break;
			case GL_OUT_OF_MEMORY:
				std::cout << "GL_OUT_OF_MEMORY\n";
				break;
			case GL_STACK_UNDERFLOW:
				std::cout << "GL_STACK_UNDERFLOW\n";
				break;
			case GL_STACK_OVERFLOW:
				std::cout << "GL_STACK_OVERFLOW\n";
				break;
			}
		}
	}
};
