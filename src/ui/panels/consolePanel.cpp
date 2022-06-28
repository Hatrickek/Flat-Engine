#include "consolePanel.h"
#include "imgui.h"
#include "utils/utility.h"
#include <format>
#include <vector>
namespace FlatEngine{
    static std::vector<std::string> buff;
    static void ClearLog(){
        buff.clear();
    }
    static void AddLog(const char* fmt, va_list args)
    {
        //std::string log = std::format(fmt, args);
        buff.emplace_back(fmt);
    }

    void ConsolePanel::DrawConsole(){
        //if (!console_window) return; TODO:
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoFocusOnAppearing;

		static char InputBuf[256];
		ImGui::Begin("Console", NULL, window_flags);
		float m_width   = ImGui::GetWindowWidth();
		float m_height  = ImGui::GetWindowHeight();
        if(ImGui::Button("Clear")){
            ClearLog();
        }
        ImGui::Separator();
		ImGui::BeginChild("Console window", ImVec2(0, m_height - 100), false, ImGuiWindowFlags_AlwaysAutoResize);
            for(auto text : buff){
                ImGui::Text(text.c_str());
            }
		ImGui::EndChild();
		ImGui::PushItemWidth(m_width - 10);
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
		if(ImGui::InputText("##", InputBuf, FE_ARRAYSIZE(InputBuf), inputFlags)){
            AddLog(InputBuf, 0);
            memset(InputBuf, 0, sizeof(InputBuf));
        }
		ImGui::PopItemWidth();
		ImGui::End();
    }
}