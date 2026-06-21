#include "SettingsState.h"
#include "../core/Game.h"
#include <raylib.h>
#include "MainMenuState.h"

void SettingsState::Init(Game* game) {
    gamePtr = game;
}

void SettingsState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    } else if (IsKeyPressed(KEY_T)) {
        gamePtr->ToggleTheme();
    }
}

void SettingsState::Update(float deltaTime) {
}

void SettingsState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const Rectangle card = gamePtr->ScaleRect({ 365.0f, 160.0f, 550.0f, 340.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, "Settings", gamePtr->ScalePoint({ 520.0f, 220.0f }), 42.0f * scale, 1.0f * scale, theme.Title);
    DrawTextEx(font, "Press T to Toggle Theme", gamePtr->ScalePoint({ 455.0f, 320.0f }), 22.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, "Press ESC to return to Menu", gamePtr->ScalePoint({ 430.0f, 370.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
}
