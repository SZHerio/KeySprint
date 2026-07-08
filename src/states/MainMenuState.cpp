#include "MainMenuState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include "../core/LessonLibrary.h"
#include "../core/ModeVisual.h"
#include "LessonSelectState.h"
#include "ProgressState.h"
#include "TypingState.h"
#include "SettingsState.h"

namespace {
const char* LocalizedOption(int index, Language language) {
    const bool ru = language == Language::Russian;
    switch (index) {
        case 0: return ru ? u8"Практика" : "Practice";
        case 1: return ru ? u8"Уроки" : "Lessons";
        case 2: return ru ? u8"Испытание дня" : "Daily Challenge";
        case 3: return ru ? u8"Сочинение" : "Composition";
        case 4: return ru ? u8"Прогресс" : "Progress";
        case 5: return ru ? u8"Настройки" : "Settings";
        case 6: return ru ? u8"Выход" : "Exit";
        default: return "";
    }
}

const char* LocalizedDescription(int index, Language language) {
    const bool ru = language == Language::Russian;
    switch (index) {
        case 0: return ru ? u8"Короткие случайные упражнения с WPM, точностью и подсказками пальцев." : "Short generated drills with live WPM and finger guidance.";
        case 1: return ru ? u8"Выбирай урок на карте курса, повторяй старые и открывай новые." : "Pick a lesson card, replay older drills or unlock the next level.";
        case 2: return ru ? u8"Ежедневный смешанный текст с игровой целью на сессию." : "A fresh mixed challenge with a daily mission mindset.";
        case 3: return ru ? u8"Длинный связный текст, прокрутка и тренировка выносливости." : "Long-form typing flow: larger text, scrolling canvas and endurance.";
        case 4: return ru ? u8"Ранг, миссии, слабые клавиши, серия и открытые уроки." : "Review rank, missions, weak keys, lesson unlocks and reset progress.";
        case 5: return ru ? u8"Тема, язык, шрифты, сложность, звук, громкость и сброс прогресса." : "Theme, language, fonts, difficulty, sound profiles and progress reset.";
        case 6: return ru ? u8"Закрыть программу и вернуться на рабочий стол." : "Close Key Sprint and return to desktop.";
        default: return "";
    }
}

bool IsModeOption(int index) {
    return index >= 0 && index <= 3;
}

TypingMode ModeForOption(int index) {
    switch (index) {
        case 1: return TypingMode::Tutorial;
        case 2: return TypingMode::Daily;
        case 3: return TypingMode::Composition;
        case 0:
        default: return TypingMode::Practice;
    }
}

Color OptionAccent(int index, const Theme& theme) {
    return IsModeOption(index) ? GetModeVisualStyle(ModeForOption(index)).Accent : theme.Highlight;
}

const char* LocalRank(const std::string& rank, Language language) {
    if (language != Language::Russian) return rank.c_str();
    if (rank == "Master") return u8"Мастер";
    if (rank == "Swift") return u8"Скоростной";
    if (rank == "Steady") return u8"Уверенный";
    return u8"Новичок";
}

const char* LocalDifficulty(const std::string& difficulty, Language language) {
    if (language != Language::Russian) return difficulty.c_str();
    if (difficulty == "Relaxed") return u8"Легкая";
    if (difficulty == "Strict") return u8"Строгая";
    return u8"Нормальная";
}

struct MainMenuLayout {
    float cardX = 320.0f;
    float cardY = 48.0f;
    float cardWidth = 640.0f;
    float cardHeight = 624.0f;
    float contentX = 390.0f;
    float contentWidth = 500.0f;
    float statusY = 204.0f;
    float statusFontSize = 15.0f;
    float menuItemSpacing = 39.0f;
    float menuItemHeight = 38.0f;
    float optionTextOffset = 8.0f;
    float detailHeight = 62.0f;
    int optionCount = 7;

    float StatusBottom() const {
        return statusY + statusFontSize;
    }

    float MenuBlockHeight() const {
        return (static_cast<float>(optionCount) - 1.0f) * menuItemSpacing + menuItemHeight;
    }

    float MenuGap() const {
        // Equal breathing room above the first item and below the last item.
        return 22.0f;
    }

    float MenuTop() const {
        return StatusBottom() + MenuGap();
    }

    float MenuBottom() const {
        return MenuTop() + MenuBlockHeight();
    }

    float DetailTop() const {
        return MenuBottom() + MenuGap();
    }

    float FooterY() const {
        return DetailTop() + detailHeight + 12.0f;
    }

    float OptionTextY(int index) const {
        return MenuTop() + optionTextOffset + static_cast<float>(index) * menuItemSpacing;
    }

    float HighlightY(int index) const {
        return MenuTop() + static_cast<float>(index) * menuItemSpacing;
    }

