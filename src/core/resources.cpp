#include "resources.h"
#include "utils/filesystem.h"
namespace FlatEngine {
	static Ref<Model>	defaultCube;
	static Ref<Shader>	defaultShader; 
	void Resources::InitResources() {
		defaultCube		= CreateRef<Model>(FileSystem::getPath("resources/objects/cube.obj").c_str());
		defaultShader	= CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao_geometry.vs").c_str(),FileSystem::getPath("resources/shaders/ssao_geometry.fs").c_str());
	}
	Ref<Model> Resources::GetDefaultCube() {
		return defaultCube;
	}
	Ref<Shader> Resources::GetDefaultShader() {

		return defaultShader;
	}

}
