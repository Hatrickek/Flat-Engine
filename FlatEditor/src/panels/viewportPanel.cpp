#include "viewportPanel.h"

#include "imgui.h"
#include "render/renderer.h"
#include "ImGuizmo/ImGuizmo.h"
#include "sceneHPanel.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/feMath.h"
#include "panelSettings.h"

#include "core/components.h"
namespace FlatEngine {
	static int m_GizmoType = -1;
	static ImVec2 m_ViewportSize;
	ImVec2 ViewportPanel::m_ViewportBounds[2];
	ImVec2 ViewportPanel::viewportPanelSize;
	ImVec2 viewportOffset;
	bool ViewportPanel::viewportFocused;
	bool ViewportPanel::viewportHovered;
	void ViewportPanel::DrawPanel() {
		DrawPerformanceOverlay();
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", NULL, flags);
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		viewportFocused = ImGui::IsWindowFocused();
		viewportHovered = ImGui::IsWindowHovered();

		viewportPanelSize = ImGui::GetContentRegionAvail();
		Renderer::frameBufferSize = { viewportPanelSize.x, viewportPanelSize.y };
		if(m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y) {
			Renderer::ResizeBuffers();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}
		uint64_t textureID = 0;
		if(Renderer::GetFramebuffer())
			textureID = Renderer::GetFramebuffer()->colorBuffer;
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if(ImGui::BeginDragDropTarget()) {
			//if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(CONTENT_PAYLOAD_ITEM)){
			//
			//}
			ImGui::EndDragDropTarget();
		}
		ViewportPanel::DrawGizmo();
		ImGui::End();
		ImGui::PopStyleVar();
	}
	void ViewportPanel::DrawGizmo() {

		Entity selectedEntity = SceneHPanel::GetSelectedEntity();
		if(selectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ViewportPanel::m_ViewportBounds[0].x, ViewportPanel::m_ViewportBounds[0].y, ViewportPanel::m_ViewportBounds[1].x - ViewportPanel::m_ViewportBounds[0].x, ViewportPanel::m_ViewportBounds[1].y - ViewportPanel::m_ViewportBounds[0].y);

			glm::mat4 cameraProjection = Renderer::GetCamera(0)->GetProjectionMatrix();
			glm::mat4 cameraView = Renderer::GetCamera(0)->GetViewMatrix();
			// Entity transform
			glm::mat4 transform;

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			transform = tc.GetTransform();

			// Snapping
			bool snap = Input::GetKey(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if(m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if(ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}
		if(Input::GetKey(Key::LeftControl)) {
			if(Input::GetKeyDown(Key::Q)) {
				if(!ImGuizmo::IsUsing())
					m_GizmoType = -1;
			}
			if(Input::GetKeyDown(Key::W)) {
				if(!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
			if(Input::GetKeyDown(Key::E)) {
				if(!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
			if(Input::GetKeyDown(Key::R)) {
				if(!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
		}

	}
	void ViewportPanel::DrawPerformanceOverlay() {
		if(!PanelSettings::performance_overlay) return;
		static int corner = 1;
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;
		if(corner != -1) {
			const float PAD_X = 20;
			const float PAD_Y = 40;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = viewportOffset; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewportPanelSize;
			ImVec2 window_pos, window_pos_pivot;
			window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD_X) : (work_pos.x + PAD_X);
			window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD_Y) : (work_pos.y + PAD_Y);
			window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
			window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
			window_flags |= ImGuiWindowFlags_NoMove;
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if(ImGui::Begin("Performance Overlay", NULL, window_flags)) {
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		if(ImGui::BeginPopupContextWindow()) {
			if(ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if(ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if(ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if(ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if(ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if(PanelSettings::performance_overlay && ImGui::MenuItem("Close")) PanelSettings::performance_overlay = false;
			ImGui::EndPopup();
		}
		ImGui::End();
	}
}