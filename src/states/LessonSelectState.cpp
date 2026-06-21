#include "LessonSelectState.h"

#include <algorithm>
#include <cmath>
#include <raylib.h>
#include "../core/Game.h"
#include "../core/LessonLibrary.h"
#include "MainMenuState.h"
#include "TypingState.h"

namespace {
void DrawSceneText(Game* game, Font font, const char* text, Vector2 position, float fontSize, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, 1.0f * scale, color);
}

bool IsRu(Language language) {
    return language == Language::Russian;
}
}

void LessonSelectState::Init(Game* game) {
    gamePtr = game;
    language = gamePtr->GetLanguage();
    selectedLesson = gamePtr->GetProgress().GetCurrentLesson(language, GetLessonCount());

    const Rectangle rect = GetCardRect(selectedLesson);
    cursorX = rect.x;
    cursorY = rect.y;
}

int LessonSelectState::GetLessonCount() const {
    return static_cast<int>(LessonLibrary::GetLessons(language).size());
}

bool LessonSelectState::IsLessonUnlocked(int lessonId) const {
    return lessonId <= gamePtr->GetProgress().GetUnlockedLesson(language);
}

Rectangle LessonSelectState::GetCardRect(int index) const {
    const int columns = 2;
    const float cardWidth = 520.0f;
    const float cardHeight = 86.0f;
    const float gapX = 40.0f;
    const float gapY = 18.0f;
    const float startX = 90.0f;
    const float startY = 160.0f;

    const int col = index % columns;
    const int row = index / columns;
    return {
        startX + col * (cardWidth + gapX),
        startY + row * (cardHeight + gapY),
        cardWidth,
        cardHeight
    };
}

void LessonSelectState::StartSelectedLesson() {
    if (IsLessonUnlocked(selectedLesson)) {
        gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Tutorial, selectedLesson));
    }
}

