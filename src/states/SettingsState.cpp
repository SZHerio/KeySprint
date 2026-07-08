#include "SettingsState.h"
#include "../core/Game.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include "../core/AudioManager.h"
#include "../core/UiDraw.h"
#include "../core/UiMotion.h"
#include "MainMenuState.h"

namespace {
enum class SettingRowId {
    Theme,
    InterfaceLanguage,
    TypingLanguage,
    Difficulty,
    UiFont,
    TypingFont,
    KeyboardFont,
    Sound,
    ClickProfile,
    Volume,
    ResetProgress,
    Count
};

struct SettingRowDefinition {
    SettingRowId id;
    const char* key;
    const char* labelRu;
    const char* labelEn;
};

constexpr int SettingRowCount = static_cast<int>(SettingRowId::Count);

const SettingRowDefinition SettingRows[SettingRowCount] = {
    { SettingRowId::Theme, "T", u8"Тема", "Theme" },
    { SettingRowId::InterfaceLanguage, "I", u8"Язык интерфейса", "Interface language" },
    { SettingRowId::TypingLanguage, "L", u8"Язык набора", "Typing language" },
    { SettingRowId::Difficulty, "D", u8"Сложность", "Difficulty" },
    { SettingRowId::UiFont, "U", u8"Шрифт интерфейса", "Interface font" },
    { SettingRowId::TypingFont, "F", u8"Шрифт печати", "Typing font" },
    { SettingRowId::KeyboardFont, "K", u8"Шрифт клавиш", "Keyboard font" },
    { SettingRowId::Sound, "A", u8"Звук", "Sound" },
    { SettingRowId::ClickProfile, "C", u8"Профиль клика", "Click profile" },
    { SettingRowId::Volume, "+/-", u8"Громкость", "Volume" },
    { SettingRowId::ResetProgress, "R", u8"Сброс прогресса", "Reset progress" }
};

constexpr int RowIndex(SettingRowId id) {
    return static_cast<int>(id);
}

Rectangle SettingsPanel() {
    return { 110.0f, 28.0f, 1060.0f, 664.0f };
}

Rectangle SettingsRow(int index) {
    return { 150.0f, 136.0f + index * 41.0f, 980.0f, 36.0f };
}

Rectangle SettingsRow(SettingRowId id) {
    return SettingsRow(RowIndex(id));
}

Rectangle ControlRect(Rectangle row) {
    return { row.x + 510.0f, row.y + 4.0f, row.width - 540.0f, 28.0f };
}

Rectangle ControlRect(SettingRowId id) {
    return ControlRect(SettingsRow(id));
}

Rectangle KeyRect(Rectangle row) {
    return { row.x + 14.0f, row.y + 6.0f, 46.0f, 24.0f };
}

Rectangle SelectorLeft(Rectangle control) {
    return { control.x, control.y, 34.0f, control.height };
}

Rectangle SelectorRight(Rectangle control) {
    return { control.x + control.width - 34.0f, control.y, 34.0f, control.height };
}

Rectangle MenuButtonRect() {
    return { 910.0f, 620.0f, 220.0f, 42.0f };
}

bool IsRu(Game* game) {
    return game->GetLanguage() == Language::Russian;
}

bool Hit(Game* game, Rectangle rect, Vector2 mouse) {
    return CheckCollisionPointRec(mouse, game->ScaleRect(rect));
}

Color HoverOutline(Game* game) {
    return game->IsDarkTheme() ? WHITE : BLACK;
}

Color TextOnHighlight(const Theme& theme) {
    (void)theme;
    return WHITE;
}

void DrawHoverOutline(Game* game, Rectangle rect, float roundness, int segments) {
    DrawRectangleRoundedLines(game->ScaleRect(rect), roundness, segments, Fade(HoverOutline(game), 0.88f));
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

int LanguageIndex(Language language) {
    return language == Language::Russian ? 1 : 0;
}

float MoveToward(float current, float target, float deltaTime) {
    return current + (target - current) * std::min(1.0f, deltaTime * SelectionMoveSpeed);
}

int NextWrappedIndex(int index, int offset, int count) {
    if (count <= 0) {
        return 0;
    }
    const int normalized = std::clamp(index, 0, count - 1);
    return (normalized + offset + count) % count;
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
    (void)mouse;
    DrawKeyBadge(game, font, theme, row, key);
    Ui::DrawFittedText(game, font, label, { row.x + 76.0f, row.y + 11.0f }, 410.0f, 16.0f, 0.0f, theme.TextDefault);
}

void DrawSegmented(Game* game, Font font, const Theme& theme, Rectangle rect, const char* const* labels, int count, int selected, float selectedPosition, Vector2 mouse) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.42f, 12, Fade(theme.PanelBorder, 0.18f));
    const Color activeText = TextOnHighlight(theme);

