#include "LessonSelectState.h"

#include <algorithm>
#include <cmath>
#include <raylib.h>
#include "../core/Game.h"
#include "../core/LessonLibrary.h"
#include "../core/ModeVisual.h"
#include "../core/UiMotion.h"
#include "MainMenuState.h"
#include "TypingState.h"

namespace {
void DrawSceneText(Game* game, Font font, const char* text, Vector2 position, float fontSize, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, 1.0f * scale, color);
}

void DrawFittedSceneText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, Color color) {
    float adjustedSize = fontSize;
    while (adjustedSize > 10.0f && MeasureTextEx(font, text, adjustedSize, 0.0f).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), adjustedSize * scale, 0.0f, color);
}

bool IsRu(Language language) {
    return language == Language::Russian;
}

void BeginScissorScene(Game* game, Rectangle rect) {
    const Rectangle scaled = game->ScaleRect(rect);
    BeginScissorMode(
        static_cast<int>(scaled.x),
        static_cast<int>(scaled.y),
        static_cast<int>(scaled.width),
        static_cast<int>(scaled.height)
    );
}
}

void LessonSelectState::Init(Game* game) {
    gamePtr = game;
    language = gamePtr->GetTypingLanguage();
    selectedLesson = gamePtr->GetProgress().GetCurrentLesson(language, GetLessonCount());
    scrollOffset = 0.0f;
    scrollTarget = 0.0f;
    EnsureSelectedVisible();
    scrollOffset = scrollTarget;

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

Rectangle LessonSelectState::GetLessonViewport() const {
    return { 75.0f, 146.0f, 1130.0f, 494.0f };
}

Rectangle LessonSelectState::GetCardBaseRect(int index) const {
    const int columns = 2;
    const float cardWidth = 520.0f;
    const float cardHeight = 70.0f;
    const float gapX = 40.0f;
    const float gapY = 8.0f;
    const float startX = 90.0f;
    const float startY = GetLessonViewport().y;

    const int col = index % columns;
    const int row = index / columns;
    return {
        startX + col * (cardWidth + gapX),
        startY + row * (cardHeight + gapY),
        cardWidth,
        cardHeight
    };
}

Rectangle LessonSelectState::GetCardRect(int index) const {
    Rectangle rect = GetCardBaseRect(index);
    rect.y -= scrollOffset;
    return rect;
}

float LessonSelectState::GetMaxScrollOffset() const {
    const int columns = 2;
    const float cardHeight = 70.0f;
    const float gapY = 8.0f;
    const int count = GetLessonCount();
    const int rows = (count + columns - 1) / columns;
    if (rows <= 0) {
        return 0.0f;
    }

    const float contentHeight = static_cast<float>(rows) * cardHeight + static_cast<float>(rows - 1) * gapY;
    return std::max(0.0f, contentHeight - GetLessonViewport().height);
}

void LessonSelectState::EnsureSelectedVisible() {
    const Rectangle viewport = GetLessonViewport();
    const Rectangle base = GetCardBaseRect(selectedLesson);
    const float margin = 10.0f;

    if (base.y - scrollTarget < viewport.y + margin) {
        scrollTarget = base.y - viewport.y - margin;
    } else if (base.y + base.height - scrollTarget > viewport.y + viewport.height - margin) {
        scrollTarget = base.y + base.height - viewport.y - viewport.height + margin;
    }

    scrollTarget = std::clamp(scrollTarget, 0.0f, GetMaxScrollOffset());
}

void LessonSelectState::StartSelectedLesson() {
    if (IsLessonUnlocked(selectedLesson)) {
        gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Tutorial, selectedLesson));
    }
}

