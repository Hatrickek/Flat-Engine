#include "resources.h"
#include "utils/filesystem.h"
namespace FlatEngine {
	static Ref<Model>	defaultCube;
	static Ref<Shader>	defaultShader;
	static Ref<Shader>  lightingShader;
	void Resources::InitResources() {
		defaultCube		= CreateRef<Model>(FileSystem::getPath("resources/objects/cube.obj").c_str());
		defaultShader	= CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao_geometry.vs").c_str(),FileSystem::getPath("resources/shaders/ssao_geometry.fs").c_str());
		lightingShader	= CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_lighting.fs").c_str());
		lightingShader->use();
		lightingShader->setInt("gPosition", 0);
		lightingShader->setInt("gNormal", 1);
		lightingShader->setInt("gAlbedo", 2);
		lightingShader->setInt("ssao", 3);
	}
	Ref<Model> Resources::GetDefaultCube() {
		return defaultCube;
	}
	Ref<Shader> Resources::GetDefaultShader() {

		return defaultShader;
	}
	Ref<Shader> Resources::GetLightingShader() {
		return lightingShader;
	}
}
