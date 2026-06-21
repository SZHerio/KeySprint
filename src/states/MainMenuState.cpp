#include "MainMenuState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include "../core/LessonLibrary.h"
#include "TypingState.h"
#include "SettingsState.h"

void MainMenuState::Init(Game* game) {
    gamePtr = game;
    highlightY = 292.0f;
}

void MainMenuState::HandleInput() {
    bool activateOption = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        const Vector2 mouse = GetMousePosition();
        for (size_t i = 0; i < options.size(); ++i) {
            const Rectangle optionRect = gamePtr->ScaleRect({ 450.0f, 292.0f + static_cast<float>(i) * 66.0f, 380.0f, 48.0f });
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
        } else {
            gamePtr->ChangeState(std::make_shared<SettingsState>());
        }
    }
}

void MainMenuState::Update(float deltaTime) {
    const float targetY = 292.0f + selectedOption * 66.0f;
    highlightY += (targetY - highlightY) * std::min(1.0f, deltaTime * 14.0f);
}

void MainMenuState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const Rectangle card = gamePtr->ScaleRect({ 390.0f, 92.0f, 500.0f, 520.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, "KeySprint", gamePtr->ScalePoint({ 468.0f, 150.0f }), 52.0f * scale, 1.0f * scale, theme.Title);
    DrawTextEx(font, "Typing speed trainer", gamePtr->ScalePoint({ 482.0f, 212.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, TextFormat("%s | Best %.0f WPM | Lessons %d",
        LessonLibrary::GetLanguageLabel(gamePtr->GetLanguage()).c_str(),
        gamePtr->GetProgress().GetBestWpm(),
        gamePtr->GetProgress().GetCompletedLessons()),
        gamePtr->ScalePoint({ 430.0f, 245.0f }), 16.0f * scale, 1.0f * scale, theme.TextDefault);

    const Rectangle highlight = gamePtr->ScaleRect({ 450.0f, highlightY, 380.0f, 48.0f });
    DrawRectangleRounded(highlight, 0.35f, 10, Fade(theme.Highlight, 0.18f));
    DrawRectangleRoundedLines(highlight, 0.35f, 10, Fade(theme.Highlight, 0.48f));

    for (size_t i = 0; i < options.size(); ++i) {
        const float y = 302.0f + static_cast<float>(i) * 66.0f;
        const Color color = selectedOption == static_cast<int>(i) ? theme.TextCorrect : theme.TextDefault;
        DrawTextEx(font, options[i], gamePtr->ScalePoint({ 520.0f, y }), 25.0f * scale, 1.0f * scale, color);
    }

    DrawTextEx(font, "Use Up/Down, Enter or Mouse", gamePtr->ScalePoint({ 462.0f, 535.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
}
