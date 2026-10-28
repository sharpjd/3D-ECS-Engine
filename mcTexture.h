#pragma once

#include "libs/glad/glad.h" // include glad to get all the required OpenGL headers

#include "loader.h"
#include "engineContext.h"
#include "debug.h"

class MCTexture {
private:
	GLuint gl_textureId; //for *generating the texture*
	int gl_textureUnitId = -1; //for the *location of texture buffer units*
public:

	TextureId texture;
	TextureLoader& loader;

	MCTexture(TextureId texture, TextureLoader& loader, int gl_textureUnitId) 
		: texture(texture), loader(loader), gl_textureUnitId(gl_textureUnitId) 
	{
		compile();
	}

	int gl_texUnitIndex() const { //infer the texture index for use in a shader
		return gl_textureUnitId - GL_TEXTURE0;
	}

	void compile() {
		auto& tLoader = loader.getLoadable(texture);

		//probably passed in something like 0 or 1, pass GL_TEXTURE0 instead
		assert(gl_textureUnitId >= GL_TEXTURE0);

		glActiveTexture(gl_textureUnitId);

		glGenTextures(1, &gl_textureId);
		glBindTexture(GL_TEXTURE_2D, gl_textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		tLoader.load();
		auto data = tLoader.getTexture();
		GLenum format = GL_RGB;
		switch (tLoader.nrChannels()) {
		case 1: format = GL_RED;  break;
		case 3: format = GL_RGB;  break;
		case 4: format = GL_RGBA; break;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			format, 
			tLoader.width(), 
			tLoader.height(), 
			0, 
			format, 
			GL_UNSIGNED_BYTE, 
			data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
		tLoader.unload();
	}
};