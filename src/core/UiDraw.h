#pragma once

#include <raylib.h>

class Game;

namespace Ui {
Color Fade(Color color, float alpha);
Rectangle Inflate(Rectangle rect, float amount);

void DrawText(Game* game, Font font, const char* text, Vector2 position, float fontSize, float spacing, Color color);
void DrawFittedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, float spacing, Color color, float minFontSize = 11.0f);
void DrawCenteredText(Game* game, Font font, const char* text, Rectangle rect, float fontSize, float spacing, Color color);
void DrawCenteredFittedText(Game* game, Font font, const char* text, Rectangle rect, float fontSize, float spacing, Color color, float minFontSize = 9.0f);
void DrawRounded(Game* game, Rectangle rect, float roundness, int segments, Color color);
void DrawRoundedLines(Game* game, Rectangle rect, float roundness, int segments, Color color);
void DrawRect(Game* game, Rectangle rect, Color color);
void BeginScissor(Game* game, Rectangle rect);
}
