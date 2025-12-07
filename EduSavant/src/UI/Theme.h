#pragma once
#include "imgui.h"

namespace Theme {
    inline void Setup(bool dark = true) {
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.WindowRounding = 5.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.ScrollbarRounding = 12.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 6.0f;
        style.FramePadding = ImVec2(10.0f, 10.0f);
        style.ItemSpacing = ImVec2(12.0f, 8.0f);

        if (dark) {
            ImGui::StyleColorsDark();
            ImVec4* colors = style.Colors;
            // Professional Dark Blue/Grey Theme (EduSavant Dark)
            colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
            colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        } else {
            ImGui::StyleColorsLight();
            ImVec4* colors = style.Colors;
            // Professional Light Theme
            colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.96f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.88f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.75f, 0.90f, 1.00f);
            // Additional tweaks for light mode readability could go here
        }
    }
}
