#include "ResultsState.h"
#include "../core/Game.h"
#include "../core/LessonLibrary.h"
#include "../core/ModeVisual.h"
#include <algorithm>
#include <string>
#include <vector>
#include <raylib.h>
#include "MainMenuState.h"
#include "TypingState.h"

namespace {
bool IsRu(Language language) {
    return language == Language::Russian;
}

const char* LocalDifficulty(const std::string& difficulty, bool ru) {
    if (!ru) return difficulty.c_str();
    if (difficulty == "Relaxed") return u8"Легкая";
    if (difficulty == "Strict") return u8"Строгая";
    return u8"Нормальная";
}

const char* LocalRank(const std::string& rank, bool ru) {
    if (!ru) return rank.c_str();
    if (rank == "Master") return u8"Мастер";
    if (rank == "Swift") return u8"Скоростной";
    if (rank == "Steady") return u8"Уверенный";
    return u8"Новичок";
}

const char* LocalFinger(const std::string& finger, bool ru) {
    if (!ru) return finger.c_str();
    if (finger == "Left pinky") return u8"левый мизинец";
    if (finger == "Left ring") return u8"левый безымянный";
    if (finger == "Left middle") return u8"левый средний";
    if (finger == "Left index") return u8"левый указательный";
    if (finger == "Right index") return u8"правый указательный";
    if (finger == "Right middle") return u8"правый средний";
    if (finger == "Right ring") return u8"правый безымянный";
    if (finger == "Right pinky") return u8"правый мизинец";
    if (finger == "Thumb") return u8"большой палец";
    return u8"нет данных";
}
void DrawFittedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, Color color) {
    float adjustedSize = fontSize;
    while (adjustedSize > 11.0f && MeasureTextEx(font, text, adjustedSize, 0.0f).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), adjustedSize * scale, 0.0f, color);
}
}

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
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 500.0f, 420.0f, 40.0f }));
    const bool menuClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 548.0f, 420.0f, 40.0f }));
    const bool buttonHover =
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 500.0f, 420.0f, 40.0f })) ||
        CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 430.0f, 548.0f, 420.0f, 40.0f }));
    SetMouseCursor(buttonHover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

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
    const ModeVisualStyle modeStyle = GetModeVisualStyle(retryMode);
    Font font = gamePtr->GetUiFont();
    const float scale = gamePtr->GetUiScale();
    const Vector2 mouse = GetMousePosition();

    const Rectangle card = gamePtr->ScaleRect({ 305.0f, 88.0f, 670.0f, 540.0f });
    DrawRectangleRounded(card, 0.10f, 12, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(card, 0.10f, 12, Fade(modeStyle.Accent, 0.54f));
    DrawRectangleRounded(gamePtr->ScaleRect({ 325.0f, 108.0f, 7.0f, 500.0f }), 0.80f, 8, Fade(modeStyle.Accent, 0.34f));

    const float threshold = gamePtr->GetProgress().GetUnlockAccuracyThreshold();
    const bool lessonPassed = retryMode == TypingMode::Tutorial && accuracy >= threshold;
    DrawTextEx(font, lessonPassed ? (IsRu(language) ? u8"Урок пройден" : "Lesson Complete") : (IsRu(language) ? u8"Разбор сессии" : "Session Review"), gamePtr->ScalePoint({ 365.0f, 130.0f }), 36.0f * scale, 1.0f * scale, theme.Title);
    DrawRectangleRounded(gamePtr->ScaleRect({ 735.0f, 132.0f, 178.0f, 34.0f }), 0.36f, 10, Fade(modeStyle.Accent, 0.15f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 735.0f, 132.0f, 178.0f, 34.0f }), 0.36f, 10, Fade(modeStyle.Accent, 0.40f));
    DrawCircleV(gamePtr->ScalePoint({ 754.0f, 149.0f }), 10.0f * scale, Fade(modeStyle.Accent, 0.76f));
    DrawFittedText(gamePtr, font, modeStyle.Mark, { 748.0f, 143.0f }, 14.0f, 10.0f, theme.Background);
    DrawFittedText(gamePtr, font, IsRu(language) ? modeStyle.LabelRu : modeStyle.LabelEn, { 772.0f, 142.0f }, 128.0f, 14.0f, theme.TextDefault);
    
    DrawTextEx(font, TextFormat("WPM %.0f", wpm), gamePtr->ScalePoint({ 365.0f, 210.0f }), 26.0f * scale, 1.0f * scale, modeStyle.Accent);
    DrawTextEx(font, TextFormat("ACC %.0f%%", accuracy), gamePtr->ScalePoint({ 530.0f, 210.0f }), 26.0f * scale, 1.0f * scale, modeStyle.Accent);
    DrawTextEx(font, TextFormat(IsRu(language) ? u8"СЕРИЯ %d" : "STREAK %d", bestStreak), gamePtr->ScalePoint({ 705.0f, 210.0f }), 26.0f * scale, 1.0f * scale, Fade(modeStyle.Accent, 0.90f));
    const std::string rank = gamePtr->GetProgress().GetRankLabel();
    const std::string difficulty = gamePtr->GetProgress().GetDifficultyLabel();
    DrawTextEx(font, TextFormat(IsRu(language) ? u8"Ранг: %s | Сложность: %s" : "Rank: %s | Difficulty: %s", LocalRank(rank, IsRu(language)), LocalDifficulty(difficulty, IsRu(language))), gamePtr->ScalePoint({ 365.0f, 258.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);

    if (retryMode == TypingMode::Tutorial) {
        const char* status = lessonPassed ? (IsRu(language) ? u8"Следующий урок открыт" : "Next lesson unlocked") : (IsRu(language) ? u8"Точность ниже порога сложности" : "Accuracy below difficulty threshold");
        DrawTextEx(font, lessonTitle.c_str(), gamePtr->ScalePoint({ 365.0f, 305.0f }), 18.0f * scale, 1.0f * scale, theme.Title);
        DrawTextEx(font, TextFormat("%s (need %.0f%%)", status, threshold), gamePtr->ScalePoint({ 365.0f, 332.0f }), 16.0f * scale, 1.0f * scale, lessonPassed ? theme.TextCorrect : theme.TextDefault);
    }

    std::vector<std::pair<std::string, int>> weak(mistakes.begin(), mistakes.end());
    std::sort(weak.begin(), weak.end(), [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });
    const std::string weakKey = weak.empty()
        ? gamePtr->GetProgress().GetWeakKeyOfDay(language)
        : weak.front().first;
    const std::string weakKeyLabel = LessonLibrary::FormatKeyLabel(weakKey, language);
    const std::string weakFinger = gamePtr->GetProgress().GetWeakFingerOfDay(language);
    DrawRectangleRounded(gamePtr->ScaleRect({ 350.0f, 370.0f, 580.0f, 86.0f }), 0.18f, 10, Fade(modeStyle.Accent, 0.10f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 350.0f, 370.0f, 580.0f, 86.0f }), 0.18f, 10, Fade(modeStyle.Accent, 0.28f));
    DrawTextEx(font, IsRu(language) ? u8"Заметка тренера" : "Coach note", gamePtr->ScalePoint({ 380.0f, 386.0f }), 18.0f * scale, 1.0f * scale, theme.Title);
    const std::string weakKeyLine = IsRu(language)
        ? u8"Слабая клавиша: «" + weakKeyLabel + u8"» | Фокус: " + LocalFinger(weakFinger, true)
        : "Weak key: " + weakKeyLabel + " | Finger focus: " + LocalFinger(weakFinger, false);
    DrawFittedText(gamePtr, font, weakKeyLine.c_str(), { 380.0f, 416.0f }, 520.0f, 16.0f, theme.TextDefault);
    const char* tip = accuracy < threshold ? (IsRu(language) ? u8"Миссия: повтори медленнее и держи точность выше порога." : "Mission: replay this lesson slower and keep accuracy above threshold.") : (IsRu(language) ? u8"Миссия: повтори и побей серию без потери точности." : "Mission: repeat once and beat your streak without losing accuracy.");
    DrawFittedText(gamePtr, font, tip, { 380.0f, 438.0f }, 520.0f, 14.0f, theme.TextDefault);

    const Rectangle retryRect = { 430.0f, 500.0f, 420.0f, 40.0f };
    const Rectangle menuRect = { 430.0f, 548.0f, 420.0f, 40.0f };
    const bool retryHover = CheckCollisionPointRec(mouse, gamePtr->ScaleRect(retryRect));
    const bool menuHover = CheckCollisionPointRec(mouse, gamePtr->ScaleRect(menuRect));

    DrawRectangleRounded(gamePtr->ScaleRect(retryRect), 0.25f, 8, Fade(modeStyle.Accent, retryHover ? 0.24f : 0.14f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect(retryRect), 0.25f, 8, Fade(modeStyle.Accent, retryHover ? 0.62f : 0.28f));
    DrawCircleV(gamePtr->ScalePoint({ retryRect.x + 18.0f, retryRect.y + 20.0f }), (retryHover ? 6.0f : 4.0f) * scale, retryHover ? modeStyle.Accent : Fade(theme.TextDefault, 0.45f));
    DrawTextEx(font, IsRu(language) ? u8"ENTER / Клик: повторить" : "ENTER / Click to Try Again", gamePtr->ScalePoint({ 455.0f, 510.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);

    DrawRectangleRounded(gamePtr->ScaleRect(menuRect), 0.25f, 8, Fade(menuHover ? modeStyle.Accent : theme.PanelBorder, menuHover ? 0.20f : 0.18f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect(menuRect), 0.25f, 8, Fade(menuHover ? modeStyle.Accent : theme.PanelBorder, menuHover ? 0.56f : 0.25f));
    DrawCircleV(gamePtr->ScalePoint({ menuRect.x + 18.0f, menuRect.y + 20.0f }), (menuHover ? 6.0f : 4.0f) * scale, menuHover ? modeStyle.Accent : Fade(theme.TextDefault, 0.45f));
    DrawTextEx(font, IsRu(language) ? u8"ESC / Клик: в меню" : "ESC / Click to return to Menu", gamePtr->ScalePoint({ 455.0f, 558.0f }), 18.0f * scale, 1.0f * scale, theme.TextDefault);
}
