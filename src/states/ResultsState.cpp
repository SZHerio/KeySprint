#include "ResultsState.h"
#include "../core/Game.h"
#include <algorithm>
#include <vector>
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
    int bestStreak,
    const std::map<std::string, int>& mistakes
) : wpm(finalWPM),
    accuracy(finalAccuracy),
    retryMode(mode),
    language(language),
    lessonId(lessonId),
    lessonTitle(lessonTitle),
    bestStreak(bestStreak),
    mistakes(mistakes) {}

void ResultsState::Init(Game* game) {
    gamePtr = game;
    if (!recorded) {
        gamePtr->GetProgress().RecordResult(language, lessonId, wpm, accuracy, bestStreak, mistakes);
        recorded = true;
    }
}

void ResultsState::HandleInput() {
    const Vector2 mouse = GetMousePosition();
    const bool retryClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 500.0f, 420.0f, 36.0f }));
    const bool menuClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 545.0f, 420.0f, 36.0f }));

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

    const Rectangle card = gamePtr->ScaleRect({ 305.0f, 88.0f, 670.0f, 540.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(theme.PanelBorder, 0.80f));

    const float threshold = gamePtr->GetProgress().GetUnlockAccuracyThreshold();
    const bool lessonPassed = retryMode == TypingMode::Tutorial && accuracy >= threshold;
    DrawTextEx(font, lessonPassed ? "Lesson Complete" : "Session Review", gamePtr->ScalePoint({ 365.0f, 130.0f }), 36.0f * scale, 1.0f * scale, theme.Title);
    
    DrawTextEx(font, TextFormat("WPM %.0f", wpm), gamePtr->ScalePoint({ 365.0f, 210.0f }), 26.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, TextFormat("ACC %.0f%%", accuracy), gamePtr->ScalePoint({ 530.0f, 210.0f }), 26.0f * scale, 1.0f * scale, theme.TextCorrect);
    DrawTextEx(font, TextFormat("STREAK %d", bestStreak), gamePtr->ScalePoint({ 705.0f, 210.0f }), 26.0f * scale, 1.0f * scale, theme.Fingers.Index);
    DrawTextEx(font, TextFormat("Rank: %s | Difficulty: %s", gamePtr->GetProgress().GetRankLabel().c_str(), gamePtr->GetProgress().GetDifficultyLabel().c_str()), gamePtr->ScalePoint({ 365.0f, 258.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);

    if (retryMode == TypingMode::Tutorial) {
        const char* status = lessonPassed ? "Next lesson unlocked" : "Accuracy below difficulty threshold";
        DrawTextEx(font, lessonTitle.c_str(), gamePtr->ScalePoint({ 365.0f, 305.0f }), 18.0f * scale, 1.0f * scale, theme.Title);
        DrawTextEx(font, TextFormat("%s (need %.0f%%)", status, threshold), gamePtr->ScalePoint({ 365.0f, 332.0f }), 16.0f * scale, 1.0f * scale, lessonPassed ? theme.TextCorrect : theme.TextDefault);
    }

    std::vector<std::pair<std::string, int>> weak(mistakes.begin(), mistakes.end());
    std::sort(weak.begin(), weak.end(), [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });
    const std::string weakKey = weak.empty() ? gamePtr->GetProgress().GetWeakKeyOfDay() : weak.front().first;
    const std::string weakFinger = gamePtr->GetProgress().GetWeakFingerOfDay();
    DrawRectangleRounded(gamePtr->ScaleRect({ 350.0f, 370.0f, 580.0f, 86.0f }), 0.18f, 10, Fade(theme.PanelBorder, 0.18f));
    DrawTextEx(font, "Coach note", gamePtr->ScalePoint({ 380.0f, 386.0f }), 18.0f * scale, 1.0f * scale, theme.Title);
    DrawTextEx(font, TextFormat("Weak key: %s | Finger focus: %s", weakKey.c_str(), weakFinger.c_str()), gamePtr->ScalePoint({ 380.0f, 416.0f }), 16.0f * scale, 1.0f * scale, theme.TextDefault);
    const char* tip = accuracy < threshold ? "Mission: replay this lesson slower and keep accuracy above threshold." : "Mission: repeat once and beat your streak without losing accuracy.";
    DrawTextEx(font, tip, gamePtr->ScalePoint({ 380.0f, 438.0f }), 14.0f * scale, 1.0f * scale, theme.TextDefault);

    DrawRectangleRounded(gamePtr->ScaleRect({ 430.0f, 500.0f, 420.0f, 36.0f }), 0.25f, 8, Fade(theme.Highlight, 0.16f));
    DrawTextEx(font, "ENTER / Click to Try Again", gamePtr->ScalePoint({ 455.0f, 510.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
    DrawRectangleRounded(gamePtr->ScaleRect({ 430.0f, 545.0f, 420.0f, 36.0f }), 0.25f, 8, Fade(theme.PanelBorder, 0.22f));
    DrawTextEx(font, "ESC / Click to return to Menu", gamePtr->ScalePoint({ 448.0f, 554.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
}
