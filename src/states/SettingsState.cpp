#include "SettingsState.h"
#include "../core/Game.h"
#include <raylib.h>
#include "../core/AudioManager.h"
#include "../core/LessonLibrary.h"
#include "MainMenuState.h"

void SettingsState::Init(Game* game) {
    gamePtr = game;
}

void SettingsState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    } else if (IsKeyPressed(KEY_T)) {
        gamePtr->ToggleTheme();
    } else if (IsKeyPressed(KEY_L)) {
        gamePtr->ToggleLanguage();
    } else if (IsKeyPressed(KEY_A)) {
        AudioManager::ToggleEnabled();
    } else if (IsKeyPressed(KEY_C)) {
        AudioManager::CycleClickProfile();
    } else if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
        AudioManager::IncreaseVolume();
    } else if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        AudioManager::DecreaseVolume();
    } else if (IsKeyPressed(KEY_R)) {
        gamePtr->GetProgress().Reset();
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        const Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 455.0f, 298.0f, 370.0f, 34.0f }))) gamePtr->ToggleTheme();
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 455.0f, 338.0f, 370.0f, 34.0f }))) gamePtr->ToggleLanguage();
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 455.0f, 378.0f, 370.0f, 34.0f }))) AudioManager::ToggleEnabled();
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 455.0f, 418.0f, 370.0f, 34.0f }))) AudioManager::CycleClickProfile();
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 455.0f, 498.0f, 370.0f, 34.0f }))) gamePtr->GetProgress().Reset();
    }
}

void SettingsState::Update(float deltaTime) {
}

void SettingsState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const Rectangle card = gamePtr->ScaleRect({ 345.0f, 105.0f, 590.0f, 505.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, "Settings", gamePtr->ScalePoint({ 515.0f, 145.0f }), 42.0f * scale, 1.0f * scale, theme.Title);
    DrawTextEx(font, TextFormat("T  Theme toggle"), gamePtr->ScalePoint({ 455.0f, 300.0f }), 20.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, TextFormat("L  Language: %s", LessonLibrary::GetLanguageLabel(gamePtr->GetLanguage()).c_str()), gamePtr->ScalePoint({ 455.0f, 340.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, TextFormat("A  Sound: %s", AudioManager::IsEnabled() ? "ON" : "OFF"), gamePtr->ScalePoint({ 455.0f, 380.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, TextFormat("C  Click profile: %d", AudioManager::GetClickProfile() + 1), gamePtr->ScalePoint({ 455.0f, 420.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, TextFormat("+/- Volume: %.0f%%", AudioManager::GetVolume() * 100.0f), gamePtr->ScalePoint({ 455.0f, 460.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, "R  Reset progress", gamePtr->ScalePoint({ 455.0f, 500.0f }), 20.0f * scale, 1.0f * scale, theme.TextError);
    DrawTextEx(font, "ESC  Return to Menu", gamePtr->ScalePoint({ 455.0f, 552.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
}
