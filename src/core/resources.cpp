#include "resources.h"
#include "render/renderer.h"
#include "utils/filesystem.h"
namespace FlatEngine {
	static Ref<Model>	defaultCube;
	static Ref<Shader>	defaultShader;
	static Ref<Shader>  lightingShader;
	static Ref<Shader>	ssaoGeometryShader;
	static Ref<Shader>	ssaoShader;
	static Ref<Shader>	ssaoBlurShader;
	void Resources::InitResources() {
		defaultCube = CreateRef<Model>(FileSystem::getPath("resources/objects/cube.obj").c_str());
		LoadShaders();
	}
	void Resources::LoadShaders(){
		defaultShader = CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao_geometry.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_geometry.fs").c_str());
		lightingShader = CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_lighting.fs").c_str());
		lightingShader->use();
		lightingShader->setInt("gPosition", 0);
		lightingShader->setInt("gNormal", 1);
		lightingShader->setInt("gAlbedo", 2);
		lightingShader->setInt("ssao", 3);

		ssaoGeometryShader = CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao_geometry.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_geometry.fs").c_str());
		ssaoShader = CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao.fs").c_str());
		ssaoBlurShader = CreateRef<Shader>(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_blur.fs").c_str());

		Renderer::GetSSAOBuffer()->SetupSSAOShader(Resources::GetSSAOShader(), Resources::GetSSAOBlurShader());
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
	Ref<Shader> Resources::GetSSAOGeometryShader() {
		return ssaoGeometryShader;
	}
	Ref<Shader> Resources::GetSSAOShader() {
		return ssaoShader;
	}
	Ref<Shader> Resources::GetSSAOBlurShader() {
		return ssaoBlurShader;
	}
	void Resources::SetDefaultShader(Ref<Shader> shader){
		defaultShader = shader;
	}
	void Resources::SetLightingShader(Ref<Shader> shader){
		lightingShader = shader;
	}
	void Resources::SetSSAOShader(Ref<Shader> shader){
		ssaoShader = shader;
	}
	void Resources::SetSSAOGeometryShader(Ref<Shader> shader){
		ssaoGeometryShader = shader;
	}
	void Resources::SetSSAOBlurShader(Ref<Shader> shader){
		ssaoBlurShader = shader;
	}
}