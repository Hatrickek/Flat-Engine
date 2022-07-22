#pragma once
#include "utils/timestep.h"
#include <string>
namespace FlatEngine{
	class Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float DeltaTime) {}
		virtual void OnImGuiRender(){}
		//virtual void OnEvent()

		 Layer& Get() { return *this;}
		const std::string& GetName() const {return m_DebugName;}
	protected:
		std::string m_DebugName;
	};
}

