#include "sceneSerializer.h"
#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include "entity.h"
#include "model.h"
#include "resources.h"

namespace YAML {
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace FlatEngine {
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene){}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "3131"; //TODO: Entity ID

		// TagComponent
		if(entity.HasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			auto &tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap; 
		}
		if(entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto &tc = entity.GetComponent<TransformComponent>();

			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}
		if(entity.HasComponent<MeshRendererComponent>()) {
			out << YAML::Key << "MeshRendererComponent";
			out << YAML::BeginMap;
			auto& mrc = entity.GetComponent<MeshRendererComponent>();

			out << YAML::Key << "Model" << YAML::Value << mrc.model->m_Path;
			out << YAML::Key << "Color" << YAML::Value << mrc.color;
			out << YAML::Key << "Shader Fragment" << YAML::Value << mrc.shader->m_fragmentPath;
			out << YAML::Key << "Shader Vertex" << YAML::Value << mrc.shader->m_vertexPath;
			out << YAML::EndMap;
		}
		if(entity.HasComponent<PrimitiveRendererComponent>()) {
			out << YAML::Key << "PrimitiveRendererComponent";
			out << YAML::BeginMap;
			auto& prc = entity.GetComponent<PrimitiveRendererComponent>();

			out << YAML::Key << "Type" << YAML::Value << prc.primitive;
			out << YAML::Key << "Color" << YAML::Value << prc.color;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filePath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID) {
			Entity entity = { entityID, m_Scene.get()};
			if(!entity)
				return;
			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filePath) {
		YAML::Node data;
		try {
			data = YAML::LoadFile(filePath);
		}
		catch (YAML::ParserException e) {
			return false;
		}
		if (!data["Scene"])
			return false;
		std::string sceneName = data["Scene"].as<std::string>();
		FE_LOG_INFO("Loading scene: '{}'", sceneName);

		if (auto entities = data["Entities"]) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>(); //TODO:
				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				Entity desEntity = m_Scene->CreateEntity(name);

				if (auto transformComponent = entity["TransformComponent"] ) {
					auto& tc = desEntity.GetComponent<TransformComponent>();
					tc.Translation	= transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation		= transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale		= transformComponent["Scale"].as<glm::vec3>();
				}

				if(auto meshRendererComponent = entity["MeshRendererComponent"]) {
					auto& mrc = desEntity.AddComponent<MeshRendererComponent>();
					const std::string& modelPath = meshRendererComponent["Model"].as<std::string>();
					Ref<Model> model = CreateRef<Model>(modelPath); 
					if(!model->meshes.empty()) {
						mrc.model = model;
					}
					else {
						mrc.model = Resources::GetDefaultCube();
					}
					mrc.color = meshRendererComponent["Color"].as<glm::vec4>();
					const std::string vertexPath	= meshRendererComponent["Shader Vertex"].as<std::string>();
					const std::string fragmenthPath	= meshRendererComponent["Shader Fragment"].as<std::string>();
					Ref<Shader> shader = CreateRef<Shader>(vertexPath.c_str(), fragmenthPath.c_str());
					mrc.shader = shader;
				}
				if(auto primitiveRendererComp = entity["PrimitiveRendererComponent"]) {
					auto& prc = desEntity.AddComponent<PrimitiveRendererComponent>();
					Primitive primitive = static_cast<Primitive>(primitiveRendererComp["Type"].as<int>());
					prc.primitive = primitive;
					prc.color = primitiveRendererComp["Color"].as<glm::vec4>();
				}
			}
		}
		return true;
	}

}
