#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "stb_image/load.h"

TextureData loadTexture(std::string_view path)
{
	int imWidth, imHeight, imNumChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imData = stbi_load(path.data(), &imWidth, &imHeight, &imNumChannels, 0);

	return {imWidth, imHeight, imNumChannels, imData};
}