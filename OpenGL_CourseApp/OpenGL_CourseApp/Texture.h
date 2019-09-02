#pragma once
#include <iostream>
#include <GL\glew.h>

#include "CommonValues.h"

using namespace std;

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	bool LoadTexture();
	bool LoadTextureA(); //Load texture with Alpha.
	
	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};

