#include "TextUtils.h"

#include <cctype>

std::vector<int> Utf8ToCodepoints(const std::string& text) {
    std::vector<int> result;

    for (size_t i = 0; i < text.size();) {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        int codepoint = 0;
        size_t advance = 1;

        if ((c & 0x80) == 0) {
            codepoint = c;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < text.size()) {
            codepoint = ((c & 0x1F) << 6) | (static_cast<unsigned char>(text[i + 1]) & 0x3F);
            advance = 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < text.size()) {
            codepoint = ((c & 0x0F) << 12) |
                ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                (static_cast<unsigned char>(text[i + 2]) & 0x3F);
            advance = 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < text.size()) {
            codepoint = ((c & 0x07) << 18) |
                ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) |
                ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) |
                (static_cast<unsigned char>(text[i + 3]) & 0x3F);
            advance = 4;
        } else {
            codepoint = '?';
        }

        result.push_back(codepoint);
        i += advance;
    }

    return result;
}

std::string CodepointToUtf8(int codepoint) {
    std::string result;

    if (codepoint <= 0x7F) {
        result.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
        result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    return result;
}

std::string CodepointsToUtf8(const std::vector<int>& codepoints) {
    std::string result;
    for (int codepoint : codepoints) {
        result += CodepointToUtf8(codepoint);
    }
    return result;
}

int ToLowerCodepoint(int codepoint) {
    if (codepoint >= 'A' && codepoint <= 'Z') {
        return std::tolower(codepoint);
    }

    // Cyrillic uppercase А-Я.
    if (codepoint >= 0x0410 && codepoint <= 0x042F) {
        return codepoint + 0x20;
    }

    // Ё -> ё.
    if (codepoint == 0x0401) {
        return 0x0451;
    }

    return codepoint;
}
