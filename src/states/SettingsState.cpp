#include "SettingsState.h"
#include "../core/Game.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include "../core/AudioManager.h"
#include "MainMenuState.h"

namespace {
Rectangle SettingsRow(int index) {
    return { 286.0f, 150.0f + index * 50.0f, 708.0f, 44.0f };
}

Rectangle ControlRect(Rectangle row) {
    return { row.x + 370.0f, row.y + 7.0f, row.width - 390.0f, 30.0f };
}

Rectangle KeyRect(Rectangle row) {
    return { row.x + 18.0f, row.y + 9.0f, 42.0f, 26.0f };
}

Rectangle SelectorLeft(Rectangle control) {
    return { control.x, control.y, 34.0f, control.height };
}

Rectangle SelectorRight(Rectangle control) {
    return { control.x + control.width - 34.0f, control.y, 34.0f, control.height };
}

Rectangle ApplyButtonRect() {
    return { 782.0f, 616.0f, 212.0f, 40.0f };
}

bool IsRu(Game* game) {
    return game->GetLanguage() == Language::Russian;
}

bool Hit(Game* game, Rectangle rect, Vector2 mouse) {
    return CheckCollisionPointRec(mouse, game->ScaleRect(rect));
}

float SliderValueAt(Game* game, Rectangle rect, Vector2 mouse) {
    const Rectangle scaled = game->ScaleRect(rect);
    if (scaled.width <= 0.0f) {
        return 0.0f;
    }
    return std::clamp((mouse.x - scaled.x) / scaled.width, 0.0f, 1.0f);
}

int SegmentIndexAt(Game* game, Rectangle rect, int count, Vector2 mouse) {
    if (count <= 0 || !Hit(game, rect, mouse)) {
        return -1;
    }

    const Rectangle scaled = game->ScaleRect(rect);
    const float segmentWidth = scaled.width / static_cast<float>(count);
    return std::clamp(static_cast<int>((mouse.x - scaled.x) / segmentWidth), 0, count - 1);
}

Difficulty DifficultyFromIndex(int index) {
    if (index == 0) return Difficulty::Relaxed;
    if (index == 2) return Difficulty::Strict;
    return Difficulty::Normal;
}

int DifficultyIndex(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Relaxed: return 0;
        case Difficulty::Strict: return 2;
        case Difficulty::Normal:
        default: return 1;
    }
}

int NextWrappedIndex(int index, int offset, int count) {
    if (count <= 0) {
        return 0;
    }
    const int normalized = std::clamp(index, 0, count - 1);
    return (normalized + offset + count) % count;
}

void DrawFittedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, Color color) {
    float adjustedSize = fontSize;
    while (adjustedSize > 11.0f && MeasureTextEx(font, text, adjustedSize, 0.0f).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), adjustedSize * scale, 0.0f, color);
}

void DrawCenteredFittedText(Game* game, Font font, const char* text, Rectangle rect, float fontSize, Color color) {
    float adjustedSize = fontSize;
    Vector2 measured = MeasureTextEx(font, text, adjustedSize, 0.0f);
    while (adjustedSize > 10.0f && measured.x > rect.width - 12.0f) {
        adjustedSize -= 1.0f;
        measured = MeasureTextEx(font, text, adjustedSize, 0.0f);
    }

    const Vector2 position = {
        rect.x + (rect.width - measured.x) * 0.5f,
        rect.y + (rect.height - measured.y) * 0.5f
    };
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), adjustedSize * scale, 0.0f, color);
}

void DrawKeyBadge(Game* game, Font font, const Theme& theme, Rectangle row, const char* key) {
    const Rectangle keyRect = KeyRect(row);
    DrawRectangleRounded(game->ScaleRect(keyRect), 0.28f, 8, Fade(theme.PanelBorder, 0.20f));
    DrawRectangleRoundedLines(game->ScaleRect(keyRect), 0.28f, 8, Fade(theme.TextDefault, 0.22f));
    DrawCenteredFittedText(game, font, key, keyRect, 14.0f, theme.TextDefault);
}

