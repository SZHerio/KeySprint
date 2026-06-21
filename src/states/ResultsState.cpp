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

    DrawTextEx(font, "Results", {320, 150}, 40, 1, theme.Title);
    
    DrawTextEx(font, TextFormat("WPM: %.0f", wpm), {320, 250}, 30, 1, theme.TextCorrect);
    DrawTextEx(font, TextFormat("Accuracy: %.0f%%", accuracy), {320, 300}, 30, 1, theme.TextCorrect);

    DrawTextEx(font, "Press ENTER to Try Again", {250, 450}, 20, 1, theme.TextDefault);
    DrawTextEx(font, "Press ESC to return to Menu", {250, 500}, 20, 1, theme.TextDefault);
}
