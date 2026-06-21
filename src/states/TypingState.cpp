#include "TypingState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include <cctype>
#include "MainMenuState.h"
#include "ResultsState.h"

TypingState::TypingState(TypingMode mode) : mode(mode) {
}

void TypingState::CalculateLayout(Font font, float fontSize) {
    const std::string& target = logic.GetTargetText();
    charPositions.clear();
    
    int startX = 50;
    int startY = 250;
    int currentX = startX;
    int currentY = startY;
    int spaceWidth = MeasureTextEx(font, " ", fontSize, 1).x;
    
    size_t i = 0;
    while (i < target.length()) {
        size_t wordEnd = target.find(' ', i);
        if (wordEnd == std::string::npos) wordEnd = target.length();
        
        std::string word = target.substr(i, wordEnd - i);
        int wordWidth = MeasureTextEx(font, word.c_str(), fontSize, 1).x;
        
        // Wrap
        if (currentX + wordWidth > 800 - 50) {
            currentX = startX;
            currentY += fontSize + 15;
        }
        
        for (size_t j = i; j < wordEnd; ++j) {
            charPositions.push_back({ (float)currentX, (float)currentY });
            char str[2] = { target[j], '\0' };
            currentX += MeasureTextEx(font, str, fontSize, 1).x + 1;
        }
        
        if (wordEnd < target.length()) {
            charPositions.push_back({ (float)currentX, (float)currentY });
            currentX += spaceWidth + 1;
        }
        
        i = wordEnd + 1;
    }
    
    // Position for the final caret after the last character
    charPositions.push_back({ (float)currentX, (float)currentY });
}

void TypingState::Init(Game* game) {
    gamePtr = game;
    if (mode == TypingMode::Tutorial) {
        logic.StartCustomTest("asdf jkl; asdf jkl; fj fj dk dk sl sl aa ss dd ff jj kk ll ;;");
    } else {
        logic.StartNewTest(24);
    }
    
    Font font = gamePtr->GetFont();
    float fontSize = 32.0f;
    CalculateLayout(font, fontSize);
    
    if (!charPositions.empty()) {
        caretX = charPositions[0].x;
        caretY = charPositions[0].y;
    }
}

void TypingState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
        return;
    }

    logic.HandleInput();
}

void TypingState::Update(float deltaTime) {
    logic.Update(deltaTime);

    // Плавность каретки
    size_t typedLen = logic.GetTypedText().length();
    if (typedLen < charPositions.size()) {
        Vector2 targetPos = charPositions[typedLen];
        // Lerp
        caretX += (targetPos.x - caretX) * 20.0f * deltaTime;
        caretY += (targetPos.y - caretY) * 20.0f * deltaTime;
    }

    if (logic.IsFinished()) {
        gamePtr->ChangeState(std::make_shared<ResultsState>(logic.GetWPM(), logic.GetAccuracy(), mode));
    }
}

char TypingState::GetNextExpectedChar() const {
    const std::string& target = logic.GetTargetText();
    const std::string& typed = logic.GetTypedText();
    if (typed.length() >= target.length()) {
        return '\0';
    }

    return static_cast<char>(std::tolower(static_cast<unsigned char>(target[typed.length()])));
}

void TypingState::DrawVirtualKeyboard(Font font, const Theme& theme) {
    const std::vector<std::string> rows = {
        "qwertyuiop",
        "asdfghjkl;",
        "zxcvbnm"
    };
    const std::vector<float> offsets = { 0.0f, 24.0f, 62.0f };

    const char nextChar = GetNextExpectedChar();
    const float keySize = 44.0f;
    const float gap = 8.0f;
    const float startX = 115.0f;
    const float startY = 408.0f;

    DrawTextEx(font, "Home row lesson: use the highlighted key next", {115.0f, 374.0f}, 18, 1, theme.TextDefault);

    for (size_t rowIndex = 0; rowIndex < rows.size(); ++rowIndex) {
        const std::string& row = rows[rowIndex];
        for (size_t keyIndex = 0; keyIndex < row.length(); ++keyIndex) {
            const char key = row[keyIndex];
            const float x = startX + offsets[rowIndex] + static_cast<float>(keyIndex) * (keySize + gap);
            const float y = startY + static_cast<float>(rowIndex) * (keySize + gap);
            const bool isNext = key == nextChar;
            const bool isHomeKey = rowIndex == 1;

            Color fill = Fade(theme.TextDefault, isHomeKey ? 0.20f : 0.10f);
            Color line = Fade(theme.TextDefault, 0.35f);
            Color text = theme.TextDefault;

            if (isNext) {
                fill = Fade(theme.Title, 0.45f);
                line = theme.Caret;
                text = theme.TextCorrect;
            }

            Rectangle keyRect = { x, y, keySize, keySize };
            DrawRectangleRounded(keyRect, 0.25f, 8, fill);
            DrawRectangleRoundedLines(keyRect, 0.25f, 8, line);

            char label[2] = { static_cast<char>(std::toupper(static_cast<unsigned char>(key))), '\0' };
            Vector2 textSize = MeasureTextEx(font, label, 22, 1);
            DrawTextEx(font, label, { x + (keySize - textSize.x) / 2.0f, y + 10.0f }, 22, 1, text);
        }
    }
}

void TypingState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    float fontSize = 32.0f;

    const std::string& target = logic.GetTargetText();
    const std::string& typed = logic.GetTypedText();

    for (size_t i = 0; i < target.length(); ++i) {
        Color color = theme.TextDefault;
        
        if (i < typed.length()) {
            if (typed[i] == target[i]) {
                color = theme.TextCorrect;
            } else {
                color = theme.TextError;
                
                // Рисуем подчеркивание для ошибок для лучшей читаемости
                DrawRectangle(charPositions[i].x, charPositions[i].y + fontSize, MeasureTextEx(font, "A", fontSize, 1).x, 3, theme.TextError);
            }
        }

        char str[2] = { target[i], '\0' };
        DrawTextEx(font, str, charPositions[i], fontSize, 1, color);
    }
    
    // Рисуем плавную каретку
    float caretWidth = MeasureTextEx(font, "A", fontSize, 1).x;
    DrawRectangleRounded({ caretX, caretY + fontSize + 2, caretWidth, 4 }, 1.0f, 4, theme.Caret);

    // Заголовки (системным шрифтом или тем же, но для UI можно оставить обычный)
    const char* title = mode == TypingMode::Tutorial ? "Tutorial Mode" : "Practice Mode";
    DrawTextEx(font, title, {50, 50}, 40, 1, theme.Title);
    DrawTextEx(font, "Press ESC to return to Menu", {50, 100}, 20, 1, theme.TextDefault);

    // Live Metrics
    DrawTextEx(font, TextFormat("WPM: %.0f", logic.GetWPM()), {50, 150}, 20, 1, theme.TextCorrect);
    DrawTextEx(font, TextFormat("Accuracy: %.0f%%", logic.GetAccuracy()), {200, 150}, 20, 1, theme.TextCorrect);

    if (mode == TypingMode::Tutorial) {
        DrawVirtualKeyboard(font, theme);
    }
}
