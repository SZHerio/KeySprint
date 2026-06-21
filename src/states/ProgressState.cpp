#include "ProgressState.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <raylib.h>
#include "../core/Game.h"
#include "../core/LessonLibrary.h"
#include "MainMenuState.h"

namespace {
bool IsRu(Game* game) {
    return game->GetLanguage() == Language::Russian;
}

const char* LocalRank(const std::string& rank, bool ru) {
    if (!ru) return rank.c_str();
    if (rank == "Master") return u8"Мастер";
    if (rank == "Swift") return u8"Скоростной";
    if (rank == "Steady") return u8"Уверенный";
    return u8"Новичок";
}

const char* LocalDifficulty(const std::string& difficulty, bool ru) {
    if (!ru) return difficulty.c_str();
    if (difficulty == "Relaxed") return u8"Легкая";
    if (difficulty == "Strict") return u8"Строгая";
    return u8"Нормальная";
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

void DrawSceneText(Game* game, Font font, const char* text, Vector2 position, float fontSize, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, 1.0f * scale, color);
}

void DrawMetricCard(Game* game, Font font, const Theme& theme, Rectangle rect, const char* label, const char* value, Color accent) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.14f, 12, Fade(theme.PanelBorder, 0.18f));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.14f, 12, Fade(accent, 0.65f));
    DrawSceneText(game, font, label, { rect.x + 24.0f, rect.y + 22.0f }, 16.0f, theme.TextDefault);
    DrawSceneText(game, font, value, { rect.x + 24.0f, rect.y + 54.0f }, 30.0f, accent);
}
}

void ProgressState::Init(Game* game) {
    gamePtr = game;
}