void DrawSettingRow(Game* game, Font font, const Theme& theme, int index, const char* key, const char* label, Vector2 mouse) {
    const Rectangle row = SettingsRow(index);
    const bool hover = Hit(game, row, mouse);
    DrawRectangleRounded(game->ScaleRect(row), 0.18f, 10, Fade(hover ? theme.Highlight : theme.PanelBorder, hover ? 0.12f : 0.055f));
    DrawKeyBadge(game, font, theme, row, key);
    DrawFittedText(game, font, label, { row.x + 76.0f, row.y + 13.0f }, 282.0f, 17.0f, theme.TextDefault);
}

void DrawSegmented(Game* game, Font font, const Theme& theme, Rectangle rect, const char* const* labels, int count, int selected, Vector2 mouse) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.42f, 12, Fade(theme.PanelBorder, 0.18f));

    for (int index = 0; index < count; ++index) {
        const Rectangle segment = {
            rect.x + rect.width * static_cast<float>(index) / static_cast<float>(count),
            rect.y,
            rect.width / static_cast<float>(count),
            rect.height
        };
        const bool active = index == selected;
        const bool hover = Hit(game, segment, mouse);
        if (active || hover) {
            DrawRectangleRounded(game->ScaleRect(segment), 0.40f, 12, Fade(active ? theme.Highlight : theme.Highlight, active ? 0.74f : 0.16f));
        }
        if (index > 0) {
            const Vector2 start = game->ScalePoint({ segment.x, segment.y + 7.0f });
            const Vector2 end = game->ScalePoint({ segment.x, segment.y + segment.height - 7.0f });
            DrawLineEx(start, end, 1.0f * game->GetUiScale(), Fade(theme.TextDefault, 0.15f));
        }
        DrawCenteredFittedText(game, font, labels[index], segment, 14.0f, active ? theme.Title : theme.TextDefault);
    }
}

void DrawSelector(Game* game, Font font, Font valueFont, const Theme& theme, Rectangle rect, const char* value, Vector2 mouse) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.30f, 10, Fade(theme.PanelBorder, 0.18f));

    const Rectangle left = SelectorLeft(rect);
    const Rectangle right = SelectorRight(rect);
    const bool hoverLeft = Hit(game, left, mouse);
    const bool hoverRight = Hit(game, right, mouse);
    DrawRectangleRounded(game->ScaleRect(left), 0.28f, 8, Fade(theme.Highlight, hoverLeft ? 0.26f : 0.12f));
    DrawRectangleRounded(game->ScaleRect(right), 0.28f, 8, Fade(theme.Highlight, hoverRight ? 0.26f : 0.12f));
    DrawCenteredFittedText(game, font, "<", left, 16.0f, theme.TextDefault);
    DrawCenteredFittedText(game, font, ">", right, 16.0f, theme.TextDefault);

    const Rectangle labelRect = { rect.x + 38.0f, rect.y, rect.width - 76.0f, rect.height };
    DrawCenteredFittedText(game, valueFont, value, labelRect, 14.0f, theme.Title);
}

void DrawThemeSelector(Game* game, Font font, const Theme& theme, Rectangle rect, int themeIndex, bool ru, Vector2 mouse) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.30f, 10, Fade(theme.PanelBorder, 0.18f));

    const Rectangle left = SelectorLeft(rect);
    const Rectangle right = SelectorRight(rect);
    const bool hoverLeft = Hit(game, left, mouse);
    const bool hoverRight = Hit(game, right, mouse);
    DrawRectangleRounded(game->ScaleRect(left), 0.28f, 8, Fade(theme.Highlight, hoverLeft ? 0.26f : 0.12f));
    DrawRectangleRounded(game->ScaleRect(right), 0.28f, 8, Fade(theme.Highlight, hoverRight ? 0.26f : 0.12f));
    DrawCenteredFittedText(game, font, "<", left, 16.0f, theme.TextDefault);
    DrawCenteredFittedText(game, font, ">", right, 16.0f, theme.TextDefault);

    const Rectangle labelRect = { rect.x + 38.0f, rect.y, rect.width - 164.0f, rect.height };
    DrawFittedText(
        game,
        font,
        ru ? ThemeManager::GetThemeLabelRu(themeIndex) : ThemeManager::GetThemeLabel(themeIndex),
        { labelRect.x + 10.0f, labelRect.y + 8.0f },
        labelRect.width - 18.0f,
        14.0f,
        theme.Title);

    const Theme preview = ThemeManager::GetTheme(themeIndex);
    const Color swatches[] = { preview.Background, preview.Panel, preview.Highlight, preview.TextCorrect };
    for (int index = 0; index < 4; ++index) {
        const Rectangle swatch = { rect.x + rect.width - 116.0f + index * 20.0f, rect.y + 8.0f, 14.0f, 14.0f };
        DrawRectangleRounded(game->ScaleRect(swatch), 0.50f, 8, swatches[index]);
        DrawRectangleRoundedLines(game->ScaleRect(swatch), 0.50f, 8, Fade(theme.TextDefault, 0.28f));
    }
}

