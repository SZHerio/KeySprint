#include "TypingState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include "../core/LessonLibrary.h"
#include "../core/TextUtils.h"
#include "MainMenuState.h"
#include "ResultsState.h"

namespace {
constexpr float TextFontSize = 30.0f;
constexpr float UiSpacing = 1.0f;

Color FadeColor(Color color, float alpha) {
    color.a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);
    return color;
}

Rectangle Inflate(Rectangle rect, float amount) {
    return { rect.x - amount, rect.y - amount, rect.width + amount * 2.0f, rect.height + amount * 2.0f };
}

void DrawTextScene(Game* game, Font font, const char* text, Vector2 position, float fontSize, float spacing, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, spacing * scale, color);
}

void DrawRoundedScene(Game* game, Rectangle rect, float roundness, int segments, Color color) {
    DrawRectangleRounded(game->ScaleRect(rect), roundness, segments, color);
}

void DrawRoundedLinesScene(Game* game, Rectangle rect, float roundness, int segments, Color color) {
    DrawRectangleRoundedLines(game->ScaleRect(rect), roundness, segments, color);
}

void DrawRectScene(Game* game, Rectangle rect, Color color) {
    DrawRectangleRec(game->ScaleRect(rect), color);
}
}

TypingState::TypingState(TypingMode mode) : mode(mode) {
}

void TypingState::BuildKeyboardModel() {
    keyboardKeys.clear();

    auto addRow = [this](int row, const std::string& keys) {
        for (int key : Utf8ToCodepoints(keys)) {
            const FingerType finger = GetFingerForKey(key);
            const bool leftHand = finger == FingerType::LeftPinky || finger == FingerType::LeftRing ||
                finger == FingerType::LeftMiddle || finger == FingerType::LeftIndex || finger == FingerType::LeftThumb;

            keyboardKeys.push_back({
                key,
                CodepointToUtf8(key),
                row,
                1.0f,
                finger,
                leftHand ? HandSide::Left : HandSide::Right
            });
        }
    };

    if (language == Language::Russian) {
        addRow(0, u8"йцукенгшщзх");
        addRow(1, u8"фывапролджэ");
        addRow(2, u8"ячсмитьбю");
    } else {
        addRow(0, "qwertyuiop");
        addRow(1, "asdfghjkl;");
        addRow(2, "zxcvbnm");
    }

    keyboardKeys.push_back({ ' ', "SPACE", 3, 5.2f, FingerType::RightThumb, HandSide::Both });
}

void TypingState::CalculateLayout(Font font, float fontSize) {
    const auto& target = logic.GetTargetCodepoints();
    charPositions.clear();
    
    const float startX = 80.0f;
    const float startY = mode == TypingMode::Tutorial ? 150.0f : 205.0f;
    const float maxX = Game::VirtualWidth - 80.0f;
    const float lineHeight = fontSize + 18.0f;
    float currentX = startX;
    float currentY = startY;
    float spaceWidth = MeasureTextEx(font, " ", fontSize, UiSpacing).x;
    
    size_t i = 0;
    while (i < target.size()) {
        size_t wordEnd = i;
        while (wordEnd < target.size() && target[wordEnd] != ' ') {
            ++wordEnd;
        }
        
        std::vector<int> wordCodepoints(target.begin() + i, target.begin() + wordEnd);
        std::string word = CodepointsToUtf8(wordCodepoints);
        float wordWidth = MeasureTextEx(font, word.c_str(), fontSize, UiSpacing).x;
        
        if (currentX > startX && currentX + wordWidth > maxX) {
            currentX = startX;
            currentY += lineHeight;
        }
        
        for (size_t j = i; j < wordEnd; ++j) {
            charPositions.push_back({ currentX, currentY });
            const std::string glyph = CodepointToUtf8(target[j]);
            currentX += MeasureTextEx(font, glyph.c_str(), fontSize, UiSpacing).x + UiSpacing;
        }
        
        if (wordEnd < target.size()) {
            charPositions.push_back({ currentX, currentY });
            currentX += spaceWidth + UiSpacing;
        }
        
        i = wordEnd + 1;
    }
    
    charPositions.push_back({ currentX, currentY });
}

