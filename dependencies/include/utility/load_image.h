#ifndef LOAD_H
#define LOAD_H

#include <string_view>

struct TextureData
{
	int width, height;
	int numChannels;
	unsigned char* data;
};

TextureData loadTexture(std::string_view path);

#endif