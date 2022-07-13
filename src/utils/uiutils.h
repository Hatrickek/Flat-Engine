#pragma once
#include <string>

namespace FlatEngine {
	class FileDialogs {
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}