void ProgressState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    }

    if (IsKeyPressed(KEY_R)) {
        gamePtr->GetProgress().Reset();
    }

    const Vector2 mouse = GetMousePosition();
    const bool resetHover = CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 830.0f, 94.0f, 250.0f, 32.0f }));
    if (resetHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        gamePtr->GetProgress().Reset();
    }
    SetMouseCursor(resetHover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void ProgressState::Update(float deltaTime) {
    animTime += deltaTime;
}

void ProgressState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    const Font font = gamePtr->GetFont();
    const ProgressManager& progress = gamePtr->GetProgress();
    const float pulse = (std::sin(animTime * 3.0f) + 1.0f) * 0.5f;

    DrawRectangleRounded(gamePtr->ScaleRect({ 70.0f, 54.0f, 1140.0f, 610.0f }), 0.04f, 16, Fade(theme.Panel, 0.80f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 70.0f, 54.0f, 1140.0f, 610.0f }), 0.04f, 16, Fade(theme.PanelBorder, 0.78f));
    DrawRectangleRounded(gamePtr->ScaleRect({ 94.0f, 82.0f, 1092.0f, 555.0f }), 0.04f, 16, Fade(theme.Highlight, 0.03f + pulse * 0.03f));

    DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"Статистика" : "Progress Stats", { 105.0f, 92.0f }, 38.0f, theme.Title);
    DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"ESC Меню | R Сброс прогресса" : "ESC Menu | R Reset Progress", { 800.0f, 105.0f }, 16.0f, theme.TextDefault);

    const std::string rank = progress.GetRankLabel();
    const std::string difficulty = progress.GetDifficultyLabel();
    DrawMetricCard(gamePtr, font, theme, { 110.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Ранг" : "Rank", LocalRank(rank, IsRu(gamePtr)), theme.Fingers.Index);
    DrawMetricCard(gamePtr, font, theme, { 365.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Рекорд WPM" : "Best WPM", TextFormat("%.0f", progress.GetBestWpm()), theme.Fingers.Middle);
    DrawMetricCard(gamePtr, font, theme, { 620.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Лучшая серия" : "Best Streak", TextFormat("%d", progress.GetBestStreak()), theme.Fingers.Ring);
    DrawMetricCard(gamePtr, font, theme, { 875.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Сложность" : "Difficulty", LocalDifficulty(difficulty, IsRu(gamePtr)), theme.Fingers.Thumb);

    DrawRectangleRounded(gamePtr->ScaleRect({ 110.0f, 325.0f, 500.0f, 250.0f }), 0.10f, 12, Fade(theme.PanelBorder, 0.16f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 110.0f, 325.0f, 500.0f, 250.0f }), 0.10f, 12, Fade(theme.PanelBorder, 0.56f));
    DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"Слабые клавиши" : "Weak Keys", { 138.0f, 352.0f }, 24.0f, theme.Title);
    const std::string weakFinger = progress.GetWeakFingerOfDay();
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"Палец дня: %s" : "Finger of the day: %s", LocalFinger(weakFinger, IsRu(gamePtr))), { 138.0f, 382.0f }, 14.0f, theme.TextDefault);

    std::vector<std::pair<std::string, int>> weak(progress.GetWeakKeys().begin(), progress.GetWeakKeys().end());
    std::sort(weak.begin(), weak.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    if (weak.empty()) {
        DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"Данных пока нет. Пройди урок или практику." : "No weak-key data yet. Complete lessons to collect it.", { 138.0f, 415.0f }, 17.0f, theme.TextDefault);
    } else {
        const int count = std::min(6, static_cast<int>(weak.size()));
        for (int i = 0; i < count; ++i) {
            const float y = 415.0f + i * 24.0f;
            const float barWidth = std::min(260.0f, weak[i].second * 28.0f);
            DrawSceneText(gamePtr, font, weak[i].first.c_str(), { 140.0f, y }, 18.0f, theme.TextCorrect);
            DrawRectangleRounded(gamePtr->ScaleRect({ 205.0f, y + 5.0f, barWidth, 12.0f }), 0.4f, 8, Fade(theme.TextError, 0.60f));
            DrawSceneText(gamePtr, font, TextFormat("%d misses", weak[i].second), { 485.0f, y }, 15.0f, theme.TextDefault);
        }
    }

    DrawRectangleRounded(gamePtr->ScaleRect({ 650.0f, 325.0f, 460.0f, 250.0f }), 0.10f, 12, Fade(theme.PanelBorder, 0.16f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 650.0f, 325.0f, 460.0f, 250.0f }), 0.10f, 12, Fade(theme.PanelBorder, 0.56f));
    DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"Миссии" : "Missions", { 680.0f, 352.0f }, 24.0f, theme.Title);
    const int englishCount = static_cast<int>(LessonLibrary::GetLessons(Language::English).size());
    const int russianCount = static_cast<int>(LessonLibrary::GetLessons(Language::Russian).size());
    const int englishUnlocked = std::min(progress.GetUnlockedLesson(Language::English) + 1, englishCount);
    const int russianUnlocked = std::min(progress.GetUnlockedLesson(Language::Russian) + 1, russianCount);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"День: пройти испытание выше %.0f%%" : "Daily: finish Daily Challenge above %.0f%%", progress.GetUnlockAccuracyThreshold()), { 680.0f, 405.0f }, 16.0f, theme.TextDefault);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"Сессия: побить серию %d" : "Session: beat streak %d", std::max(15, progress.GetBestStreak() + 5)), { 680.0f, 435.0f }, 16.0f, theme.TextDefault);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"Тренер: 2 минуты на клавишу %s" : "Coach: drill weak key %s for 2 minutes", progress.GetWeakKeyOfDay().c_str()), { 680.0f, 465.0f }, 16.0f, theme.TextDefault);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"Курс: EN %d/%d | RU %d/%d" : "Course: EN %d/%d | RU %d/%d", englishUnlocked, englishCount, russianUnlocked, russianCount), { 680.0f, 505.0f }, 17.0f, theme.TextCorrect);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"Порог открытия: %.0f%% точности" : "Unlock threshold: %.0f%% accuracy", progress.GetUnlockAccuracyThreshold()), { 680.0f, 535.0f }, 15.0f, theme.TextDefault);
}
