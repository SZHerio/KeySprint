#include "ResultsState.h"
#include "../core/Game.h"
#include <raylib.h>
#include "MainMenuState.h"
#include "TypingState.h"

ResultsState::ResultsState(float finalWPM, float finalAccuracy, TypingMode mode)
    : wpm(finalWPM), accuracy(finalAccuracy), retryMode(mode) {}

void ResultsState::Init(Game* game) {
    gamePtr = game;
}

void ResultsState::HandleInput() {
    if (IsKeyPressed(KEY_ENTER)) {
        gamePtr->ChangeState(std::make_shared<TypingState>(retryMode));
    } else if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    }
}

void ResultsState::Update(float deltaTime) {
}

void ResultsState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const Rectangle card = gamePtr->ScaleRect({ 365.0f, 130.0f, 550.0f, 430.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, "Results", gamePtr->ScalePoint({ 530.0f, 190.0f }), 42.0f * scale, 1.0f * scale, theme.Title);
    
    DrawTextEx(font, TextFormat("WPM: %.0f", wpm), gamePtr->ScalePoint({ 500.0f, 285.0f }), 30.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, TextFormat("Accuracy: %.0f%%", accuracy), gamePtr->ScalePoint({ 500.0f, 335.0f }), 30.0f * scale, 1.0f * scale, theme.TextCorrect);

    DrawTextEx(font, "Press ENTER to Try Again", gamePtr->ScalePoint({ 450.0f, 455.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawTextEx(font, "Press ESC to return to Menu", gamePtr->ScalePoint({ 438.0f, 500.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
}
