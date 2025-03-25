// Logger.h
#pragma once
#include <string>
#include <vector>
class Logger {
public:
    void addLog(const std::string& log);
    void draw(const std::string& title);
private:
    std::vector<std::string> logs;
};