    Rectangle CardRect() const {
        return { cardX, cardY, cardWidth, cardHeight };
    }
};

const MainMenuLayout& GetLayout() {
    static const MainMenuLayout layout;
    return layout;
}

void DrawWrappedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, float spacing, Color color) {
    std::istringstream words(text);
    std::string word;
    std::string line;
    float y = position.y;

    while (words >> word) {
        const std::string candidate = line.empty() ? word : line + " " + word;
        const float width = MeasureTextEx(font, candidate.c_str(), fontSize, spacing).x;

        if (width > maxWidth && !line.empty()) {
            const float scale = game->GetUiScale();
            DrawTextEx(font, line.c_str(), game->ScalePoint({ position.x, y }), fontSize * scale, spacing * scale, color);
            line = word;
            y += fontSize + 6.0f;
        } else {
            line = candidate;
        }
    }

    if (!line.empty()) {
        const float scale = game->GetUiScale();
        DrawTextEx(font, line.c_str(), game->ScalePoint({ position.x, y }), fontSize * scale, spacing * scale, color);
    }
}

void DrawBrandTitle(Game* game, Font font, const Theme& theme, float centerX, float y) {
    const float fontSize = 52.0f;
    const float spacing = 0.0f;
    const float wordGap = 12.0f;
    const Vector2 keySize = MeasureTextEx(font, "Key", fontSize, spacing);
    const Vector2 sprintSize = MeasureTextEx(font, "Sprint", fontSize, spacing);
    const float totalWidth = keySize.x + wordGap + sprintSize.x;
    const float startX = centerX - totalWidth * 0.5f;

    DrawTextEx(font, "Key", game->ScalePoint({ startX + 2.0f, y + 2.0f }), fontSize * game->GetUiScale(), spacing, Fade(theme.Background, 0.34f));
    DrawTextEx(font, "Sprint", game->ScalePoint({ startX + keySize.x + wordGap + 2.0f, y + 2.0f }), fontSize * game->GetUiScale(), spacing, Fade(theme.Highlight, 0.18f));
    DrawTextEx(font, "Key", game->ScalePoint({ startX, y }), fontSize * game->GetUiScale(), spacing, theme.Title);
    DrawTextEx(font, "Sprint", game->ScalePoint({ startX + keySize.x + wordGap, y }), fontSize * game->GetUiScale(), spacing, theme.Highlight);
}

void DrawFittedText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, float spacing, Color color) {
    float adjustedSize = fontSize;
    while (adjustedSize > 11.0f && MeasureTextEx(font, text, adjustedSize, spacing).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), adjustedSize * scale, spacing * scale, color);
}
}

void MainMenuState::Init(Game* game) {
    gamePtr = game;
    highlightY = GetLayout().HighlightY(0);
}

void MainMenuState::HandleInput() {
    bool activateOption = false;
    bool hoveringOption = false;

    const Vector2 mouse = GetMousePosition();
    const MainMenuLayout& layout = GetLayout();
    for (size_t i = 0; i < options.size(); ++i) {
        const Rectangle optionRect = gamePtr->ScaleRect({ layout.contentX, layout.HighlightY(static_cast<int>(i)), layout.contentWidth, layout.menuItemHeight - 2.0f });
        if (CheckCollisionPointRec(mouse, optionRect)) {
            selectedOption = static_cast<int>(i);
            hoveringOption = true;
            activateOption = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
            break;
        }
    }
    SetMouseCursor(hoveringOption ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % static_cast<int>(options.size());
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption + static_cast<int>(options.size()) - 1) % static_cast<int>(options.size());
    } else if (IsKeyPressed(KEY_ENTER) || activateOption) {
        if (selectedOption == 0) {
            gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Practice));
        } else if (selectedOption == 1) {
            gamePtr->ChangeState(std::make_shared<LessonSelectState>());
        } else if (selectedOption == 2) {
            gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Daily));
        } else if (selectedOption == 3) {
            gamePtr->ChangeState(std::make_shared<TypingState>(TypingMode::Composition));
        } else if (selectedOption == 4) {
            gamePtr->ChangeState(std::make_shared<ProgressState>());
        } else if (selectedOption == 5) {
            gamePtr->ChangeState(std::make_shared<SettingsState>());
        } else {
            gamePtr->Quit();
        }
    }
}

void MainMenuState::Update(float deltaTime) {
    menuTime += deltaTime;
    highlightY = GetLayout().HighlightY(selectedOption);
}

