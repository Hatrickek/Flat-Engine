#pragma once
#include "utils/log.h"

namespace FlatEngine {
	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	unsigned int load_texture(const char* path, bool gammaCorrection);
}