void DrawToggle(Game* game, const Theme& theme, Rectangle rect, bool enabled, Vector2 mouse) {
    const bool hover = Hit(game, rect, mouse);
    const Rectangle scaled = game->ScaleRect(rect);
    DrawRectangleRounded(scaled, 0.50f, 14, Fade(enabled ? theme.TextCorrect : theme.PanelBorder, enabled ? 0.70f : (hover ? 0.28f : 0.20f)));

    const float scale = game->GetUiScale();
    const float knobRadius = 10.0f * scale;
    const Vector2 knob = game->ScalePoint({
        rect.x + (enabled ? rect.width - 15.0f : 15.0f),
        rect.y + rect.height * 0.5f
    });
    DrawCircleV(knob, knobRadius, enabled ? theme.Title : theme.TextDefault);
}

void DrawSlider(Game* game, Font font, const Theme& theme, Rectangle rect, float value, Vector2 mouse) {
    const Rectangle track = { rect.x, rect.y + 11.0f, rect.width - 58.0f, 8.0f };
    const Rectangle scaledTrack = game->ScaleRect(track);
    DrawRectangleRounded(scaledTrack, 0.50f, 10, Fade(theme.PanelBorder, 0.26f));

    const Rectangle filled = { track.x, track.y, track.width * std::clamp(value, 0.0f, 1.0f), track.height };
    DrawRectangleRounded(game->ScaleRect(filled), 0.50f, 10, Fade(theme.Highlight, 0.78f));

    const Vector2 knob = game->ScalePoint({ track.x + track.width * std::clamp(value, 0.0f, 1.0f), track.y + track.height * 0.5f });
    const float scale = game->GetUiScale();
    DrawCircleV(knob, 8.0f * scale, theme.Title);

    const Rectangle percentRect = { rect.x + rect.width - 52.0f, rect.y, 52.0f, rect.height };
    DrawCenteredFittedText(game, font, TextFormat("%.0f%%", value * 100.0f), percentRect, 14.0f, theme.TextDefault);
}

void DrawApplyButton(Game* game, Font font, const Theme& theme, Rectangle rect, bool enabled, bool ru, Vector2 mouse) {
    const bool hover = enabled && Hit(game, rect, mouse);
    DrawRectangleRounded(game->ScaleRect(rect), 0.26f, 10, Fade(enabled ? theme.Highlight : theme.PanelBorder, hover ? 0.86f : (enabled ? 0.68f : 0.18f)));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.26f, 10, Fade(enabled ? theme.Highlight : theme.PanelBorder, enabled ? 0.88f : 0.35f));
    DrawCenteredFittedText(game, font, ru ? u8"Подтвердить" : "Apply", rect, 16.0f, enabled ? theme.Title : theme.TextDefault);
}
}

void SettingsState::Init(Game* game) {
    gamePtr = game;
    LoadDraftFromCurrent();
}

