#pragma once
#include "utils/log.h"

namespace FlatEngine {
	class Texture {
	public:
		uint32_t m_ID;
		std::string m_Type;
		std::string m_Path;
		uint32_t m_Width, m_Height;
		Texture() = default;
		Texture(const char* path);
		~Texture();
	private:
		static unsigned int TextureFromFile(const char* path, bool gammaCorrection = false);
	};

}