    if (count > 0) {
        const float segmentWidth = rect.width / static_cast<float>(count);
        const Rectangle activeRect = {
            rect.x + segmentWidth * std::clamp(selectedPosition, 0.0f, static_cast<float>(count - 1)),
            rect.y,
            segmentWidth,
            rect.height
        };
        DrawRectangleRounded(game->ScaleRect(activeRect), 0.40f, 12, Fade(theme.Highlight, 0.90f));
    }

    for (int index = 0; index < count; ++index) {
        const Rectangle segment = {
            rect.x + rect.width * static_cast<float>(index) / static_cast<float>(count),
            rect.y,
            rect.width / static_cast<float>(count),
            rect.height
        };
        const bool active = index == selected;
        const bool hover = Hit(game, segment, mouse);
        if (hover && !active) {
            DrawRectangleRounded(game->ScaleRect(segment), 0.40f, 12, Fade(theme.Highlight, 0.16f));
        }
        if (hover) {
            DrawHoverOutline(game, segment, 0.40f, 12);
        }
        if (index > 0) {
            const Vector2 start = game->ScalePoint({ segment.x, segment.y + 7.0f });
            const Vector2 end = game->ScalePoint({ segment.x, segment.y + segment.height - 7.0f });
            DrawLineEx(start, end, 1.0f * game->GetUiScale(), Fade(theme.TextDefault, 0.15f));
        }
        DrawCenteredFittedText(game, font, labels[index], segment, 14.0f, active ? activeText : theme.TextDefault);
    }
}

void DrawSelector(Game* game, Font font, Font valueFont, const Theme& theme, Rectangle rect, const char* value, float pulse, float direction, Vector2 mouse) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.30f, 10, Fade(theme.PanelBorder, 0.18f));

    const Rectangle left = SelectorLeft(rect);
    const Rectangle right = SelectorRight(rect);
    const bool hoverLeft = Hit(game, left, mouse);
    const bool hoverRight = Hit(game, right, mouse);
    DrawRectangleRounded(game->ScaleRect(left), 0.28f, 8, Fade(theme.Highlight, hoverLeft ? 0.26f : 0.12f));
    DrawRectangleRounded(game->ScaleRect(right), 0.28f, 8, Fade(theme.Highlight, hoverRight ? 0.26f : 0.12f));
    if (Hit(game, rect, mouse)) {
        DrawHoverOutline(game, rect, 0.30f, 10);
    }
    DrawCenteredFittedText(game, font, "<", left, 16.0f, theme.TextDefault);
    DrawCenteredFittedText(game, font, ">", right, 16.0f, theme.TextDefault);

    const Rectangle labelRect = { rect.x + 38.0f + direction * pulse * 8.0f, rect.y, rect.width - 76.0f, rect.height };
    DrawRectangleRounded(game->ScaleRect({ rect.x + 38.0f, rect.y, rect.width - 76.0f, rect.height }), 0.26f, 8, Fade(theme.Highlight, 0.04f + pulse * 0.08f));
    DrawCenteredFittedText(game, valueFont, value, labelRect, 14.0f, theme.Title);
}

