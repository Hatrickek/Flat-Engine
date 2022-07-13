#include "sceneHPanel.h"

#include <glm/gtc/type_ptr.inl>
#include "imgui.h"
#include "imgui_internal.h"
#include "panelSettings.h"
#include "editorlayer.h"
namespace FlatEngine {
	bool SceneHPanel::scenehierarchy_window = true;
	Entity SceneHPanel::m_SelectionContext = {};
	bool SceneHPanel::renameEntity = false;
	void SceneHPanel::DrawPanel() {
		if(!scenehierarchy_window) return;

		ImGui::Begin("Scene Hierarchy");
		EditorLayer::Get().GetActiveScene()->m_Registry.each([&](auto entityID) {
			Entity entity{ entityID, EditorLayer::Get().GetActiveScene().get() };
			DrawEntityNode(entity);
			});

		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		if(ImGui::BeginPopupContextWindow(0, 1, false)) {
			if(ImGui::BeginMenu("Create")) {
				if(ImGui::MenuItem("Empty Entity")) {
					EditorLayer::Get().GetActiveScene()->CreateEntity("New Entity");
				}
				if(ImGui::MenuItem("Empty Mesh")) {
					EditorLayer::Get().GetActiveScene()->CreateEntity("New Mesh").AddComponent<MeshRendererComponent>();
				}
				if(ImGui::MenuItem("Cube")) {
					EditorLayer::Get().GetActiveScene()->CreateEntity("Cube").AddComponent<PrimitiveRendererComponent>();
				}
				if(ImGui::MenuItem("Light")) {
					EditorLayer::Get().GetActiveScene()->CreateEntity("New Light").AddComponent<LightComponent>();
				}
				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		DrawInspectorPanel();
		ImGui::End();
	}
	Entity SceneHPanel::GetSelectedEntity() {
		return m_SelectionContext;
	}
	template <typename T, typename ComponentFunction>
	static void DrawComponent(const std::string& name, Entity entity, ComponentFunction componentFunction) {
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if(entity.HasComponent<T>()) {
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if(ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if(ImGui::BeginPopup("ComponentSettings")) {
				if(ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if(open) {
				componentFunction(component);
				ImGui::TreePop();
			}

			if(removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHPanel::DrawEntityNode(Entity entity) {
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if(ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}
		bool entityDeleted = false;
		bool entityDuplicated = false;
		if(ImGui::BeginPopupContextItem()) {
			if(ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			if(ImGui::MenuItem("Duplicate Entity"))
				entityDuplicated = true;
			ImGui::EndPopup();
		}

		if(opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			//bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			bool opened = false;
			if(opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if(entityDeleted) {
			EditorLayer::Get().GetActiveScene()->DestroyEntity(entity);
			if(m_SelectionContext == entity)
				m_SelectionContext = {};
		}
		if(entityDuplicated) {
			Entity ent = EditorLayer::Get().GetActiveScene()->DuplicateEntity(entity);
			m_SelectionContext = ent;
		}
	}

	void SceneHPanel::DrawInspectorPanel() {
		if(!PanelSettings::inspector_window) return;
		ImGui::Begin("Inspector");
		if(m_SelectionContext) {
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if(ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHPanel::DrawComponents(Entity entity) {
		if(entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[128] = {};
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if(renameEntity)
				ImGui::SetKeyboardFocusHere();
			if(ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if(ImGui::Button("Add Component")) {
			ImGui::OpenPopup("Add Component");
		}
		if(ImGui::BeginPopup("Add Component")) {
			//Add component panel
			if(ImGui::MenuItem("Mesh Renderer")) {
				if(!m_SelectionContext.HasComponent<MeshRendererComponent>())
					m_SelectionContext.AddComponent<MeshRendererComponent>();
				else
					FE_LOG_WARN("This entity already has the Mesh Renderer Component!");
				ImGui::CloseCurrentPopup();
			}

			if(ImGui::MenuItem("Primitive Renderer")) {
				if(!m_SelectionContext.HasComponent<PrimitiveRendererComponent>())
					m_SelectionContext.AddComponent<PrimitiveRendererComponent>();
				else
					FE_LOG_WARN("This entity already has the Primitive Renderer Component!");
				ImGui::CloseCurrentPopup();
			}

			if(ImGui::MenuItem("Light Component")) {
				if(!m_SelectionContext.HasComponent<LightComponent>())
					m_SelectionContext.AddComponent<LightComponent>();
				else
					FE_LOG_WARN("This entity already has the Light Component!");
				ImGui::CloseCurrentPopup();
			}
			if(ImGui::MenuItem("Rigidbody")) {
				if(!m_SelectionContext.HasComponent<RigidBodyComponent>())
					m_SelectionContext.AddComponent<RigidBodyComponent>();
				else
					FE_LOG_WARN("This entity already has the Rigidbody Component!");
				ImGui::CloseCurrentPopup();
			}
			if(ImGui::MenuItem("Box Collider")) {
				if(!m_SelectionContext.HasComponent<BoxColliderComponent>())
					m_SelectionContext.AddComponent<BoxColliderComponent>();
				else
					FE_LOG_WARN("This entity already has the Box Collider Component!");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();
		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
			DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
			});
		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component) {
			auto& mesh = component.model;
			auto& shader = component.shader;
			auto& diffuseColor = component.diffuseColor;
			auto& textures = component.model->textures_loaded;
			ImGui::Text("Model: %s", component.model->name.c_str());
			ImGui::Text("Textures: %d", textures.size());
			ImGui::ColorEdit4("Diffuse color", glm::value_ptr(diffuseColor));
			});
		DrawComponent<PrimitiveRendererComponent>("Primitive Renderer", entity, [](auto& component) {
			auto& primitive = component.primitive;
			auto& shader = component.shader;
			auto& color = component.color;
			const char* primitives[] = { "EMPTY", "CUBE","QUAD" };
			static int selected_primitive = component.primitive;
			if(ImGui::Combo("Primitive", &selected_primitive, primitives, FE_ARRAYSIZE(primitives))) {
				primitive = static_cast<Primitive>(selected_primitive);
			}
			ImGui::ColorEdit4("Color", glm::value_ptr(color));
			});
		DrawComponent<LightComponent>("Light Component", entity, [](auto& component) {
			auto& light_type = component.type;
			auto& color = component.color;
			auto& enabled = component.enabled;
			ImGui::Checkbox("Enabled", &enabled);
			const char* types[] = { "Directional", "Point" };
			static int selected_type = 1;
			if(ImGui::Combo("Type", &selected_type, types, FE_ARRAYSIZE(types))) {
				light_type = static_cast<LightType>(selected_type);
			}
			ImGui::ColorEdit4("Color", glm::value_ptr(color));
			});
		DrawComponent<RigidBodyComponent>("Rigidbody Component", entity, [](auto& component) {
			auto& type = component.type;
			const char* types[] = { "Static", "Dynamic" };
			static int selected_type = 0;
			if(ImGui::Combo("Type", &selected_type, types, FE_ARRAYSIZE(types))) {
				type = static_cast<RigidBodyType>(selected_type);
			}
			});
		DrawComponent<BoxColliderComponent>("Box Collider Component", entity, [](auto& component) {
			DrawVec3Control("Size", component.size, 1.0f);
			});
	}
}