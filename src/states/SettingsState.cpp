#include "SettingsState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <string>
#include "../core/AudioManager.h"
#include "../core/LessonLibrary.h"
#include "MainMenuState.h"

namespace {
Rectangle SettingsRow(int index) {
    return { 410.0f, 232.0f + index * 46.0f, 460.0f, 38.0f };
}

bool IsRu(Game* game) {
    return game->GetLanguage() == Language::Russian;
}

const char* LocalDifficulty(const std::string& difficulty, bool ru) {
    if (!ru) return difficulty.c_str();
    if (difficulty == "Relaxed") return u8"Легкая";
    if (difficulty == "Strict") return u8"Строгая";
    return u8"Нормальная";
}
}

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
    } else if (IsKeyPressed(KEY_D)) {
        gamePtr->GetProgress().CycleDifficulty();
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

    const Vector2 mouse = GetMousePosition();
    bool hover = false;
    for (int i = 0; i < 7; ++i) {
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect(SettingsRow(i)))) {
            hover = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (i == 0) gamePtr->ToggleTheme();
                if (i == 1) gamePtr->ToggleLanguage();
                if (i == 2) gamePtr->GetProgress().CycleDifficulty();
                if (i == 3) AudioManager::ToggleEnabled();
                if (i == 4) AudioManager::CycleClickProfile();
                if (i == 5) AudioManager::IncreaseVolume();
                if (i == 6) gamePtr->GetProgress().Reset();
            }
            break;
        }
    }
    SetMouseCursor(hover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void SettingsState::Update(float deltaTime) {
}

void SettingsState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const Rectangle card = gamePtr->ScaleRect({ 330.0f, 70.0f, 620.0f, 580.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, IsRu(gamePtr) ? u8"Настройки" : "Settings", gamePtr->ScalePoint({ 498.0f, 122.0f }), 42.0f * scale, 1.0f * scale, theme.Title);

    const Vector2 mouse = GetMousePosition();
    auto drawRow = [&](int index, const char* key, const char* label, Color color) {
        const Rectangle row = SettingsRow(index);
        const bool hover = CheckCollisionPointRec(mouse, gamePtr->ScaleRect(row));
        DrawRectangleRounded(gamePtr->ScaleRect(row), 0.28f, 10, Fade(hover ? theme.Highlight : theme.PanelBorder, hover ? 0.18f : 0.08f));
        DrawTextEx(font, key, gamePtr->ScalePoint({ row.x + 24.0f, row.y + 8.0f }), 19.0f * scale, 1.0f * scale, color);
        DrawTextEx(font, label, gamePtr->ScalePoint({ row.x + 70.0f, row.y + 8.0f }), 19.0f * scale, 1.0f * scale, color);
    };

    drawRow(0, "T", IsRu(gamePtr) ? u8"Тема" : "Theme toggle", theme.TextCorrect);
    drawRow(1, "L", TextFormat("%s: %s", IsRu(gamePtr) ? u8"Язык" : "Language", LessonLibrary::GetLanguageLabel(gamePtr->GetLanguage()).c_str()), theme.TextDefault);
    const std::string difficulty = gamePtr->GetProgress().GetDifficultyLabel();
    drawRow(2, "D", TextFormat("%s: %s (%.0f%%)", IsRu(gamePtr) ? u8"Сложность" : "Difficulty", LocalDifficulty(difficulty, IsRu(gamePtr)), gamePtr->GetProgress().GetUnlockAccuracyThreshold()), theme.TextDefault);
    drawRow(3, "A", TextFormat("%s: %s", IsRu(gamePtr) ? u8"Звук" : "Sound", AudioManager::IsEnabled() ? "ON" : "OFF"), theme.TextDefault);
    drawRow(4, "C", TextFormat("%s: %d", IsRu(gamePtr) ? u8"Профиль клика" : "Click profile", AudioManager::GetClickProfile() + 1), theme.TextDefault);
    drawRow(5, "+/-", TextFormat("%s: %.0f%%", IsRu(gamePtr) ? u8"Громкость" : "Volume", AudioManager::GetVolume() * 100.0f), theme.TextDefault);
    drawRow(6, "R", IsRu(gamePtr) ? u8"Сбросить прогресс" : "Reset progress", theme.TextError);
    DrawTextEx(font, IsRu(gamePtr) ? u8"F11  Оконный fullscreen | ESC  Меню" : "F11  Borderless fullscreen | ESC  Menu", gamePtr->ScalePoint({ 420.0f, 590.0f }), 16.0f * scale, 1.0f * scale, theme.TextDefault);
}
