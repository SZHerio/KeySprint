#include "ResultsState.h"
#include "../core/Game.h"
#include <raylib.h>
#include "MainMenuState.h"
#include "TypingState.h"

ResultsState::ResultsState(
    float finalWPM,
    float finalAccuracy,
    TypingMode mode,
    Language language,
    int lessonId,
    const std::string& lessonTitle,
    const std::map<std::string, int>& mistakes
) : wpm(finalWPM),
    accuracy(finalAccuracy),
    retryMode(mode),
    language(language),
    lessonId(lessonId),
    lessonTitle(lessonTitle),
    mistakes(mistakes) {}

void ResultsState::Init(Game* game) {
    gamePtr = game;
    if (!recorded && retryMode == TypingMode::Tutorial && lessonId >= 0) {
        gamePtr->GetProgress().RecordResult(language, lessonId, wpm, accuracy, mistakes);
        recorded = true;
    }
}

void ResultsState::HandleInput() {
    const Vector2 mouse = GetMousePosition();
    const bool retryClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 475.0f, 420.0f, 36.0f }));
    const bool menuClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 516.0f, 420.0f, 36.0f }));

    if (IsKeyPressed(KEY_ENTER) || retryClicked) {
        gamePtr->ChangeState(std::make_shared<TypingState>(retryMode, lessonId));
    } else if (IsKeyPressed(KEY_ESCAPE) || menuClicked) {
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

    const bool lessonPassed = retryMode == TypingMode::Tutorial && accuracy >= 85.0f;
    DrawTextEx(font, lessonPassed ? "Lesson Complete" : "Results", gamePtr->ScalePoint({ 435.0f, 175.0f }), 38.0f * scale, 1.0f * scale, theme.Title);
    
    DrawTextEx(font, TextFormat("WPM: %.0f", wpm), gamePtr->ScalePoint({ 470.0f, 265.0f }), 28.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, TextFormat("Accuracy: %.0f%%", accuracy), gamePtr->ScalePoint({ 470.0f, 310.0f }), 28.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, TextFormat("Best: %.0f WPM / %.0f%%", gamePtr->GetProgress().GetBestWpm(), gamePtr->GetProgress().GetBestAccuracy()), gamePtr->ScalePoint({ 470.0f, 360.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);

    if (retryMode == TypingMode::Tutorial) {
        const char* status = lessonPassed ? "Next lesson unlocked" : "Reach 85% accuracy to unlock next";
        DrawTextEx(font, lessonTitle.c_str(), gamePtr->ScalePoint({ 470.0f, 405.0f }), 18.0f * scale, 1.0f * scale, theme.Title);
        DrawTextEx(font, status, gamePtr->ScalePoint({ 470.0f, 435.0f }), 17.0f * scale, 1.0f * scale, lessonPassed ? theme.TextCorrect : theme.TextDefault);
    }

    DrawRectangleRounded(gamePtr->ScaleRect({ 430.0f, 475.0f, 420.0f, 36.0f }), 0.25f, 8, Fade(theme.Highlight, 0.16f));
    DrawTextEx(font, "ENTER / Click to Try Again", gamePtr->ScalePoint({ 455.0f, 485.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawRectangleRounded(gamePtr->ScaleRect({ 430.0f, 516.0f, 420.0f, 36.0f }), 0.25f, 8, Fade(theme.PanelBorder, 0.22f));
    DrawTextEx(font, "ESC / Click to return to Menu", gamePtr->ScalePoint({ 448.0f, 525.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
}
