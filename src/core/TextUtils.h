#pragma once

#include <string>
#include <vector>

std::vector<int> Utf8ToCodepoints(const std::string& text);
std::string CodepointToUtf8(int codepoint);
std::string CodepointsToUtf8(const std::vector<int>& codepoints);
int ToLowerCodepoint(int codepoint);
