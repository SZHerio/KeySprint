#include "MainMenuState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include "../core/LessonLibrary.h"
#include "TypingState.h"
#include "SettingsState.h"

namespace {
void DrawWrappedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, float spacing, Color color) {
    std::istringstream words(text);
    std::string word;
    std::string line;
    float y = position.y;

    while (words >> word) {
        const std::string candidate = line.empty() ? word : line + " " + word;
        const float width = MeasureTextEx(font, candidate.c_str(), fontSize, spacing).x;

        if (width > maxWidth && !line.empty()) {
            const float scale = game->GetUiScale();
            DrawTextEx(font, line.c_str(), game->ScalePoint({ position.x, y }), fontSize * scale, spacing * scale, color);
            line = word;
            y += fontSize + 6.0f;
        } else {
            line = candidate;
        }
    }

    if (!line.empty()) {
        const float scale = game->GetUiScale();
        DrawTextEx(font, line.c_str(), game->ScalePoint({ position.x, y }), fontSize * scale, spacing * scale, color);
    }
}
}

void MainMenuState::Init(Game* game) {
    gamePtr = game;
    highlightY = 272.0f;
}

void MainMenuState::HandleInput() {
    bool activateOption = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        const Vector2 mouse = GetMousePosition();
        for (size_t i = 0; i < options.size(); ++i) {
            const Rectangle optionRect = gamePtr->ScaleRect({ 415.0f, 272.0f + static_cast<float>(i) * 58.0f, 450.0f, 46.0f });
            if (CheckCollisionPointRec(mouse, optionRect)) {
                selectedOption = static_cast<int>(i);
                activateOption = true;
                break;
            }
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % static_cast<int>(options.size());
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption + static_cast<int>(options.size()) - 1) % static_cast<int>(options.size());
    } else if (IsKeyPressed(KEY_ENTER) || activateOption) {
        if (selectedOption == 0) {
            gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Practice));
        } else if (selectedOption == 1) {
            gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Tutorial));
        } else if (selectedOption == 2) {
            gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Composition));
        } else {
            gamePtr->ChangeState(std::make_shared<SettingsState>());
        }
    }
}

void MainMenuState::Update(float deltaTime) {
    menuTime += deltaTime;
    const float targetY = 272.0f + selectedOption * 58.0f;
    highlightY += (targetY - highlightY) * std::min(1.0f, deltaTime * 14.0f);
}

void MainMenuState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const float glow = (std::sin(menuTime * 2.1f) + 1.0f) * 0.5f;
    const Rectangle card = gamePtr->ScaleRect({ 310.0f, 70.0f, 660.0f, 585.0f });
    DrawRectangleRounded(card, 0.09f, 16, Fade(theme.Panel, 0.82f));
    DrawRectangleRoundedLines(card, 0.09f, 16, Fade(theme.PanelBorder, 0.80f));
    DrawRectangleRounded(gamePtr->ScaleRect({ 335.0f, 95.0f, 610.0f, 535.0f }), 0.08f, 16, Fade(theme.Highlight, 0.04f + glow * 0.035f));

    DrawTextEx(font, "KeySprint", gamePtr->ScalePoint({ 438.0f, 122.0f }), 54.0f * scale, 1.0f * scale, theme.Title);
    DrawTextEx(font, "Typing speed trainer", gamePtr->ScalePoint({ 470.0f, 185.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, TextFormat("%s | Best %.0f WPM | Lessons %d",
        LessonLibrary::GetLanguageLabel(gamePtr->GetLanguage()).c_str(),
        gamePtr->GetProgress().GetBestWpm(),
        gamePtr->GetProgress().GetCompletedLessons()),
        gamePtr->ScalePoint({ 410.0f, 218.0f }), 16.0f * scale, 1.0f * scale, theme.TextDefault);

    const Rectangle highlight = gamePtr->ScaleRect({ 415.0f, highlightY, 450.0f, 46.0f });
    DrawRectangleRounded(highlight, 0.35f, 10, Fade(theme.Highlight, 0.18f));
    DrawRectangleRoundedLines(highlight, 0.35f, 10, Fade(theme.Highlight, 0.48f));

    const Rectangle detail = gamePtr->ScaleRect({ 400.0f, 518.0f, 480.0f, 72.0f });
    DrawRectangleRounded(detail, 0.18f, 10, Fade(theme.PanelBorder, 0.18f));
    DrawRectangleRoundedLines(detail, 0.18f, 10, Fade(theme.Highlight, 0.35f));
    DrawWrappedText(gamePtr, font, descriptions[selectedOption], { 425.0f, 535.0f }, 430.0f, 15.0f, 1.0f, theme.TextDefault);

    for (size_t i = 0; i < options.size(); ++i) {
        const float y = 282.0f + static_cast<float>(i) * 58.0f;
        const Color color = selectedOption == static_cast<int>(i) ? theme.TextCorrect : theme.TextDefault;
        const float dotPulse = selectedOption == static_cast<int>(i) ? glow : 0.15f;
        DrawCircleV(gamePtr->ScalePoint({ 440.0f, y + 14.0f }), (5.0f + dotPulse * 3.0f) * scale, selectedOption == static_cast<int>(i) ? theme.Highlight : Fade(theme.TextDefault, 0.35f));
        DrawTextEx(font, options[i], gamePtr->ScalePoint({ 470.0f, y }), 23.0f * scale, 1.0f * scale, color);
    }

    DrawTextEx(font, "Use Up/Down, Enter or Mouse", gamePtr->ScalePoint({ 457.0f, 606.0f }), 17.0f * scale, 1.0f * scale, theme.TextDefault);
}
