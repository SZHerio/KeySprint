#include "UiDraw.h"
#include "Game.h"

#include <algorithm>

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
