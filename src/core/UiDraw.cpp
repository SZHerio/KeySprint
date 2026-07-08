#include "UiDraw.h"
#include "Game.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
std::vector<std::string> SplitLines(const char* text) {
    std::vector<std::string> lines;
    std::string current;
    const char* cursor = text;
    while (*cursor != '\0') {
        if (*cursor == '\n') {
            lines.push_back(current);
            current.clear();
        } else {
            current.push_back(*cursor);
        }
        ++cursor;
    }
    lines.push_back(current);
    return lines;
}

Vector2 MeasureTextBlock(Font font, const std::vector<std::string>& lines, float fontSize, float spacing, float lineGap) {
    Vector2 size{ 0.0f, 0.0f };
    for (const std::string& line : lines) {
        const Vector2 lineSize = MeasureTextEx(font, line.c_str(), fontSize, spacing);
        size.x = std::max(size.x, lineSize.x);
        size.y += lineSize.y;
    }

    if (lines.size() > 1) {
        size.y += static_cast<float>(lines.size() - 1) * lineGap;
    }
    return size;
}
}

namespace Ui {
Color Fade(Color color, float alpha) {
    color.a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);
    return color;
}

Rectangle Inflate(Rectangle rect, float amount) {
    return { rect.x - amount, rect.y - amount, rect.width + amount * 2.0f, rect.height + amount * 2.0f };
}

void DrawText(Game* game, Font font, const char* text, Vector2 position, float fontSize, float spacing, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, spacing * scale, color);
}

void DrawFittedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, float spacing, Color color, float minFontSize) {
    float adjustedSize = fontSize;
    while (adjustedSize > minFontSize && MeasureTextEx(font, text, adjustedSize, spacing).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    DrawText(game, font, text, position, adjustedSize, spacing, color);
}

void DrawCenteredText(Game* game, Font font, const char* text, Rectangle rect, float fontSize, float spacing, Color color) {
    const std::vector<std::string> lines = SplitLines(text);
    const float lineGap = std::max(1.0f, fontSize * 0.14f);
    const Vector2 blockSize = MeasureTextBlock(font, lines, fontSize, spacing, lineGap);
    float y = rect.y + (rect.height - blockSize.y) * 0.5f;

    for (const std::string& line : lines) {
        const Vector2 lineSize = MeasureTextEx(font, line.c_str(), fontSize, spacing);
        DrawText(game, font, line.c_str(), { rect.x + (rect.width - lineSize.x) * 0.5f, y }, fontSize, spacing, color);
        y += lineSize.y + lineGap;
    }
}

void DrawCenteredFittedText(Game* game, Font font, const char* text, Rectangle rect, float fontSize, float spacing, Color color, float minFontSize) {
    float adjustedSize = fontSize;
    while (adjustedSize > minFontSize) {
        const std::vector<std::string> lines = SplitLines(text);
        const float lineGap = std::max(1.0f, adjustedSize * 0.14f);
        const Vector2 blockSize = MeasureTextBlock(font, lines, adjustedSize, spacing, lineGap);
        if (blockSize.x <= rect.width - 6.0f && blockSize.y <= rect.height - 4.0f) {
            break;
        }
        adjustedSize -= 1.0f;
    }

    DrawCenteredText(game, font, text, rect, adjustedSize, spacing, color);
}

void DrawRounded(Game* game, Rectangle rect, float roundness, int segments, Color color) {
    DrawRectangleRounded(game->ScaleRect(rect), roundness, segments, color);
}

void DrawRoundedLines(Game* game, Rectangle rect, float roundness, int segments, Color color) {
    DrawRectangleRoundedLines(game->ScaleRect(rect), roundness, segments, color);
}

void DrawRect(Game* game, Rectangle rect, Color color) {
    DrawRectangleRec(game->ScaleRect(rect), color);
}

void BeginScissor(Game* game, Rectangle rect) {
    const Rectangle scaled = game->ScaleRect(rect);
    BeginScissorMode(
        static_cast<int>(scaled.x),
        static_cast<int>(scaled.y),
        static_cast<int>(scaled.width),
        static_cast<int>(scaled.height)
    );
}
}
