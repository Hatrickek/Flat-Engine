#pragma once

#include "core/layer.h"

namespace FlatEngine {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

		void SetTheme(int index);
		int selectedTheme;
	private:
		void ImGuiDarkTheme();
	};
}
