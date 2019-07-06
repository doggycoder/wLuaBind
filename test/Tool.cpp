//
// Created by wuwang on 19-7-5.
//

#include <fstream>
#include <sstream>
#include "Tool.h"

std::string PlatformTool::loadString(const std::string &path) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string contents(buffer.str());
    return contents;
}