void DrawThemeSelector(Game* game, Font font, const Theme& theme, Rectangle rect, int themeIndex, bool ru, float pulse, float direction, Vector2 mouse) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.30f, 10, Fade(theme.PanelBorder, 0.18f));

    const Rectangle left = SelectorLeft(rect);
    const Rectangle right = SelectorRight(rect);
    const bool hoverLeft = Hit(game, left, mouse);
    const bool hoverRight = Hit(game, right, mouse);
    DrawRectangleRounded(game->ScaleRect(left), 0.28f, 8, Fade(theme.Highlight, hoverLeft ? 0.26f : 0.12f));
    DrawRectangleRounded(game->ScaleRect(right), 0.28f, 8, Fade(theme.Highlight, hoverRight ? 0.26f : 0.12f));
    if (Hit(game, rect, mouse)) {
        DrawHoverOutline(game, rect, 0.30f, 10);
    }
    DrawCenteredFittedText(game, font, "<", left, 16.0f, theme.TextDefault);
    DrawCenteredFittedText(game, font, ">", right, 16.0f, theme.TextDefault);

    const Rectangle labelRect = { rect.x + 38.0f + direction * pulse * 8.0f, rect.y, rect.width - 164.0f, rect.height };
    DrawRectangleRounded(game->ScaleRect({ rect.x + 38.0f, rect.y, rect.width - 164.0f, rect.height }), 0.26f, 8, Fade(theme.Highlight, 0.03f + pulse * 0.07f));
    Ui::DrawFittedText(
        game,
        font,
        ru ? ThemeManager::GetThemeLabelRu(themeIndex) : ThemeManager::GetThemeLabel(themeIndex),
        { labelRect.x + 10.0f, labelRect.y + 8.0f },
        labelRect.width - 18.0f,
        14.0f,
        0.0f,
        theme.Title);

    const Theme preview = ThemeManager::GetTheme(themeIndex);
    const Color swatches[] = { preview.Background, preview.Panel, preview.Highlight, preview.TextCorrect };
    for (int index = 0; index < 4; ++index) {
        const Rectangle swatch = { rect.x + rect.width - 116.0f + index * 20.0f, rect.y + 8.0f, 14.0f, 14.0f };
        DrawRectangleRounded(game->ScaleRect(swatch), 0.50f, 8, swatches[index]);
        DrawRectangleRoundedLines(game->ScaleRect(swatch), 0.50f, 8, Fade(theme.TextDefault, 0.28f));
    }
}

void DrawToggle(Game* game, const Theme& theme, Rectangle rect, bool enabled, float position, Vector2 mouse) {
    const bool hover = Hit(game, rect, mouse);
    const Rectangle scaled = game->ScaleRect(rect);
    DrawRectangleRounded(scaled, 0.50f, 14, Fade(enabled ? theme.TextCorrect : theme.PanelBorder, enabled ? 0.70f : (hover ? 0.28f : 0.20f)));
    if (hover) {
        DrawHoverOutline(game, rect, 0.50f, 14);
    }

    const float scale = game->GetUiScale();
    const float knobRadius = 10.0f * scale;
    const Vector2 knob = game->ScalePoint({
        rect.x + 15.0f + (rect.width - 30.0f) * std::clamp(position, 0.0f, 1.0f),
        rect.y + rect.height * 0.5f
    });
    DrawCircleV(knob, knobRadius, enabled ? theme.Title : theme.TextDefault);
}

void DrawSlider(Game* game, Font font, const Theme& theme, Rectangle rect, float value, float displayValue, Vector2 mouse) {
    const Rectangle track = { rect.x, rect.y + 11.0f, rect.width - 58.0f, 8.0f };
    const Rectangle scaledTrack = game->ScaleRect(track);
    DrawRectangleRounded(scaledTrack, 0.50f, 10, Fade(theme.PanelBorder, 0.26f));

    const Rectangle filled = { track.x, track.y, track.width * std::clamp(displayValue, 0.0f, 1.0f), track.height };
    DrawRectangleRounded(game->ScaleRect(filled), 0.50f, 10, Fade(theme.Highlight, 0.78f));

    const Vector2 knob = game->ScalePoint({ track.x + track.width * std::clamp(displayValue, 0.0f, 1.0f), track.y + track.height * 0.5f });
    const float scale = game->GetUiScale();
    DrawCircleV(knob, 8.0f * scale, theme.Title);

    const Rectangle percentRect = { rect.x + rect.width - 52.0f, rect.y, 52.0f, rect.height };
    DrawCenteredFittedText(game, font, TextFormat("%.0f%%", value * 100.0f), percentRect, 14.0f, theme.TextDefault);
}

void DrawMenuButton(Game* game, Font font, const Theme& theme, Rectangle rect, bool ru, Vector2 mouse) {
    const bool hover = Hit(game, rect, mouse);
    DrawRectangleRounded(game->ScaleRect(rect), 0.26f, 10, Fade(theme.Highlight, hover ? 0.92f : 0.82f));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.26f, 10, hover ? Fade(HoverOutline(game), 0.92f) : Fade(theme.Highlight, 0.72f));
    DrawCenteredFittedText(game, font, ru ? u8"Выйти в меню" : "Back to menu", rect, 16.0f, TextOnHighlight(theme));
}
}