void LessonSelectState::HandleInput() {
    const int count = GetLessonCount();

    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
        return;
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        selectedLesson = std::min(count - 1, selectedLesson + 1);
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        selectedLesson = std::max(0, selectedLesson - 1);
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedLesson = std::min(count - 1, selectedLesson + 2);
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedLesson = std::max(0, selectedLesson - 2);
    } else if (IsKeyPressed(KEY_ENTER)) {
        StartSelectedLesson();
    }

    const Vector2 mouse = GetMousePosition();
    bool hover = false;
    for (int i = 0; i < count; ++i) {
        if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect(GetCardRect(i)))) {
            selectedLesson = i;
            hover = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                StartSelectedLesson();
            }
            break;
        }
    }
    SetMouseCursor(hover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void LessonSelectState::Update(float deltaTime) {
    animTime += deltaTime;
    const Rectangle target = GetCardRect(selectedLesson);
    cursorX += (target.x - cursorX) * std::min(1.0f, deltaTime * 14.0f);
    cursorY += (target.y - cursorY) * std::min(1.0f, deltaTime * 14.0f);
}

void LessonSelectState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    const Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();
    const auto& lessons = LessonLibrary::GetLessons(language);
    const int unlocked = gamePtr->GetProgress().GetUnlockedLesson(language);
    const float pulse = (std::sin(animTime * 4.0f) + 1.0f) * 0.5f;

    DrawRectangleRounded(gamePtr->ScaleRect({ 55.0f, 42.0f, 1170.0f, 635.0f }), 0.04f, 16, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 55.0f, 42.0f, 1170.0f, 635.0f }), 0.04f, 16, Fade(theme.PanelBorder, 0.80f));

    DrawSceneText(gamePtr, font, IsRu(language) ? u8"Выбор урока" : "Lesson Select", { 90.0f, 72.0f }, 38.0f, theme.Title);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(language) ? u8"%s карта курса | открыто до урока %d" : "%s course map | unlocked through lesson %d", LessonLibrary::GetLanguageLabel(language).c_str(), unlocked + 1), { 90.0f, 118.0f }, 18.0f, theme.TextDefault);
    DrawSceneText(gamePtr, font, IsRu(language) ? u8"ESC Меню | Стрелки/WASD | Enter Старт" : "ESC Menu | Arrows/WASD Move | Enter Start", { 720.0f, 92.0f }, 16.0f, theme.TextDefault);

    for (int i = 0; i + 1 < static_cast<int>(lessons.size()); ++i) {
        const Rectangle from = GetCardRect(i);
        const Rectangle to = GetCardRect(i + 1);
        const bool sameRow = static_cast<int>(from.y) == static_cast<int>(to.y);
        const Vector2 start = sameRow
            ? gamePtr->ScalePoint({ from.x + from.width + 10.0f, from.y + from.height * 0.5f })
            : gamePtr->ScalePoint({ from.x + from.width - 28.0f, from.y + from.height + 8.0f });
        const Vector2 end = sameRow
            ? gamePtr->ScalePoint({ to.x - 10.0f, to.y + to.height * 0.5f })
            : gamePtr->ScalePoint({ to.x + 28.0f, to.y - 8.0f });
        DrawLineEx(start, end, 2.0f * scale, Fade(i < unlocked ? theme.Highlight : theme.PanelBorder, i < unlocked ? 0.42f : 0.16f));
        DrawCircleV(start, 3.0f * scale, Fade(i < unlocked ? theme.Highlight : theme.PanelBorder, 0.45f));
        DrawCircleV(end, 3.0f * scale, Fade(i < unlocked ? theme.Highlight : theme.PanelBorder, 0.45f));
    }

    DrawRectangleRounded(gamePtr->ScaleRect({ cursorX - 5.0f, cursorY - 5.0f, 530.0f, 96.0f }), 0.12f, 12, Fade(theme.Highlight, 0.16f + pulse * 0.06f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ cursorX - 5.0f, cursorY - 5.0f, 530.0f, 96.0f }), 0.12f, 12, Fade(theme.Highlight, 0.55f));

    for (int i = 0; i < static_cast<int>(lessons.size()); ++i) {
        const Lesson& lesson = lessons[i];
        const Rectangle rect = GetCardRect(i);
        const bool unlockedLesson = IsLessonUnlocked(i);
        const bool selected = selectedLesson == i;

        Color fill = Fade(theme.PanelBorder, unlockedLesson ? 0.20f : 0.08f);
        if (selected) {
            fill = Fade(theme.Highlight, unlockedLesson ? 0.20f : 0.10f);
        }

        DrawRectangleRounded(gamePtr->ScaleRect(rect), 0.12f, 12, fill);
        DrawRectangleRoundedLines(gamePtr->ScaleRect(rect), 0.12f, 12, Fade(unlockedLesson ? theme.PanelBorder : theme.TextDefault, unlockedLesson ? 0.65f : 0.25f));

        DrawSceneText(gamePtr, font, TextFormat("%02d", i + 1), { rect.x + 22.0f, rect.y + 18.0f }, 28.0f, unlockedLesson ? theme.Highlight : theme.TextDefault);
        DrawSceneText(gamePtr, font, lesson.title.c_str(), { rect.x + 88.0f, rect.y + 16.0f }, 22.0f, unlockedLesson ? theme.TextCorrect : Fade(theme.TextDefault, 0.55f));
        DrawSceneText(gamePtr, font, lesson.description.c_str(), { rect.x + 88.0f, rect.y + 50.0f }, 14.0f, unlockedLesson ? theme.TextDefault : Fade(theme.TextDefault, 0.42f));

        const char* status = unlockedLesson ? (i < unlocked ? (IsRu(language) ? u8"ГОТОВО" : "DONE") : (IsRu(language) ? u8"ОТКРЫТ" : "OPEN")) : (IsRu(language) ? u8"ЗАКРЫТ" : "LOCKED");
        const Color statusColor = unlockedLesson ? theme.TextCorrect : theme.TextError;
        DrawSceneText(gamePtr, font, status, { rect.x + rect.width - 92.0f, rect.y + 20.0f }, 15.0f, statusColor);
    }
}
