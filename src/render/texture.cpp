#include "texture.h"
#include "stb_image.h"
#include "glad/glad.h"

namespace FlatEngine {
	//TODO: @CLEANUP
	//TODO: @NOT_WORKING
	Texture::Texture(const char* path) {
		//m_Type = type;
		m_Path = path;

		glGenTextures(1, &m_ID);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		data = stbi_load(path, &width, &height, &channels, 0);
		if(data) {
			m_Width = width;
			m_Height = height;
			GLenum dataFormat = 0;
			GLenum internalFormat = 0;
			if(channels == 1)
				dataFormat = internalFormat = GL_RED;
			else if(channels == 3) {
				dataFormat = GL_RGB;
				internalFormat = GL_RGB;
			}
			else if(channels == 4) {
				dataFormat = GL_RGBA;
				internalFormat = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, m_ID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			FE_LOG_INFO("Texture loaded id: {} - name: {}", m_ID, m_Path);

			stbi_image_free(data);
		}
		else {
			FE_LOG_ERROR("Texture failed to load at path: {}", path);
			stbi_image_free(data);
		}
	}
	Texture::~Texture() {
		glDeleteTextures(1, &m_ID);
	}
	unsigned int Texture::TextureFromFile(const char* path, bool gammaCorrection) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		data = stbi_load(path, &width, &height, &channels, 0);
		if(data) {
			//m_Width = width;
			//m_Height = height;
			GLenum dataFormat = 0;
			GLenum internalFormat = 0;
			if(channels == 1)
				dataFormat = internalFormat = GL_RED;
			else if(channels == 3) {
				dataFormat = GL_RGB;
				internalFormat = GL_RGB;
			}
			else if(channels == 4) {
				dataFormat = GL_RGBA;
				internalFormat = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			FE_LOG_INFO("Texture loaded id: {} - name: {}", textureID, path);

			stbi_image_free(data);
		}
		else {
			FE_LOG_ERROR("Texture failed to load at path: {}", path);
			stbi_image_free(data);
		}

		return textureID;
	}
}