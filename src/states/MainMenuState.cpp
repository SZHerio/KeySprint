#include "MainMenuState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include "../core/LessonLibrary.h"
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
        case 5: return ru ? u8"Тема, язык, сложность, звук, громкость и сброс прогресса." : "Theme, language, difficulty, sound profiles and progress reset.";
        case 6: return ru ? u8"Закрыть программу и вернуться на рабочий стол." : "Close KeySprint and return to desktop.";
        default: return "";
    }
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
}

void MainMenuState::Init(Game* game) {
    gamePtr = game;
    highlightY = 232.0f;
}

void MainMenuState::HandleInput() {
    bool activateOption = false;
    bool hoveringOption = false;

    const Vector2 mouse = GetMousePosition();
    for (size_t i = 0; i < options.size(); ++i) {
        const Rectangle optionRect = gamePtr->ScaleRect({ 390.0f, 232.0f + static_cast<float>(i) * 39.0f, 500.0f, 36.0f });
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
    const float targetY = 232.0f + selectedOption * 39.0f;
    highlightY = targetY;
}

void MainMenuState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    const float scale = gamePtr->GetUiScale();

    const float glow = (std::sin(menuTime * 2.1f) + 1.0f) * 0.5f;
    const Rectangle card = gamePtr->ScaleRect({ 310.0f, 70.0f, 660.0f, 585.0f });
    DrawRectangleRounded(card, 0.09f, 16, Fade(theme.Panel, 0.82f));
    DrawRectangleRoundedLines(card, 0.09f, 16, Fade(theme.PanelBorder, 0.80f));

    DrawTextEx(font, "KeySprint", gamePtr->ScalePoint({ 438.0f, 122.0f }), 54.0f * scale, 1.0f * scale, theme.Title);
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

    const Rectangle highlight = gamePtr->ScaleRect({ 390.0f, highlightY, 500.0f, 38.0f });
    DrawRectangleRounded(highlight, 0.35f, 10, Fade(theme.Highlight, 0.18f));
    DrawRectangleRoundedLines(highlight, 0.35f, 10, Fade(theme.Highlight, 0.48f));

    const Rectangle detail = gamePtr->ScaleRect({ 390.0f, 535.0f, 500.0f, 62.0f });
    DrawRectangleRounded(detail, 0.18f, 10, Fade(theme.PanelBorder, 0.18f));
    DrawRectangleRoundedLines(detail, 0.18f, 10, Fade(theme.Highlight, 0.35f));
    DrawWrappedText(gamePtr, font, LocalizedDescription(selectedOption, gamePtr->GetLanguage()), { 420.0f, 550.0f }, 440.0f, 14.0f, 1.0f, theme.TextDefault);

    for (size_t i = 0; i < options.size(); ++i) {
        const float y = 240.0f + static_cast<float>(i) * 39.0f;
        const Color color = selectedOption == static_cast<int>(i) ? theme.TextCorrect : theme.TextDefault;
        const float dotPulse = selectedOption == static_cast<int>(i) ? glow : 0.15f;
        DrawCircleV(gamePtr->ScalePoint({ 420.0f, y + 14.0f }), (5.0f + dotPulse * 3.0f) * scale, selectedOption == static_cast<int>(i) ? theme.Highlight : Fade(theme.TextDefault, 0.35f));
        DrawTextEx(font, LocalizedOption(static_cast<int>(i), gamePtr->GetLanguage()), gamePtr->ScalePoint({ 455.0f, y }), 21.0f * scale, 1.0f * scale, color);
    }

    DrawTextEx(font, gamePtr->GetLanguage() == Language::Russian ? u8"Стрелки/WASD, Enter или мышь" : "Use Up/Down, Enter or Mouse", gamePtr->ScalePoint({ 420.0f, 612.0f }), 16.0f * scale, 1.0f * scale, theme.TextDefault);
}
