#include "TextureLoader.h"

#include <IL/il.h>

GLuint loadTexture(const std::string& name, int mode)
{
	ilInit();

	GLuint texId;
	glGenTextures(1, &texId);

	ILuint ilId;
	ilGenImages(1, &ilId);

	// DevIL-Bild Name binden
	ilBindImage(ilId);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	if (!ilLoadImage(name.c_str()))
	{
		return 0;
	}
	// Bild in RGBA umwandeln
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	glActiveTexture(GL_TEXTURE0);
	// OpenGL Texture erstellen und laden
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	ilDeleteImage(ilId);

	return texId;
}

