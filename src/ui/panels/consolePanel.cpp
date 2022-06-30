#include "consolePanel.h"
#include "imgui.h"
#include "utils/utility.h"
#include <format>
#include <vector>
#include "ui/ui.h"
#include "spdlog/common.h"
namespace FlatEngine{
    struct ConsoleText{
        std::string text;
        ImVec4 color;
    };
    static std::vector<ConsoleText> buff;
    void AddLog(const char* fmt, spdlog::level::level_enum level)
    {
        ConsoleText text;
        text.text = fmt;
        switch (level)
        {
        case SPDLOG_LEVEL_INFO:
            text.color = ImVec4(1,1,1,1);    
            break;
        case SPDLOG_LEVEL_WARN:
            text.color = ImVec4(0.2, 0,0,1);
        case SPDLOG_LEVEL_ERROR:
            text.color = ImVec4(1,0,0,1);
        default:
            text.color = ImVec4(1,1,1,1);    
            break;
        }
        buff.emplace_back(text);
    }
    static void ClearLog(){
        buff.clear();
    }
    void ConsolePanel::DrawConsole(){
        if (!UI::console_window) return;
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
            for(auto cText : buff){
                ImGui::TextColored(cText.color, cText.text.c_str());
            }
		ImGui::EndChild();
		ImGui::PushItemWidth(m_width - 10);
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
		if(ImGui::InputText("##", InputBuf, FE_ARRAYSIZE(InputBuf), inputFlags)){
            memset(InputBuf, 0, sizeof(InputBuf));
        }
		ImGui::PopItemWidth();
		ImGui::End();
    }
}