void SettingsState::Init(Game* game) {
    gamePtr = game;
    LoadDraftFromCurrent();
}

void SettingsState::LoadDraftFromCurrent() {
    const int fontCount = gamePtr->GetTypingFontCount();
    draftThemeIndex = std::clamp(gamePtr->GetThemeIndex(), 0, gamePtr->GetThemeCount() - 1);
    draftInterfaceLanguage = gamePtr->GetLanguage();
    draftTypingLanguage = gamePtr->GetTypingLanguage();
    draftDifficulty = gamePtr->GetProgress().GetDifficulty();
    draftUiFontIndex = std::clamp(gamePtr->GetProgress().GetUiFontIndex(), 0, gamePtr->GetUiFontCount() - 1);
    draftTypingTextFontIndex = std::clamp(gamePtr->GetProgress().GetTypingTextFontIndex(), 0, fontCount - 1);
    draftKeyboardFontIndex = std::clamp(gamePtr->GetProgress().GetKeyboardFontIndex(), 0, fontCount - 1);
    draftAudioEnabled = AudioManager::IsEnabled();
    draftClickProfile = AudioManager::GetClickProfile() <= 0 ? 0 : 1;
    draftVolume = AudioManager::GetVolume();
    interfaceLanguagePosition = static_cast<float>(LanguageIndex(draftInterfaceLanguage));
    typingLanguagePosition = static_cast<float>(LanguageIndex(draftTypingLanguage));
    difficultyPosition = static_cast<float>(DifficultyIndex(draftDifficulty));
    clickProfilePosition = static_cast<float>(draftClickProfile);
    soundPosition = draftAudioEnabled ? 1.0f : 0.0f;
    volumeVisual = draftVolume;
}

void SettingsState::SetDraftThemeIndex(int index, int direction) {
    draftThemeIndex = std::clamp(index, 0, gamePtr->GetThemeCount() - 1);
    gamePtr->SetThemeIndex(draftThemeIndex);
    themePulse = 1.0f;
    themeDirection = direction < 0 ? -1.0f : 1.0f;
}

void SettingsState::SetDraftInterfaceLanguage(Language language) {
    draftInterfaceLanguage = language;
    gamePtr->SetLanguage(draftInterfaceLanguage);
}

void SettingsState::SetDraftTypingLanguage(Language language) {
    draftTypingLanguage = language;
    gamePtr->SetTypingLanguage(draftTypingLanguage);
}

void SettingsState::SetDraftDifficulty(Difficulty difficulty) {
    draftDifficulty = difficulty;
    gamePtr->GetProgress().SetDifficulty(draftDifficulty);
}

void SettingsState::SetDraftUiFontIndex(int index, int direction) {
    draftUiFontIndex = std::clamp(index, 0, gamePtr->GetUiFontCount() - 1);
    gamePtr->SetUiFontIndex(draftUiFontIndex);
    uiFontPulse = 1.0f;
    uiFontDirection = direction < 0 ? -1.0f : 1.0f;
}

void SettingsState::SetDraftTypingTextFontIndex(int index, int direction) {
    draftTypingTextFontIndex = std::clamp(index, 0, gamePtr->GetTypingFontCount() - 1);
    gamePtr->SetTypingTextFontIndex(draftTypingTextFontIndex);
    typingFontPulse = 1.0f;
    typingFontDirection = direction < 0 ? -1.0f : 1.0f;
}

void SettingsState::SetDraftKeyboardFontIndex(int index, int direction) {
    draftKeyboardFontIndex = std::clamp(index, 0, gamePtr->GetTypingFontCount() - 1);
    gamePtr->SetKeyboardFontIndex(draftKeyboardFontIndex);
    keyboardFontPulse = 1.0f;
    keyboardFontDirection = direction < 0 ? -1.0f : 1.0f;
}

void SettingsState::SetDraftAudioEnabled(bool enabled) {
    draftAudioEnabled = enabled;
    AudioManager::SetEnabled(draftAudioEnabled);
}

void SettingsState::SetDraftClickProfile(int profile) {
    draftClickProfile = std::clamp(profile, 0, 1);
    AudioManager::SetClickProfile(draftClickProfile);
}

void SettingsState::SetDraftVolume(float volume) {
    draftVolume = std::clamp(volume, 0.0f, 1.0f);
    AudioManager::SetVolume(draftVolume);
}

