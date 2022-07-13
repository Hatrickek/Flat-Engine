#pragma once

#include "rootpath.h"

#include <string>
#include <cstdlib>
#include <filesystem>
#include "log.h"

//TODO: CLEANUP!

class FileSystem {
public:
	static std::string getPath(const std::string& path) {
		static std::string(*pathBuilder)(std::string const&) = getPathBuilder();
		return (*pathBuilder)(path);
	}
	static void SetRootPath(const std::filesystem::path& path){
		if(path.empty()){
			ROOTPATH = std::filesystem::current_path().string();
		}
		else{
			ROOTPATH = path.string();
		}
		
	}
private:
	typedef std::string(*Builder) (const std::string& path);
private:
	static std::string const& getRoot() {
		static char const* envRoot = getenv("LOGL_ROOT_PATH");
		static char const* givenRoot = (envRoot != nullptr ? envRoot : ROOTPATH.string().c_str());
		static std::string root = (givenRoot != nullptr ? givenRoot : "");
		return root;
	}

	static Builder getPathBuilder() {
		if(getRoot() != "")
			return &FileSystem::getPathRelativeRoot;
		else
			return &FileSystem::getPathRelativeBinary;
	}

	static std::string getPathRelativeRoot(const std::string& path) {
		return getRoot() + std::string("/") + path;
	}

	static std::string getPathRelativeBinary(const std::string& path) {
		return path;
	}

};
