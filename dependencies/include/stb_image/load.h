#ifndef LOAD_H
#define LOAD_H

struct TextureData
{
	int width, height;
	int numChannels;
	unsigned char* data;
};

TextureData loadTexture(const char* path);

#endif