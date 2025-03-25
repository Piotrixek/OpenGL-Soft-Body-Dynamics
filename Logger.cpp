// Logger.cpp
#include "Logger.h"
#include "imgui.h"
void Logger::addLog(const std::string& log) {
    logs.push_back(log);
}
void Logger::draw(const std::string& title) {
    ImGui::Begin(title.c_str());
    for (auto& log : logs) {
        ImGui::TextUnformatted(log.c_str());
    }
    ImGui::End();
}