void TypingState::Init(Game* game) {
    gamePtr = game;
    language = gamePtr->GetLanguage();
    BuildKeyboardModel();

    if (mode == TypingMode::Tutorial) {
        const auto& lessons = LessonLibrary::GetLessons(language);
        lessonId = gamePtr->GetProgress().GetCurrentLesson(language, static_cast<int>(lessons.size()));
        const Lesson lesson = LessonLibrary::BuildAdaptiveLesson(language, lessonId, gamePtr->GetProgress().GetWeakKeys());
        lessonTitle = lesson.title;
        lessonDescription = lesson.description;
        logic.StartCustomTest(lesson.text);
    } else {
        if (language == Language::Russian) {
            logic.StartCustomTest(u8"мама дом школа код урок рука палец клавиша текст скорость точность фокус ритм");
        } else {
            logic.StartNewTest(24);
        }
        lessonId = -1;
        lessonTitle = "Practice";
        lessonDescription = "Free typing test";
    }
    
    Font font = gamePtr->GetFont();
    float fontSize = TextFontSize;
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
    pulseTime += deltaTime;

    // Плавность каретки
    size_t typedLen = logic.GetTypedCodepoints().size();
    if (typedLen < charPositions.size()) {
        Vector2 targetPos = charPositions[typedLen];
        // Lerp
        caretX += (targetPos.x - caretX) * 20.0f * deltaTime;
        caretY += (targetPos.y - caretY) * 20.0f * deltaTime;
    }

    if (logic.IsFinished()) {
        gamePtr->ChangeState(std::make_shared<ResultsState>(
            logic.GetWPM(),
            logic.GetAccuracy(),
            mode,
            language,
            lessonId,
            lessonTitle,
            logic.GetMistakeCountsUtf8()
        ));
    }
}

int TypingState::GetNextExpectedChar() const {
    const auto& target = logic.GetTargetCodepoints();
    const auto& typed = logic.GetTypedCodepoints();
    if (typed.size() >= target.size()) {
        return 0;
    }

    return ToLowerCodepoint(target[typed.size()]);
}

FingerType TypingState::GetFingerForKey(int key) const {
    switch (ToLowerCodepoint(key)) {
        case 'q':
        case 0x0439: // й
        case 'a':
        case 0x0444: // ф
        case 'z':
        case 0x044f: // я
            return FingerType::LeftPinky;
        case 'w':
        case 0x0446: // ц
        case 's':
        case 0x044b: // ы
        case 'x':
        case 0x0447: // ч
            return FingerType::LeftRing;
        case 'e':
        case 0x0443: // у
        case 'd':
        case 0x0432: // в
        case 'c':
        case 0x0441: // с
            return FingerType::LeftMiddle;
        case 'r':
        case 0x043a: // к
        case 't':
        case 0x0435: // е
        case 'f':
        case 0x0430: // а
        case 'g':
        case 0x043f: // п
        case 'v':
        case 0x043c: // м
        case 'b':
        case 0x0438: // и
            return FingerType::LeftIndex;
        case ' ':
            return FingerType::RightThumb;
        case 'y':
        case 0x043d: // н
        case 'u':
        case 0x0433: // г
        case 'h':
        case 0x0440: // р
        case 'j':
        case 0x043e: // о
        case 'n':
        case 0x0442: // т
        case 'm':
        case 0x044c: // ь
            return FingerType::RightIndex;
        case 'i':
        case 0x0448: // ш
        case 'k':
        case 0x043b: // л
            return FingerType::RightMiddle;
        case 'o':
        case 0x0449: // щ
        case 'l':
        case 0x0434: // д
            return FingerType::RightRing;
        case 'p':
        case 0x0437: // з
        case 0x0445: // х
        case 0x044a: // ъ
        case ';':
        case 0x0436: // ж
        case 0x044d: // э
        case 0x0431: // б
        case 0x044e: // ю
            return FingerType::RightPinky;
        default:
            return FingerType::None;
    }
}

Color TypingState::GetFingerColor(FingerType finger, const Theme& theme) const {
    switch (finger) {
        case FingerType::LeftPinky:
        case FingerType::RightPinky:
            return theme.Fingers.Pinky;
        case FingerType::LeftRing:
        case FingerType::RightRing:
            return theme.Fingers.Ring;
        case FingerType::LeftMiddle:
        case FingerType::RightMiddle:
            return theme.Fingers.Middle;
        case FingerType::LeftIndex:
        case FingerType::RightIndex:
            return theme.Fingers.Index;
        case FingerType::LeftThumb:
        case FingerType::RightThumb:
            return theme.Fingers.Thumb;
        default:
            return theme.TextDefault;
    }
}