void SettingsState::LoadDraftFromCurrent() {
    const int fontCount = gamePtr->GetTypingFontCount();
    draftThemeIndex = std::clamp(gamePtr->GetThemeIndex(), 0, gamePtr->GetThemeCount() - 1);
    draftLanguage = gamePtr->GetLanguage();
    draftDifficulty = gamePtr->GetProgress().GetDifficulty();
    draftTypingTextFontIndex = std::clamp(gamePtr->GetProgress().GetTypingTextFontIndex(), 0, fontCount - 1);
    draftKeyboardFontIndex = std::clamp(gamePtr->GetProgress().GetKeyboardFontIndex(), 0, fontCount - 1);
    draftAudioEnabled = AudioManager::IsEnabled();
    draftClickProfile = AudioManager::GetClickProfile() <= 0 ? 0 : 1;
    draftVolume = AudioManager::GetVolume();
}

void SettingsState::ApplyDraft() {
    gamePtr->SetThemeIndex(draftThemeIndex);
    gamePtr->SetLanguage(draftLanguage);
    gamePtr->GetProgress().SetDifficulty(draftDifficulty);
    gamePtr->GetProgress().SetTypingTextFontIndex(draftTypingTextFontIndex, gamePtr->GetTypingFontCount());
    gamePtr->GetProgress().SetKeyboardFontIndex(draftKeyboardFontIndex, gamePtr->GetTypingFontCount());
    AudioManager::SetEnabled(draftAudioEnabled);
    AudioManager::SetClickProfile(draftClickProfile);
    AudioManager::SetVolume(draftVolume);
    LoadDraftFromCurrent();
}

bool SettingsState::HasPendingChanges() const {
    return draftThemeIndex != gamePtr->GetThemeIndex()
        || draftLanguage != gamePtr->GetLanguage()
        || draftDifficulty != gamePtr->GetProgress().GetDifficulty()
        || draftTypingTextFontIndex != gamePtr->GetProgress().GetTypingTextFontIndex()
        || draftKeyboardFontIndex != gamePtr->GetProgress().GetKeyboardFontIndex()
        || draftAudioEnabled != AudioManager::IsEnabled()
        || draftClickProfile != AudioManager::GetClickProfile()
        || std::fabs(draftVolume - AudioManager::GetVolume()) > 0.001f;
}

