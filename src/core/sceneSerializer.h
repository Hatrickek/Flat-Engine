#pragma once
#include "scene.h"
namespace FlatEngine {
	class SceneSerializer {
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filePath);
		//void SerializeRuntime(const std::string& filePath);

		bool Deserialize(const std::string& filePath);
		//bool DeserializeRuntime(const std::string& filePath);
	private:
		Ref<Scene> m_Scene;
	};
}
