#pragma once
#include <ostream>
#include "glad/glad.h"

#include "utils/log.h"

namespace FlatEngine {
	class FBuffer {
	public:
		unsigned int fBufferID;
		unsigned int colorBuffer;
		unsigned int m_width;
		unsigned int m_height;
		FBuffer(unsigned int width, unsigned int height) {
			m_width = width;
			m_height = height;

			glGenFramebuffers(1, &fBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, fBufferID);

			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

			unsigned int attachments = GL_COLOR_ATTACHMENT0;
			glDrawBuffers(3, &attachments);

			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				FE_LOG_ERROR("Framebuffer not complete!");
			//else
			//	FE_LOG_INFO("Framebuffer created! ID: {0}", fBufferID);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void RegenerateBuffers(unsigned int width, unsigned int height) {
			Unload();
			FBuffer(width, height);
		}
		void Begin() {
			glBindFramebuffer(GL_FRAMEBUFFER, fBufferID);
			//glClear(GL_COLOR_BUFFER_BIT);
		}
		void End() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void Unload() {
			glDeleteFramebuffers(1, &fBufferID);
			glDeleteTextures(1, &colorBuffer);
		}
	private:
	};
}