void TypingState::DrawVirtualKeyboard(Font font, const Theme& theme) {
    const int nextChar = GetNextExpectedChar();
    const float keySize = 42.0f;
    const float gap = 7.0f;
    const float startY = 512.0f;
    const float pulse = (std::sin(pulseTime * 6.0f) + 1.0f) * 0.5f;

    DrawTextScene(gamePtr, font, "Keyboard color = finger to use", { 80.0f, 502.0f }, 17.0f, UiSpacing, theme.TextDefault);

    for (int row = 0; row <= 3; ++row) {
        std::vector<const KeyLayout*> rowKeys;
        for (const KeyLayout& key : keyboardKeys) {
            if (key.row == row) {
                rowKeys.push_back(&key);
            }
        }

        float totalWidth = 0.0f;
        for (const KeyLayout* key : rowKeys) {
            totalWidth += key->width * keySize;
        }
        if (!rowKeys.empty()) {
            totalWidth += static_cast<float>(rowKeys.size() - 1) * gap;
        }

        float x = (Game::VirtualWidth - totalWidth) * 0.5f;
        const float y = startY + row * (keySize + gap);

        for (const KeyLayout* key : rowKeys) {
            const bool isNext = key->key == nextChar;
            const Color fingerColor = GetFingerColor(key->finger, theme);
            Rectangle keyRect = { x, y, key->width * keySize, keySize };

            if (isNext) {
                DrawRoundedScene(gamePtr, Inflate(keyRect, 4.0f + pulse * 3.0f), 0.25f, 8, FadeColor(fingerColor, 0.28f));
            }

            DrawRoundedScene(gamePtr, keyRect, 0.22f, 8, FadeColor(fingerColor, isNext ? 0.80f : 0.34f));
            DrawRoundedLinesScene(gamePtr, keyRect, 0.22f, 8, FadeColor(isNext ? theme.TextCorrect : theme.PanelBorder, isNext ? 0.95f : 0.60f));

            const float labelSize = key->key == ' ' ? 15.0f : (language == Language::Russian ? 16.0f : 18.0f);
            const Vector2 textSize = MeasureTextEx(font, key->label.c_str(), labelSize, UiSpacing);
            DrawTextScene(
                gamePtr,
                font,
                key->label.c_str(),
                { keyRect.x + (keyRect.width - textSize.x) * 0.5f, keyRect.y + (keyRect.height - labelSize) * 0.5f - 2.0f },
                labelSize,
                UiSpacing,
                isNext ? theme.TextCorrect : theme.TextDefault
            );

            x += keyRect.width + gap;
        }
    }
}

void TypingState::DrawHandsGuide(Font font, const Theme& theme) {
    const FingerType activeFinger = GetFingerForKey(GetNextExpectedChar());
    const float pulse = (std::sin(pulseTime * 6.0f) + 1.0f) * 0.5f;

    auto drawFinger = [&](Rectangle rect, FingerType finger, const char* label) {
        const bool active = activeFinger == finger;
        const Color color = GetFingerColor(finger, theme);

        if (active) {
            DrawRoundedScene(gamePtr, Inflate(rect, 5.0f + pulse * 4.0f), 0.45f, 12, FadeColor(color, 0.28f));
        }

        DrawRoundedScene(gamePtr, rect, 0.45f, 12, FadeColor(color, active ? 0.88f : 0.46f));
        DrawRoundedLinesScene(gamePtr, rect, 0.45f, 12, FadeColor(active ? theme.TextCorrect : theme.PanelBorder, active ? 0.95f : 0.45f));

        const Vector2 labelSize = MeasureTextEx(font, label, 13.0f, UiSpacing);
        DrawTextScene(gamePtr, font, label, { rect.x + (rect.width - labelSize.x) * 0.5f, rect.y + rect.height + 5.0f }, 13.0f, UiSpacing, theme.TextDefault);
    };

    auto drawHand = [&](float x, const char* title, bool left) {
        DrawTextScene(gamePtr, font, title, { x + 56.0f, 384.0f }, 16.0f, UiSpacing, theme.TextDefault);
        DrawRoundedScene(gamePtr, { x + 25.0f, 455.0f, 190.0f, 66.0f }, 0.32f, 12, FadeColor(theme.PanelBorder, 0.28f));

        if (left) {
            drawFinger({ x + 15.0f, 421.0f, 28.0f, 62.0f }, FingerType::LeftPinky, "P");
            drawFinger({ x + 56.0f, 410.0f, 30.0f, 74.0f }, FingerType::LeftRing, "R");
            drawFinger({ x + 99.0f, 402.0f, 30.0f, 82.0f }, FingerType::LeftMiddle, "M");
            drawFinger({ x + 142.0f, 414.0f, 32.0f, 70.0f }, FingerType::LeftIndex, "I");
            drawFinger({ x + 166.0f, 486.0f, 62.0f, 30.0f }, FingerType::LeftThumb, "T");
        } else {
            drawFinger({ x + 178.0f, 421.0f, 28.0f, 62.0f }, FingerType::RightPinky, "P");
            drawFinger({ x + 135.0f, 410.0f, 30.0f, 74.0f }, FingerType::RightRing, "R");
            drawFinger({ x + 92.0f, 402.0f, 30.0f, 82.0f }, FingerType::RightMiddle, "M");
            drawFinger({ x + 47.0f, 414.0f, 32.0f, 70.0f }, FingerType::RightIndex, "I");
            drawFinger({ x + 0.0f, 486.0f, 62.0f, 30.0f }, FingerType::RightThumb, "T");
        }
    };

    DrawTextScene(gamePtr, font, "Finger guide", { 80.0f, 360.0f }, 22.0f, UiSpacing, theme.Title);
    drawHand(350.0f, "LEFT HAND", true);
    drawHand(705.0f, "RIGHT HAND", false);
}