void LessonSelectState::HandleInput() {
    const int count = GetLessonCount();
    bool selectionChanged = false;

    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
        return;
    }

    const float wheel = GetMouseWheelMove();
    if (std::abs(wheel) > 0.01f) {
        scrollTarget = std::clamp(scrollTarget - wheel * 56.0f, 0.0f, GetMaxScrollOffset());
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        selectedLesson = std::min(count - 1, selectedLesson + 1);
        selectionChanged = true;
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        selectedLesson = std::max(0, selectedLesson - 1);
        selectionChanged = true;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedLesson = std::min(count - 1, selectedLesson + 2);
        selectionChanged = true;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedLesson = std::max(0, selectedLesson - 2);
        selectionChanged = true;
    } else if (IsKeyPressed(KEY_ENTER)) {
        StartSelectedLesson();
    }

    const Vector2 mouse = GetMousePosition();
    bool hover = false;
    if (CheckCollisionPointRec(mouse, gamePtr->ScaleRect(GetLessonViewport()))) {
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
    }
    if (selectionChanged) {
        EnsureSelectedVisible();
    }
    SetMouseCursor(hover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void LessonSelectState::Update(float deltaTime) {
    animTime += deltaTime;
    scrollTarget = std::clamp(scrollTarget, 0.0f, GetMaxScrollOffset());
    scrollOffset += (scrollTarget - scrollOffset) * std::min(1.0f, deltaTime * SelectionMoveSpeed);

    const Rectangle target = GetCardRect(selectedLesson);
    cursorX += (target.x - cursorX) * std::min(1.0f, deltaTime * SelectionMoveSpeed);
    cursorY += (target.y - cursorY) * std::min(1.0f, deltaTime * SelectionMoveSpeed);
}

void LessonSelectState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    const ModeVisualStyle modeStyle = GetModeVisualStyle(TypingMode::Tutorial);
    const Font font = gamePtr->GetUiFont();
    const Language uiLanguage = gamePtr->GetLanguage();
    const float scale = gamePtr->GetUiScale();
    const auto& lessons = LessonLibrary::GetLessons(language);
    const int unlocked = gamePtr->GetProgress().GetUnlockedLesson(language);
    const float pulse = (std::sin(animTime * 4.0f) + 1.0f) * 0.5f;
    const Rectangle viewport = GetLessonViewport();

    DrawRectangleRounded(gamePtr->ScaleRect({ 55.0f, 42.0f, 1170.0f, 635.0f }), 0.04f, 16, Fade(theme.Panel, 0.78f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 55.0f, 42.0f, 1170.0f, 635.0f }), 0.04f, 16, Fade(modeStyle.Accent, 0.44f));
    DrawRectangleRounded(gamePtr->ScaleRect({ 72.0f, 58.0f, 7.0f, 602.0f }), 0.80f, 8, Fade(modeStyle.Accent, 0.34f));

    DrawSceneText(gamePtr, font, IsRu(uiLanguage) ? u8"Выбор урока" : "Lesson Select", { 90.0f, 72.0f }, 38.0f, theme.Title);
    const int visibleUnlocked = std::min(unlocked + 1, static_cast<int>(lessons.size()));
    DrawSceneText(gamePtr, font, TextFormat(IsRu(uiLanguage) ? u8"%s карта курса | открыто %d/%d" : "%s course map | %d/%d open", LessonLibrary::GetLanguageLabel(language).c_str(), visibleUnlocked, static_cast<int>(lessons.size())), { 90.0f, 118.0f }, 18.0f, theme.TextDefault);
    DrawSceneText(gamePtr, font, IsRu(uiLanguage) ? u8"ESC Меню | Стрелки/WASD | Enter Старт" : "ESC Menu | Arrows/WASD Move | Enter Start", { 720.0f, 92.0f }, 16.0f, theme.TextDefault);

    BeginScissorScene(gamePtr, viewport);
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
        DrawLineEx(start, end, 2.0f * scale, Fade(i < unlocked ? modeStyle.Accent : theme.PanelBorder, i < unlocked ? 0.50f : 0.16f));
        DrawCircleV(start, 3.0f * scale, Fade(i < unlocked ? modeStyle.Accent : theme.PanelBorder, 0.55f));
        DrawCircleV(end, 3.0f * scale, Fade(i < unlocked ? modeStyle.Accent : theme.PanelBorder, 0.55f));
    }

    DrawRectangleRounded(gamePtr->ScaleRect({ cursorX - 5.0f, cursorY - 5.0f, 530.0f, 80.0f }), 0.12f, 12, Fade(modeStyle.Accent, 0.16f + pulse * 0.06f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ cursorX - 5.0f, cursorY - 5.0f, 530.0f, 80.0f }), 0.12f, 12, Fade(modeStyle.Accent, 0.58f));

    for (int i = 0; i < static_cast<int>(lessons.size()); ++i) {
        const Lesson& lesson = lessons[i];
        const Rectangle rect = GetCardRect(i);
        const bool unlockedLesson = IsLessonUnlocked(i);
        const bool selected = selectedLesson == i;

        Color fill = Fade(theme.PanelBorder, unlockedLesson ? 0.20f : 0.08f);
        if (selected) {
            fill = Fade(modeStyle.Accent, unlockedLesson ? 0.20f : 0.10f);
        }

        DrawRectangleRounded(gamePtr->ScaleRect(rect), 0.12f, 12, fill);
        DrawRectangleRoundedLines(gamePtr->ScaleRect(rect), 0.12f, 12, Fade(unlockedLesson ? modeStyle.Accent : theme.TextDefault, unlockedLesson ? 0.36f : 0.25f));

        DrawSceneText(gamePtr, font, TextFormat("%02d", i + 1), { rect.x + 22.0f, rect.y + 13.0f }, 26.0f, unlockedLesson ? modeStyle.Accent : theme.TextDefault);
        DrawSceneText(gamePtr, font, lesson.title.c_str(), { rect.x + 88.0f, rect.y + 12.0f }, 20.0f, unlockedLesson ? theme.TextCorrect : Fade(theme.TextDefault, 0.55f));
        DrawFittedSceneText(gamePtr, font, lesson.description.c_str(), { rect.x + 88.0f, rect.y + 42.0f }, 320.0f, 13.0f, unlockedLesson ? theme.TextDefault : Fade(theme.TextDefault, 0.42f));

        const char* status = unlockedLesson ? (i < unlocked ? (IsRu(uiLanguage) ? u8"ГОТОВО" : "DONE") : (IsRu(uiLanguage) ? u8"ОТКРЫТ" : "OPEN")) : (IsRu(uiLanguage) ? u8"ЗАКРЫТ" : "LOCKED");
        const Color statusColor = unlockedLesson ? theme.TextCorrect : theme.TextError;
        DrawSceneText(gamePtr, font, status, { rect.x + rect.width - 92.0f, rect.y + 17.0f }, 14.0f, statusColor);
    }
    EndScissorMode();

    const float maxScroll = GetMaxScrollOffset();
    if (maxScroll > 0.5f) {
        const Rectangle track = { viewport.x + viewport.width + 10.0f, viewport.y + 8.0f, 5.0f, viewport.height - 16.0f };
        const float thumbHeight = std::max(46.0f, track.height * (viewport.height / (viewport.height + maxScroll)));
        const float thumbY = track.y + (track.height - thumbHeight) * (scrollOffset / maxScroll);
        DrawRectangleRounded(gamePtr->ScaleRect(track), 0.80f, 8, Fade(theme.PanelBorder, 0.22f));
        DrawRectangleRounded(gamePtr->ScaleRect({ track.x, thumbY, track.width, thumbHeight }), 0.80f, 8, Fade(modeStyle.Accent, 0.72f));
    }
}
