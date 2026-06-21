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
constexpr Rectangle TextViewport = { 60.0f, 130.0f, 1160.0f, 190.0f };

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
    keyboardKeys.push_back({ '\n', "ENTER", 3, 2.0f, FingerType::RightPinky, HandSide::Right });
}

void TypingState::CalculateLayout(Font font, float fontSize) {
    const auto& target = logic.GetTargetCodepoints();
    charPositions.clear();
    
    const float startX = 80.0f;
    const float startY = 150.0f;
    const float maxX = Game::VirtualWidth - 80.0f;
    const float lineHeight = fontSize + 18.0f;
    float currentX = startX;
    float currentY = startY;
    float spaceWidth = MeasureTextEx(font, " ", fontSize, UiSpacing).x;
    
    size_t i = 0;
    while (i < target.size()) {
        if (target[i] == '\n') {
            charPositions.push_back({ currentX, currentY });
            currentX = startX;
            currentY += lineHeight * 1.45f;
            ++i;
            continue;
        }

        size_t wordEnd = i;
        while (wordEnd < target.size() && target[wordEnd] != ' ' && target[wordEnd] != '\n') {
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
        
        if (wordEnd < target.size() && target[wordEnd] == ' ') {
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
    } else if (mode == TypingMode::Composition) {
        logic.StartCustomTest(BuildCompositionText());
        lessonId = -1;
        lessonTitle = "Composition";
        lessonDescription = "Long-form endurance typing";
    } else {
        if (language == Language::Russian) {
            logic.StartCustomTest(
                u8"мама дом школа код урок рука палец клавиша текст скорость точность фокус ритм. "
                u8"Печатай спокойно и возвращай пальцы на домашний ряд.\n"
                u8"Каждая короткая фраза помогает удерживать внимание на точности."
            );
        } else {
            logic.StartCustomTest(
                "calm focus builds reliable typing speed. keep your fingers near the home row and watch the rhythm.\n"
                "short clear sentences help you train accuracy before you push for a faster result."
            );
        }
        lessonId = -1;
        lessonTitle = "Practice";
        lessonDescription = "Free typing test";
    }
    
    Font font = gamePtr->GetFont();
    float fontSize = GetTextFontSize();
    CalculateLayout(font, fontSize);
    
    if (!charPositions.empty()) {
        caretX = charPositions[0].x;
        caretY = charPositions[0].y;
    }
    textScrollY = 0.0f;
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

    const float targetScroll = std::max(0.0f, caretY - (TextViewport.y + TextViewport.height - 58.0f));
    textScrollY += (targetScroll - textScrollY) * std::min(1.0f, deltaTime * 10.0f);

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

float TypingState::GetTextFontSize() const {
    return mode == TypingMode::Composition ? 23.0f : TextFontSize;
}

const char* TypingState::GetModeTitle() const {
    switch (mode) {
        case TypingMode::Tutorial:
            return "Tutorial Mode";
        case TypingMode::Composition:
            return "Composition Mode";
        case TypingMode::Practice:
        default:
            return "Practice Mode";
    }
}

std::string TypingState::BuildCompositionText() const {
    if (language == Language::Russian) {
        return u8"Слепая печать начинается не со скорости, а с спокойного ритма. "
            u8"Пальцы должны возвращаться на домашний ряд после каждого движения. "
            u8"Когда внимание перестает метаться между клавишами, текст начинает течь ровнее. "
            u8"Ошибки не нужно бояться: каждая ошибка показывает, какой палец просит отдельной тренировки.\n"
            u8"Сохраняй ровное дыхание, не ускоряйся раньше времени и следи за точностью. "
            u8"Хорошая скорость появляется тогда, когда движение становится уверенным и повторяемым. "
            u8"Если абзац закончился, нажми Enter и продолжай новый фрагмент без спешки.\n"
            u8"Длинный текст учит держать внимание дольше нескольких слов. "
            u8"В такой тренировке важны не рекорды, а устойчивость: одинаковая сила нажатий, мягкая посадка пальцев и спокойный взгляд на строку. "
            u8"Когда точность остается высокой на протяжении нескольких абзацев, скорость начинает расти сама.";
    }

    return "Fast typing is not only about speed. It starts with calm rhythm, relaxed hands, "
        "and a clear return to the home row after every movement. When your attention stops "
        "jumping from key to key, the sentence begins to flow naturally. Mistakes are not a "
        "problem: each one shows which finger needs a focused drill.\nKeep your breathing even, "
        "avoid rushing too early, and let accuracy build the speed. A reliable rhythm will "
        "always beat a tense burst of random fast typing. When the paragraph ends, press Enter "
        "and continue with the next thought without breaking your posture.\nLong-form typing "
        "teaches endurance. It asks you to keep the same pressure, the same focus, and the same "
        "soft return to the home row for several connected ideas. If accuracy stays high through "
        "multiple paragraphs, speed becomes a natural result instead of a forced sprint.";
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
    const float handYOffset = -20.0f;

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
        DrawTextScene(gamePtr, font, title, { x + 56.0f, 384.0f + handYOffset }, 16.0f, UiSpacing, theme.TextDefault);
        DrawRoundedScene(gamePtr, { x + 25.0f, 455.0f + handYOffset, 190.0f, 66.0f }, 0.32f, 12, FadeColor(theme.PanelBorder, 0.28f));

        if (left) {
            drawFinger({ x + 15.0f, 421.0f + handYOffset, 28.0f, 62.0f }, FingerType::LeftPinky, "P");
            drawFinger({ x + 56.0f, 410.0f + handYOffset, 30.0f, 74.0f }, FingerType::LeftRing, "R");
            drawFinger({ x + 99.0f, 402.0f + handYOffset, 30.0f, 82.0f }, FingerType::LeftMiddle, "M");
            drawFinger({ x + 142.0f, 414.0f + handYOffset, 32.0f, 70.0f }, FingerType::LeftIndex, "I");
            drawFinger({ x + 166.0f, 486.0f + handYOffset, 62.0f, 30.0f }, FingerType::LeftThumb, "T");
        } else {
            drawFinger({ x + 178.0f, 421.0f + handYOffset, 28.0f, 62.0f }, FingerType::RightPinky, "P");
            drawFinger({ x + 135.0f, 410.0f + handYOffset, 30.0f, 74.0f }, FingerType::RightRing, "R");
            drawFinger({ x + 92.0f, 402.0f + handYOffset, 30.0f, 82.0f }, FingerType::RightMiddle, "M");
            drawFinger({ x + 47.0f, 414.0f + handYOffset, 32.0f, 70.0f }, FingerType::RightIndex, "I");
            drawFinger({ x + 0.0f, 486.0f + handYOffset, 62.0f, 30.0f }, FingerType::RightThumb, "T");
        }
    };

    DrawTextScene(gamePtr, font, "Finger guide", { 80.0f, 365.0f }, 22.0f, UiSpacing, theme.Title);
    drawHand(350.0f, "LEFT HAND", true);
    drawHand(705.0f, "RIGHT HAND", false);
}

void TypingState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    Font font = gamePtr->GetFont();
    float fontSize = GetTextFontSize();

    const auto& target = logic.GetTargetCodepoints();
    const auto& typed = logic.GetTypedCodepoints();

    DrawRoundedScene(gamePtr, { 60.0f, 28.0f, 1160.0f, 96.0f }, 0.18f, 12, FadeColor(theme.Panel, 0.70f));
    DrawRoundedLinesScene(gamePtr, { 60.0f, 28.0f, 1160.0f, 96.0f }, 0.18f, 12, FadeColor(theme.PanelBorder, 0.75f));
    DrawRoundedScene(gamePtr, TextViewport, 0.12f, 12, FadeColor(theme.Panel, 0.54f));
    DrawRoundedLinesScene(gamePtr, TextViewport, 0.12f, 12, FadeColor(theme.PanelBorder, 0.55f));

    BeginScissorScene(gamePtr, TextViewport);
    for (size_t i = 0; i < target.size(); ++i) {
        if (target[i] == '\n') {
            const bool isCurrentNewline = i == typed.size();
            const bool isTyped = i < typed.size();
            const Color enterColor = isTyped
                ? (typed[i] == '\n' ? theme.TextCorrect : theme.TextError)
                : (isCurrentNewline ? theme.Highlight : FadeColor(theme.TextDefault, 0.45f));

            DrawTextScene(gamePtr, font, "ENTER", { charPositions[i].x, charPositions[i].y - textScrollY }, 15.0f, UiSpacing, enterColor);
            continue;
        }

        Color color = theme.TextDefault;
        
        if (i < typed.size()) {
            if (ToLowerCodepoint(typed[i]) == ToLowerCodepoint(target[i])) {
                color = theme.TextCorrect;
            } else {
                color = theme.TextError;
                
                // Рисуем подчеркивание для ошибок для лучшей читаемости
                DrawRectScene(gamePtr, { charPositions[i].x, charPositions[i].y - textScrollY + fontSize + 3.0f, MeasureTextEx(font, "A", fontSize, UiSpacing).x, 3.0f }, theme.TextError);
            }
        }

        const std::string glyph = CodepointToUtf8(target[i]);
        DrawTextScene(gamePtr, font, glyph.c_str(), { charPositions[i].x, charPositions[i].y - textScrollY }, fontSize, UiSpacing, color);
    }
    
    // Рисуем плавную каретку
    float caretWidth = MeasureTextEx(font, "A", fontSize, UiSpacing).x;
    DrawRoundedScene(gamePtr, { caretX, caretY - textScrollY + fontSize + 4.0f, caretWidth, 4.0f }, 1.0f, 4, theme.Caret);
    EndScissorMode();

    // Заголовки (системным шрифтом или тем же, но для UI можно оставить обычный)
    const char* title = GetModeTitle();
    DrawTextScene(gamePtr, font, title, {80.0f, 48.0f}, 34.0f, UiSpacing, theme.Title);
    DrawTextScene(gamePtr, font, TextFormat("%s | %s | ESC Menu", LessonLibrary::GetLanguageLabel(language).c_str(), lessonTitle.c_str()), {80.0f, 90.0f}, 17.0f, UiSpacing, theme.TextDefault);

    // Live Metrics
    DrawTextScene(gamePtr, font, TextFormat("WPM %.0f", logic.GetWPM()), {930.0f, 52.0f}, 20.0f, UiSpacing, theme.TextCorrect);
    DrawTextScene(gamePtr, font, TextFormat("ACC %.0f%%", logic.GetAccuracy()), {930.0f, 84.0f}, 20.0f, UiSpacing, theme.TextCorrect);

    DrawRoundedScene(gamePtr, { 60.0f, 342.0f, 1160.0f, 362.0f }, 0.12f, 12, FadeColor(theme.Panel, 0.66f));
    DrawRoundedLinesScene(gamePtr, { 60.0f, 342.0f, 1160.0f, 362.0f }, 0.12f, 12, FadeColor(theme.PanelBorder, 0.70f));
    DrawHandsGuide(font, theme);
    DrawVirtualKeyboard(font, theme);
}