void TypingState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    float fontSize = TextFontSize;

    const auto& target = logic.GetTargetCodepoints();
    const auto& typed = logic.GetTypedCodepoints();

    DrawRoundedScene(gamePtr, { 60.0f, 28.0f, 1160.0f, 96.0f }, 0.18f, 12, FadeColor(theme.Panel, 0.70f));
    DrawRoundedLinesScene(gamePtr, { 60.0f, 28.0f, 1160.0f, 96.0f }, 0.18f, 12, FadeColor(theme.PanelBorder, 0.75f));

    for (size_t i = 0; i < target.size(); ++i) {
        Color color = theme.TextDefault;
        
        if (i < typed.size()) {
            if (ToLowerCodepoint(typed[i]) == ToLowerCodepoint(target[i])) {
                color = theme.TextCorrect;
            } else {
                color = theme.TextError;
                
                // Рисуем подчеркивание для ошибок для лучшей читаемости
                DrawRectScene(gamePtr, { charPositions[i].x, charPositions[i].y + fontSize + 3.0f, MeasureTextEx(font, "A", fontSize, UiSpacing).x, 3.0f }, theme.TextError);
            }
        }

        const std::string glyph = CodepointToUtf8(target[i]);
        DrawTextScene(gamePtr, font, glyph.c_str(), charPositions[i], fontSize, UiSpacing, color);
    }
    
    // Рисуем плавную каретку
    float caretWidth = MeasureTextEx(font, "A", fontSize, UiSpacing).x;
    DrawRoundedScene(gamePtr, { caretX, caretY + fontSize + 4.0f, caretWidth, 4.0f }, 1.0f, 4, theme.Caret);

    // Заголовки (системным шрифтом или тем же, но для UI можно оставить обычный)
    const char* title = mode == TypingMode::Tutorial ? "Tutorial Mode" : "Practice Mode";
    DrawTextScene(gamePtr, font, title, {80.0f, 48.0f}, 34.0f, UiSpacing, theme.Title);
    DrawTextScene(gamePtr, font, TextFormat("%s | %s | ESC Menu", LessonLibrary::GetLanguageLabel(language).c_str(), lessonTitle.c_str()), {80.0f, 90.0f}, 17.0f, UiSpacing, theme.TextDefault);

    // Live Metrics
    DrawTextScene(gamePtr, font, TextFormat("WPM %.0f", logic.GetWPM()), {930.0f, 52.0f}, 20.0f, UiSpacing, theme.TextCorrect);
    DrawTextScene(gamePtr, font, TextFormat("ACC %.0f%%", logic.GetAccuracy()), {930.0f, 84.0f}, 20.0f, UiSpacing, theme.TextCorrect);

    if (mode == TypingMode::Tutorial) {
        DrawRoundedScene(gamePtr, { 60.0f, 342.0f, 1160.0f, 362.0f }, 0.12f, 12, FadeColor(theme.Panel, 0.66f));
        DrawRoundedLinesScene(gamePtr, { 60.0f, 342.0f, 1160.0f, 362.0f }, 0.12f, 12, FadeColor(theme.PanelBorder, 0.70f));
        DrawHandsGuide(font, theme);
        DrawVirtualKeyboard(font, theme);
    }
}
