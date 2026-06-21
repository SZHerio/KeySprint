#include "MainMenuState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include "TypingState.h"
#include "SettingsState.h"

void MainMenuState::Init(Game* game) {
    gamePtr = game;
    highlightY = 250.0f;
}

void MainMenuState::HandleInput() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % static_cast<int>(options.size());
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption + static_cast<int>(options.size()) - 1) % static_cast<int>(options.size());
    } else if (IsKeyPressed(KEY_ENTER)) {
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
    const float targetY = 245.0f + selectedOption * 55.0f;
    highlightY += (targetY - highlightY) * std::min(1.0f, deltaTime * 14.0f);
}

void MainMenuState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();

    DrawTextEx(font, "KeySprint", {260, 120}, 46, 1, theme.Title);
    DrawTextEx(font, "Typing speed trainer", {262, 172}, 20, 1, theme.TextDefault);

    DrawRectangleRounded({235.0f, highlightY, 330.0f, 42.0f}, 0.35f, 10, Fade(theme.Title, 0.18f));
    DrawRectangleRoundedLines({235.0f, highlightY, 330.0f, 42.0f}, 0.35f, 10, Fade(theme.Title, 0.45f));

    for (size_t i = 0; i < options.size(); ++i) {
        const float y = 252.0f + static_cast<float>(i) * 55.0f;
        const Color color = selectedOption == static_cast<int>(i) ? theme.TextCorrect : theme.TextDefault;
        DrawTextEx(font, options[i], {280.0f, y}, 24, 1, color);
    }

    DrawTextEx(font, "Use Up/Down and Enter", {250, 465}, 18, 1, theme.TextDefault);
}
