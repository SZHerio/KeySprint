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

    DrawTextEx(font, "Settings", {320, 150}, 40, 1, theme.Title);
    DrawTextEx(font, "Press T to Toggle Theme", {250, 250}, 20, 1, theme.TextCorrect);
    DrawTextEx(font, "Press ESC to return to Menu", {250, 300}, 20, 1, theme.TextDefault);
}