void SettingsState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedRow = (selectedRow + 1) % SettingRowCount;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedRow = (selectedRow + SettingRowCount - 1) % SettingRowCount;
    } else if (IsKeyPressed(KEY_T)) {
        SetDraftThemeIndex(NextWrappedIndex(draftThemeIndex, 1, gamePtr->GetThemeCount()), 1);
    } else if (IsKeyPressed(KEY_I)) {
        SetDraftInterfaceLanguage(draftInterfaceLanguage == Language::English ? Language::Russian : Language::English);
    } else if (IsKeyPressed(KEY_L)) {
        SetDraftTypingLanguage(draftTypingLanguage == Language::English ? Language::Russian : Language::English);
    } else if (IsKeyPressed(KEY_D)) {
        SetDraftDifficulty(DifficultyFromIndex((DifficultyIndex(draftDifficulty) + 1) % 3));
    } else if (IsKeyPressed(KEY_U)) {
        SetDraftUiFontIndex(NextWrappedIndex(draftUiFontIndex, 1, gamePtr->GetUiFontCount()), 1);
    } else if (IsKeyPressed(KEY_F)) {
        SetDraftTypingTextFontIndex(NextWrappedIndex(draftTypingTextFontIndex, 1, gamePtr->GetTypingFontCount()), 1);
    } else if (IsKeyPressed(KEY_K)) {
        SetDraftKeyboardFontIndex(NextWrappedIndex(draftKeyboardFontIndex, 1, gamePtr->GetTypingFontCount()), 1);
    } else if (IsKeyPressed(KEY_A)) {
        SetDraftAudioEnabled(!draftAudioEnabled);
    } else if (IsKeyPressed(KEY_C)) {
        SetDraftClickProfile(draftClickProfile == 0 ? 1 : 0);
    } else if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
        SetDraftVolume(draftVolume + 0.1f);
    } else if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        SetDraftVolume(draftVolume - 0.1f);
    } else if (IsKeyPressed(KEY_R)) {
        gamePtr->GetProgress().Reset();
        LoadDraftFromCurrent();
    } else if (IsKeyPressed(KEY_ENTER)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    }

    const Vector2 mouse = GetMousePosition();
    bool hover = false;
    for (int index = 0; index < SettingRowCount; ++index) {
        if (Hit(gamePtr, SettingsRow(index), mouse)) {
            selectedRow = index;
            hover = true;
            break;
        }
    }
    if (Hit(gamePtr, MenuButtonRect(), mouse)) {
        hover = true;
    }

    const Rectangle volumeControl = ControlRect(SettingRowId::Volume);
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && Hit(gamePtr, volumeControl, mouse)) {
        const Rectangle track = { volumeControl.x, volumeControl.y, volumeControl.width - 58.0f, volumeControl.height };
        SetDraftVolume(SliderValueAt(gamePtr, track, mouse));
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (Hit(gamePtr, MenuButtonRect(), mouse)) {
            gamePtr->ChangeState(std::make_shared<MainMenuState>());
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            return;
        }

        const Rectangle themeControl = ControlRect(SettingRowId::Theme);
        if (Hit(gamePtr, themeControl, mouse)) {
            const int offset = Hit(gamePtr, SelectorLeft(themeControl), mouse) ? -1 : 1;
            SetDraftThemeIndex(NextWrappedIndex(draftThemeIndex, offset, gamePtr->GetThemeCount()), offset);
        }

        const Rectangle interfaceLanguageControl = ControlRect(SettingRowId::InterfaceLanguage);
        const int interfaceLanguageIndex = SegmentIndexAt(gamePtr, interfaceLanguageControl, 2, mouse);
        if (interfaceLanguageIndex >= 0) {
            SetDraftInterfaceLanguage(interfaceLanguageIndex == 0 ? Language::English : Language::Russian);
        }

        const Rectangle typingLanguageControl = ControlRect(SettingRowId::TypingLanguage);
        const int typingLanguageIndex = SegmentIndexAt(gamePtr, typingLanguageControl, 2, mouse);
        if (typingLanguageIndex >= 0) {
            SetDraftTypingLanguage(typingLanguageIndex == 0 ? Language::English : Language::Russian);
        }

        const Rectangle difficultyControl = ControlRect(SettingRowId::Difficulty);
        const int difficultyIndex = SegmentIndexAt(gamePtr, difficultyControl, 3, mouse);
        if (difficultyIndex >= 0) {
            SetDraftDifficulty(DifficultyFromIndex(difficultyIndex));
        }

        const Rectangle uiFontControl = ControlRect(SettingRowId::UiFont);
        if (Hit(gamePtr, uiFontControl, mouse)) {
            const int count = gamePtr->GetUiFontCount();
            const int offset = Hit(gamePtr, SelectorLeft(uiFontControl), mouse) ? -1 : 1;
            SetDraftUiFontIndex(NextWrappedIndex(draftUiFontIndex, offset, count), offset);
        }

        const Rectangle typingFontControl = ControlRect(SettingRowId::TypingFont);
        if (Hit(gamePtr, typingFontControl, mouse)) {
            const int count = gamePtr->GetTypingFontCount();
            const int offset = Hit(gamePtr, SelectorLeft(typingFontControl), mouse) ? -1 : 1;
            SetDraftTypingTextFontIndex(NextWrappedIndex(draftTypingTextFontIndex, offset, count), offset);
        }

        const Rectangle keyboardFontControl = ControlRect(SettingRowId::KeyboardFont);
        if (Hit(gamePtr, keyboardFontControl, mouse)) {
            const int count = gamePtr->GetTypingFontCount();
            const int offset = Hit(gamePtr, SelectorLeft(keyboardFontControl), mouse) ? -1 : 1;
            SetDraftKeyboardFontIndex(NextWrappedIndex(draftKeyboardFontIndex, offset, count), offset);
        }

        const Rectangle soundControl = ControlRect(SettingRowId::Sound);
        if (Hit(gamePtr, soundControl, mouse)) {
            SetDraftAudioEnabled(!draftAudioEnabled);
        }

        const Rectangle clickControl = ControlRect(SettingRowId::ClickProfile);
        const int clickIndex = SegmentIndexAt(gamePtr, clickControl, 2, mouse);
        if (clickIndex >= 0) {
            SetDraftClickProfile(clickIndex);
        }

        const Rectangle resetControl = ControlRect(SettingRowId::ResetProgress);
        if (Hit(gamePtr, resetControl, mouse)) {
            gamePtr->GetProgress().Reset();
            LoadDraftFromCurrent();
        }
    }

    SetMouseCursor(hover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void SettingsState::Update(float deltaTime) {
    interfaceLanguagePosition = MoveToward(interfaceLanguagePosition, static_cast<float>(LanguageIndex(draftInterfaceLanguage)), deltaTime);
    typingLanguagePosition = MoveToward(typingLanguagePosition, static_cast<float>(LanguageIndex(draftTypingLanguage)), deltaTime);
    difficultyPosition = MoveToward(difficultyPosition, static_cast<float>(DifficultyIndex(draftDifficulty)), deltaTime);
    clickProfilePosition = MoveToward(clickProfilePosition, static_cast<float>(draftClickProfile), deltaTime);
    soundPosition = MoveToward(soundPosition, draftAudioEnabled ? 1.0f : 0.0f, deltaTime);
    volumeVisual = MoveToward(volumeVisual, draftVolume, deltaTime);
    themePulse = std::max(0.0f, themePulse - deltaTime * SelectionMoveSpeed);
    uiFontPulse = std::max(0.0f, uiFontPulse - deltaTime * SelectionMoveSpeed);
    typingFontPulse = std::max(0.0f, typingFontPulse - deltaTime * SelectionMoveSpeed);
    keyboardFontPulse = std::max(0.0f, keyboardFontPulse - deltaTime * SelectionMoveSpeed);
}

void SettingsState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetUiFont();
    const float scale = gamePtr->GetUiScale();
    const bool ru = IsRu(gamePtr);

    const Rectangle panel = gamePtr->ScaleRect(SettingsPanel());
    DrawRectangleRounded(panel, 0.08f, 12, Fade(theme.Panel, 0.80f));
    DrawRectangleRoundedLines(panel, 0.08f, 12, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, ru ? u8"Настройки" : "Settings", gamePtr->ScalePoint({ 150.0f, 62.0f }), 36.0f * scale, 0.0f, theme.Title);
    DrawTextEx(font, ru ? u8"Внешний вид, языки, ввод и звук" : "Display, languages, input and sound", gamePtr->ScalePoint({ 152.0f, 106.0f }), 16.0f * scale, 0.0f, theme.TextDefault);

    const Vector2 mouse = GetMousePosition();

    for (int index = 0; index < SettingRowCount; ++index) {
        const SettingRowDefinition& row = SettingRows[index];
        DrawSettingRow(gamePtr, font, theme, index, row.key, ru ? row.labelRu : row.labelEn, mouse);
    }

    DrawThemeSelector(gamePtr, font, theme, ControlRect(SettingRowId::Theme), draftThemeIndex, ru, themePulse, themeDirection, mouse);

    const char* languageLabels[] = { "EN", "RU" };
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingRowId::InterfaceLanguage), languageLabels, 2, LanguageIndex(draftInterfaceLanguage), interfaceLanguagePosition, mouse);
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingRowId::TypingLanguage), languageLabels, 2, LanguageIndex(draftTypingLanguage), typingLanguagePosition, mouse);

    const char* difficultyLabelsRu[] = { u8"Легкая", u8"Норма", u8"Строгая" };
    const char* difficultyLabelsEn[] = { "Relaxed", "Normal", "Strict" };
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingRowId::Difficulty), ru ? difficultyLabelsRu : difficultyLabelsEn, 3, DifficultyIndex(draftDifficulty), difficultyPosition, mouse);

    DrawSelector(gamePtr, font, gamePtr->GetUiFontByIndex(draftUiFontIndex), theme, ControlRect(SettingRowId::UiFont), gamePtr->GetUiFontLabel(draftUiFontIndex), uiFontPulse, uiFontDirection, mouse);
    DrawSelector(gamePtr, font, gamePtr->GetTypingFontByIndex(draftTypingTextFontIndex), theme, ControlRect(SettingRowId::TypingFont), gamePtr->GetTypingFontLabel(draftTypingTextFontIndex), typingFontPulse, typingFontDirection, mouse);
    DrawSelector(gamePtr, font, gamePtr->GetTypingFontByIndex(draftKeyboardFontIndex), theme, ControlRect(SettingRowId::KeyboardFont), gamePtr->GetTypingFontLabel(draftKeyboardFontIndex), keyboardFontPulse, keyboardFontDirection, mouse);

    const Rectangle soundControl = ControlRect(SettingRowId::Sound);
    DrawToggle(gamePtr, theme, { soundControl.x + soundControl.width - 70.0f, soundControl.y + 1.0f, 58.0f, 28.0f }, draftAudioEnabled, soundPosition, mouse);
    Ui::DrawFittedText(gamePtr, font, draftAudioEnabled ? (ru ? u8"Вкл" : "On") : (ru ? u8"Выкл" : "Off"), { soundControl.x + 6.0f, soundControl.y + 7.0f }, 160.0f, 14.0f, 0.0f, theme.TextDefault);

    const char* clickLabelsRu[] = { u8"Мягкий", u8"Яркий" };
    const char* clickLabelsEn[] = { "Soft", "Bright" };
    DrawSegmented(gamePtr, font, theme, ControlRect(SettingRowId::ClickProfile), ru ? clickLabelsRu : clickLabelsEn, 2, draftClickProfile, clickProfilePosition, mouse);

    DrawSlider(gamePtr, font, theme, ControlRect(SettingRowId::Volume), draftVolume, volumeVisual, mouse);

    const Rectangle resetControl = ControlRect(SettingRowId::ResetProgress);
    DrawRectangleRounded(gamePtr->ScaleRect(resetControl), 0.32f, 10, Fade(theme.TextError, Hit(gamePtr, resetControl, mouse) ? 0.25f : 0.14f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect(resetControl), 0.32f, 10, Hit(gamePtr, resetControl, mouse) ? Fade(HoverOutline(gamePtr), 0.88f) : Fade(theme.TextError, 0.55f));
    DrawCenteredFittedText(gamePtr, font, ru ? u8"Сбросить" : "Reset", resetControl, 15.0f, theme.TextError);

    const char* status = ru ? u8"\u0418\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u043f\u0440\u0438\u043c\u0435\u043d\u044f\u044e\u0442\u0441\u044f \u0441\u0440\u0430\u0437\u0443" : "Changes apply instantly";
    Ui::DrawFittedText(
        gamePtr,
        font,
        status,
        { 150.0f, 632.0f },
        420.0f,
        14.0f,
        0.0f,
        Fade(theme.TextDefault, 0.70f));
    DrawMenuButton(gamePtr, font, theme, MenuButtonRect(), ru, mouse);
}