void MainMenuState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetUiFont();
    Font titleFont = gamePtr->GetTypingFontByIndex(2);
    const float scale = gamePtr->GetUiScale();
    const MainMenuLayout& layout = GetLayout();

    const float glow = (std::sin(menuTime * 2.1f) + 1.0f) * 0.5f;
    const Color selectedAccent = OptionAccent(selectedOption, theme);
    const Rectangle card = gamePtr->ScaleRect(layout.CardRect());
    DrawRectangleRounded(card, 0.09f, 16, Fade(theme.Panel, 0.82f));
    DrawRectangleRoundedLines(card, 0.09f, 16, Fade(theme.PanelBorder, 0.80f));

    DrawBrandTitle(gamePtr, titleFont, theme, 640.0f, 104.0f);
    DrawTextEx(font, gamePtr->GetLanguage() == Language::Russian ? u8"Тренажер скорости печати" : "Typing speed trainer", gamePtr->ScalePoint({ 414.0f, 178.0f }), 20.0f * scale, 1.0f * scale, theme.TextDefault);
    const Language lang = gamePtr->GetLanguage();
    const std::string rank = gamePtr->GetProgress().GetRankLabel();
    const std::string difficulty = gamePtr->GetProgress().GetDifficultyLabel();
    DrawTextEx(font, TextFormat(lang == Language::Russian ? u8"%s | Ранг %s | %s | Рекорд %.0f WPM" : "%s | %s Rank | %s | Best %.0f WPM",
        LessonLibrary::GetLanguageLabel(lang).c_str(),
        LocalRank(rank, lang),
        LocalDifficulty(difficulty, lang),
        gamePtr->GetProgress().GetBestWpm()),
        gamePtr->ScalePoint({ 384.0f, 215.0f }), 15.0f * scale, 1.0f * scale, theme.TextDefault);

    const Rectangle highlight = gamePtr->ScaleRect({ layout.contentX, highlightY, layout.contentWidth, 38.0f });
    DrawRectangleRounded(highlight, 0.35f, 10, Fade(selectedAccent, 0.17f));
    DrawRectangleRoundedLines(highlight, 0.35f, 10, Fade(selectedAccent, 0.52f));

    const Rectangle detail = gamePtr->ScaleRect({ layout.contentX, layout.DetailTop(), layout.contentWidth, layout.detailHeight });
    DrawRectangleRounded(detail, 0.18f, 10, Fade(IsModeOption(selectedOption) ? selectedAccent : theme.PanelBorder, IsModeOption(selectedOption) ? 0.12f : 0.18f));
    DrawRectangleRoundedLines(detail, 0.18f, 10, Fade(selectedAccent, 0.38f));

    if (IsModeOption(selectedOption)) {
        const ModeVisualStyle modeStyle = GetModeVisualStyle(ModeForOption(selectedOption));
        const Rectangle badge = gamePtr->ScaleRect({ layout.contentX + layout.contentWidth - 156.0f, layout.DetailTop() + 13.0f, 126.0f, 28.0f });
        DrawRectangleRounded(badge, 0.42f, 10, Fade(modeStyle.Accent, 0.18f));
        DrawRectangleRoundedLines(badge, 0.42f, 10, Fade(modeStyle.Accent, 0.42f));
        DrawFittedText(
            gamePtr,
            font,
            gamePtr->GetLanguage() == Language::Russian ? modeStyle.ToneRu : modeStyle.ToneEn,
            { layout.contentX + layout.contentWidth - 142.0f, layout.DetailTop() + 20.0f },
            98.0f,
            12.0f,
            0.0f,
            theme.TextDefault);
    }

    DrawWrappedText(
        gamePtr,
        font,
        LocalizedDescription(selectedOption, gamePtr->GetLanguage()),
        { layout.contentX + 30.0f, layout.DetailTop() + 15.0f },
        IsModeOption(selectedOption) ? layout.contentWidth - 200.0f : layout.contentWidth - 60.0f,
        14.0f,
        0.0f,
        theme.TextDefault);

    for (size_t i = 0; i < options.size(); ++i) {
        const float y = layout.OptionTextY(static_cast<int>(i));
        const bool modeOption = IsModeOption(static_cast<int>(i));
        const ModeVisualStyle modeStyle = GetModeVisualStyle(ModeForOption(static_cast<int>(i)));
        const Color accent = modeOption ? modeStyle.Accent : theme.Highlight;
        const Color color = selectedOption == static_cast<int>(i) ? theme.TextCorrect : theme.TextDefault;
        const float dotPulse = selectedOption == static_cast<int>(i) ? glow : 0.15f;
        if (modeOption) {
            DrawRectangleRounded(
                gamePtr->ScaleRect({ layout.contentX + 15.0f, y + 5.0f, 5.0f, 25.0f }),
                0.80f,
                6,
                Fade(accent, selectedOption == static_cast<int>(i) ? 0.78f : 0.36f));
        }
        DrawCircleV(gamePtr->ScalePoint({ layout.contentX + 30.0f, y + 14.0f }), (5.0f + dotPulse * 3.0f) * scale, selectedOption == static_cast<int>(i) ? accent : Fade(modeOption ? accent : theme.TextDefault, modeOption ? 0.45f : 0.35f));
        DrawTextEx(font, LocalizedOption(static_cast<int>(i), gamePtr->GetLanguage()), gamePtr->ScalePoint({ layout.contentX + 65.0f, y }), 21.0f * scale, 0.0f, color);
    }

    DrawTextEx(font, gamePtr->GetLanguage() == Language::Russian ? u8"Стрелки/WASD, Enter или мышь" : "Use Up/Down, Enter or Mouse", gamePtr->ScalePoint({ 420.0f, layout.FooterY() }), 16.0f * scale, 1.0f * scale, theme.TextDefault);
}