void SettingsState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    } else if (IsKeyPressed(KEY_T)) {
        draftThemeIndex = NextWrappedIndex(draftThemeIndex, 1, gamePtr->GetThemeCount());
    } else if (IsKeyPressed(KEY_L)) {
        draftLanguage = draftLanguage == Language::English ? Language::Russian : Language::English;
    } else if (IsKeyPressed(KEY_D)) {
        draftDifficulty = DifficultyFromIndex((DifficultyIndex(draftDifficulty) + 1) % 3);
    } else if (IsKeyPressed(KEY_F)) {
        draftTypingTextFontIndex = NextWrappedIndex(draftTypingTextFontIndex, 1, gamePtr->GetTypingFontCount());
    } else if (IsKeyPressed(KEY_K)) {
        draftKeyboardFontIndex = NextWrappedIndex(draftKeyboardFontIndex, 1, gamePtr->GetTypingFontCount());
    } else if (IsKeyPressed(KEY_A)) {
        draftAudioEnabled = !draftAudioEnabled;
    } else if (IsKeyPressed(KEY_C)) {
        draftClickProfile = draftClickProfile == 0 ? 1 : 0;
    } else if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
        draftVolume = std::clamp(draftVolume + 0.1f, 0.0f, 1.0f);
    } else if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        draftVolume = std::clamp(draftVolume - 0.1f, 0.0f, 1.0f);
    } else if (IsKeyPressed(KEY_R)) {
        gamePtr->GetProgress().Reset();
    } else if (IsKeyPressed(KEY_ENTER)) {
        ApplyDraft();
    }

    const Vector2 mouse = GetMousePosition();
    bool hover = false;
    for (int index = 0; index < 9; ++index) {
        if (Hit(gamePtr, SettingsRow(index), mouse)) {
            hover = true;
            break;
        }
    }
    if (HasPendingChanges() && Hit(gamePtr, ApplyButtonRect(), mouse)) {
        hover = true;
    }

    const Rectangle volumeControl = ControlRect(SettingsRow(7));
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && Hit(gamePtr, volumeControl, mouse)) {
        const Rectangle track = { volumeControl.x, volumeControl.y, volumeControl.width - 58.0f, volumeControl.height };
        draftVolume = SliderValueAt(gamePtr, track, mouse);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (HasPendingChanges() && Hit(gamePtr, ApplyButtonRect(), mouse)) {
            ApplyDraft();
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            return;
        }

        const Rectangle themeControl = ControlRect(SettingsRow(0));
        if (Hit(gamePtr, themeControl, mouse)) {
            const int offset = Hit(gamePtr, SelectorLeft(themeControl), mouse) ? -1 : 1;
            draftThemeIndex = NextWrappedIndex(draftThemeIndex, offset, gamePtr->GetThemeCount());
        }

        const Rectangle languageControl = ControlRect(SettingsRow(1));
        const int languageIndex = SegmentIndexAt(gamePtr, languageControl, 2, mouse);
        if (languageIndex >= 0) {
            draftLanguage = languageIndex == 0 ? Language::English : Language::Russian;
        }

        const Rectangle difficultyControl = ControlRect(SettingsRow(2));
        const int difficultyIndex = SegmentIndexAt(gamePtr, difficultyControl, 3, mouse);
        if (difficultyIndex >= 0) {
            draftDifficulty = DifficultyFromIndex(difficultyIndex);
        }

        const Rectangle typingFontControl = ControlRect(SettingsRow(3));
        if (Hit(gamePtr, typingFontControl, mouse)) {
            const int count = gamePtr->GetTypingFontCount();
            const int offset = Hit(gamePtr, SelectorLeft(typingFontControl), mouse) ? -1 : 1;
            draftTypingTextFontIndex = NextWrappedIndex(draftTypingTextFontIndex, offset, count);
        }

        const Rectangle keyboardFontControl = ControlRect(SettingsRow(4));
        if (Hit(gamePtr, keyboardFontControl, mouse)) {
            const int count = gamePtr->GetTypingFontCount();
            const int offset = Hit(gamePtr, SelectorLeft(keyboardFontControl), mouse) ? -1 : 1;
            draftKeyboardFontIndex = NextWrappedIndex(draftKeyboardFontIndex, offset, count);
        }

        const Rectangle soundControl = ControlRect(SettingsRow(5));
        if (Hit(gamePtr, soundControl, mouse)) {
            draftAudioEnabled = !draftAudioEnabled;
        }

        const Rectangle clickControl = ControlRect(SettingsRow(6));
        const int clickIndex = SegmentIndexAt(gamePtr, clickControl, 2, mouse);
        if (clickIndex >= 0) {
            draftClickProfile = clickIndex;
        }

        const Rectangle resetControl = ControlRect(SettingsRow(8));
        if (Hit(gamePtr, resetControl, mouse)) {
            gamePtr->GetProgress().Reset();
        }
    }

    SetMouseCursor(hover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void SettingsState::Update(float deltaTime) {
}

void SettingsState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetUiFont();
    const float scale = gamePtr->GetUiScale();
    const bool ru = IsRu(gamePtr);

    const Rectangle panel = gamePtr->ScaleRect({ 250.0f, 46.0f, 780.0f, 630.0f });
    DrawRectangleRounded(panel, 0.08f, 12, Fade(theme.Panel, 0.80f));
    DrawRectangleRoundedLines(panel, 0.08f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, ru ? u8"Настройки" : "Settings", gamePtr->ScalePoint({ 286.0f, 92.0f }), 39.0f * scale, 0.0f, theme.Title);
    DrawTextEx(font, ru ? u8"Внешний вид, ввод и звук" : "Display, input and sound", gamePtr->ScalePoint({ 286.0f, 124.0f }), 16.0f * scale, 0.0f, theme.TextDefault);

    const Vector2 mouse = GetMousePosition();

    DrawSettingRow(gamePtr, font, theme, 0, "T", ru ? u8"Тема" : "Theme", mouse);
    DrawSettingRow(gamePtr, font, theme, 1, "L", ru ? u8"Язык" : "Language", mouse);
    DrawSettingRow(gamePtr, font, theme, 2, "D", ru ? u8"Сложность" : "Difficulty", mouse);
    DrawSettingRow(gamePtr, font, theme, 3, "F", ru ? u8"Шрифт текста" : "Typing text font", mouse);
    DrawSettingRow(gamePtr, font, theme, 4, "K", ru ? u8"Шрифт клавиш" : "Keyboard font", mouse);
    DrawSettingRow(gamePtr, font, theme, 5, "A", ru ? u8"Звук" : "Sound", mouse);
    DrawSettingRow(gamePtr, font, theme, 6, "C", ru ? u8"Профиль клика" : "Click profile", mouse);
    DrawSettingRow(gamePtr, font, theme, 7, "+/-", ru ? u8"Громкость" : "Volume", mouse);
    DrawSettingRow(gamePtr, font, theme, 8, "R", ru ? u8"Сброс прогресса" : "Reset progress", mouse);

    DrawThemeSelector(gamePtr, font, theme, ControlRect(SettingsRow(0)), draftThemeIndex, ru, mouse);

    const char* languageLabels[] = { "EN", "RU" };
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingsRow(1)), languageLabels, 2, draftLanguage == Language::Russian ? 1 : 0, mouse);

    const char* difficultyLabelsRu[] = { u8"Легкая", u8"Норма", u8"Строгая" };
    const char* difficultyLabelsEn[] = { "Relaxed", "Normal", "Strict" };
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingsRow(2)), ru ? difficultyLabelsRu : difficultyLabelsEn, 3, DifficultyIndex(draftDifficulty), mouse);

    DrawSelector(gamePtr, font, gamePtr->GetTypingFontByIndex(draftTypingTextFontIndex), theme, ControlRect(SettingsRow(3)), gamePtr->GetTypingFontLabel(draftTypingTextFontIndex), mouse);
    DrawSelector(gamePtr, font, gamePtr->GetTypingFontByIndex(draftKeyboardFontIndex), theme, ControlRect(SettingsRow(4)), gamePtr->GetTypingFontLabel(draftKeyboardFontIndex), mouse);

    const Rectangle soundControl = ControlRect(SettingsRow(5));
    DrawToggle(gamePtr, theme, { soundControl.x + soundControl.width - 70.0f, soundControl.y + 1.0f, 58.0f, 28.0f }, draftAudioEnabled, mouse);
    DrawFittedText(gamePtr, font, draftAudioEnabled ? (ru ? u8"Вкл" : "On") : (ru ? u8"Выкл" : "Off"), { soundControl.x + 6.0f, soundControl.y + 7.0f }, 160.0f, 14.0f, theme.TextDefault);

    const char* clickLabelsRu[] = { u8"Мягкий", u8"Яркий" };
    const char* clickLabelsEn[] = { "Soft", "Bright" };
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingsRow(6)), ru ? clickLabelsRu : clickLabelsEn, 2, draftClickProfile, mouse);

    DrawSlider(gamePtr, font, theme, ControlRect(SettingsRow(7)), draftVolume, mouse);

    const Rectangle resetControl = ControlRect(SettingsRow(8));
    DrawRectangleRounded(gamePtr->ScaleRect(resetControl), 0.32f, 10, Fade(theme.TextError, Hit(gamePtr, resetControl, mouse) ? 0.25f : 0.14f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect(resetControl), 0.32f, 10, Fade(theme.TextError, 0.55f));
    DrawCenteredFittedText(gamePtr, font, ru ? u8"Сбросить" : "Reset", resetControl, 15.0f, theme.TextError);

    const bool pending = HasPendingChanges();
    DrawFittedText(
        gamePtr,
        font,
        pending ? (ru ? u8"Есть непримененные изменения" : "Pending changes") : (ru ? u8"Изменения применены" : "Settings applied"),
        { 286.0f, 628.0f },
        420.0f,
        14.0f,
        Fade(theme.TextDefault, pending ? 0.92f : 0.58f));
    DrawApplyButton(gamePtr, font, theme, ApplyButtonRect(), pending, ru, mouse);
}
