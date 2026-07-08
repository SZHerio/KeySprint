#include "TypingState.h"
#include "../core/Game.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include "../core/LessonLibrary.h"
#include "../core/ModeVisual.h"
#include "../core/TextUtils.h"
#include "MainMenuState.h"
#include "ResultsState.h"

namespace {
constexpr float TextFontSize = 27.0f;
constexpr float UiSpacing = 1.0f;
constexpr Rectangle TextViewport = { 60.0f, 128.0f, 1160.0f, 202.0f };

Color FadeColor(Color color, float alpha) {
    color.a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);
    return color;
}

Rectangle Inflate(Rectangle rect, float amount) {
    return { rect.x - amount, rect.y - amount, rect.width + amount * 2.0f, rect.height + amount * 2.0f };
}

float GlyphWidth(Font font, int codepoint, float fontSize, float spacing) {
    const std::string glyph = codepoint == ' ' ? "n" : CodepointToUtf8(codepoint);
    const float measured = MeasureTextEx(font, glyph.c_str(), fontSize, spacing).x;
    const float fallback = MeasureTextEx(font, "n", fontSize, spacing).x * 0.62f;
    return std::max(measured, fallback);
}

float EaseOut(float value) {
    const float clamped = std::clamp(value, 0.0f, 1.0f);
    return 1.0f - (1.0f - clamped) * (1.0f - clamped);
}

void DrawTextScene(Game* game, Font font, const char* text, Vector2 position, float fontSize, float spacing, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, spacing * scale, color);
}

void DrawFittedTextScene(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, float spacing, Color color) {
    float adjustedSize = fontSize;
    while (adjustedSize > 11.0f && MeasureTextEx(font, text, adjustedSize, spacing).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    DrawTextScene(game, font, text, position, adjustedSize, spacing, color);
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

void DrawTypingProgress(Game* game, const Theme& theme, Color accent, Rectangle viewport, float progress, float errorPulse) {
    const Rectangle track = { viewport.x + 22.0f, viewport.y + viewport.height - 18.0f, viewport.width - 44.0f, 5.0f };
    const float fillWidth = std::clamp(progress, 0.0f, 1.0f) * track.width;
    const Color fillColor = errorPulse > 0.0f
        ? FadeColor(theme.TextError, 0.60f + errorPulse * 0.28f)
        : FadeColor(accent, 0.72f);

    DrawRoundedScene(game, track, 1.0f, 8, FadeColor(theme.PanelBorder, 0.20f));
    if (fillWidth > 1.0f) {
        DrawRoundedScene(game, { track.x, track.y, fillWidth, track.height }, 1.0f, 8, fillColor);
        DrawCircleV(game->ScalePoint({ track.x + fillWidth, track.y + track.height * 0.5f }), (4.0f + errorPulse * 3.0f) * game->GetUiScale(), fillColor);
    }
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

TypingState::TypingState(TypingMode mode, int lessonOverride) : mode(mode), lessonOverride(lessonOverride) {
}

float MeasureCodepointRunWidth(Font font, const std::vector<int>& codepoints, size_t from, size_t to, float fontSize, float spacing) {
    float width = 0.0f;
    for (size_t index = from; index < to; ++index) {
        width += MeasureTextEx(font, CodepointToUtf8(codepoints[index]).c_str(), fontSize, spacing).x;
        if (index + 1 < to) {
            width += spacing;
        }
    }
    return width;
}

void TypingState::CalculateLayout(Font font, float fontSize) {
    const auto& target = logic.GetTargetCodepoints();
    charPositions.clear();

    const float startX = TextViewport.x + 20.0f;
    const float maxX = TextViewport.x + TextViewport.width - 20.0f;
    const float startY = 150.0f;
    const float lineHeight = (fontSize + 15.0f) * 0.80f;
    float currentX = startX;
    float currentY = startY;
    const float spaceWidth = MeasureTextEx(font, " ", fontSize, UiSpacing).x;

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

        const float wordWidth = MeasureCodepointRunWidth(font, target, i, wordEnd, fontSize, UiSpacing);
        if (currentX > startX && currentX + wordWidth > maxX) {
            currentX = startX;
            currentY += lineHeight;
        }

        for (size_t j = i; j < wordEnd; ++j) {
            charPositions.push_back({ currentX, currentY });
            currentX += MeasureTextEx(font, CodepointToUtf8(target[j]).c_str(), fontSize, UiSpacing).x;
            if (j + 1 < wordEnd) {
                currentX += UiSpacing;
            }
        }

        if (wordEnd < target.size() && target[wordEnd] == ' ') {
            charPositions.push_back({ currentX, currentY });
            currentX += spaceWidth + UiSpacing;
            i = wordEnd + 1;
        } else {
            i = wordEnd;
        }
    }

    charPositions.push_back({ currentX, currentY });
}

void TypingState::Init(Game* game) {
    gamePtr = game;
    language = gamePtr->GetTypingLanguage();
    uiLanguage = gamePtr->GetLanguage();
    keyboardKeys = BuildKeyboardLayout(language);

    if (mode == TypingMode::Tutorial) {
        const auto& lessons = LessonLibrary::GetLessons(language);
        lessonId = lessonOverride >= 0
            ? std::clamp(lessonOverride, 0, static_cast<int>(lessons.size()) - 1)
            : gamePtr->GetProgress().GetCurrentLesson(language, static_cast<int>(lessons.size()));
        const Lesson lesson = LessonLibrary::BuildAdaptiveLesson(language, lessonId, gamePtr->GetProgress().GetWeakKeys());
        lessonTitle = lesson.title;
        lessonDescription = lesson.description;
        logic.StartCustomTest(lesson.text);
    } else if (mode == TypingMode::Composition) {
        logic.StartCustomTest(LessonLibrary::BuildCompositionText(language));
        lessonId = -1;
        lessonTitle = uiLanguage == Language::Russian ? u8"Сочинение" : "Composition";
        lessonDescription = uiLanguage == Language::Russian ? u8"Большой текст на 10+ минут письма" : "10+ minute long-form typing text";
    } else if (mode == TypingMode::Daily) {
        logic.StartCustomTest(LessonLibrary::BuildDailyChallengeText(language));
        lessonId = -1;
        lessonTitle = uiLanguage == Language::Russian ? u8"Испытание дня" : "Daily Challenge";
        lessonDescription = uiLanguage == Language::Russian ? u8"Одна цель на сегодня" : "One focused challenge for today";
    } else {
        logic.StartCustomTest(LessonLibrary::BuildPracticeText(language));
        lessonId = -1;
        lessonTitle = uiLanguage == Language::Russian ? u8"Практика" : "Practice";
        lessonDescription = uiLanguage == Language::Russian ? u8"Фраза за фразой, Enter после каждой строки" : "Phrase-by-phrase practice, press Enter after each line";
    }
    
    Font font = gamePtr->GetTypingTextFont();
    float fontSize = GetTextFontSize();
    CalculateLayout(font, fontSize);
    
    if (!charPositions.empty()) {
        caretX = charPositions[0].x;
        caretY = charPositions[0].y;
    }
    textScrollY = 0.0f;
    progressFill = 0.0f;
    correctFeedback = 0.0f;
    errorFeedback = 0.0f;
    caretImpact = 0.0f;
    visualTypedLength = 0;
    lastFeedbackIndex = 0;
    lastFeedbackWasError = false;
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
    const auto& target = logic.GetTargetCodepoints();
    const auto& typed = logic.GetTypedCodepoints();
    const size_t typedLen = typed.size();

    if (typedLen != visualTypedLength) {
        if (typedLen > visualTypedLength && typedLen > 0 && typedLen - 1 < target.size()) {
            lastFeedbackIndex = typedLen - 1;
            lastFeedbackWasError = ToLowerCodepoint(typed[lastFeedbackIndex]) != ToLowerCodepoint(target[lastFeedbackIndex]);
            correctFeedback = lastFeedbackWasError ? 0.0f : 1.0f;
            errorFeedback = lastFeedbackWasError ? 1.0f : 0.0f;
            caretImpact = 1.0f;
        }
        visualTypedLength = typedLen;
    }

    correctFeedback = std::max(0.0f, correctFeedback - deltaTime * 3.0f);
    errorFeedback = std::max(0.0f, errorFeedback - deltaTime * 2.8f);
    caretImpact = std::max(0.0f, caretImpact - deltaTime * 4.6f);

    const float targetProgress = target.empty() ? 0.0f : static_cast<float>(typedLen) / static_cast<float>(target.size());
    progressFill += (targetProgress - progressFill) * std::min(1.0f, deltaTime * 9.0f);

    if (typedLen < charPositions.size()) {
        Vector2 targetPos = charPositions[typedLen];
        // Lerp
        caretX += (targetPos.x - caretX) * 20.0f * deltaTime;
        caretY += (targetPos.y - caretY) * 20.0f * deltaTime;
    }

    const float targetScroll = std::max(0.0f, caretY - (TextViewport.y + TextViewport.height - 62.0f));
    textScrollY += (targetScroll - textScrollY) * std::min(1.0f, deltaTime * 10.0f);

    if (logic.IsFinished()) {
        gamePtr->ChangeState(std::make_shared<ResultsState>(
            logic.GetWPM(),
            logic.GetAccuracy(),
            mode,
            language,
            lessonId,
            lessonTitle,
            logic.GetBestStreak(),
            logic.GetMistakeCountsUtf8()
        ));
    }
}

float TypingState::GetTextFontSize() const {
    return mode == TypingMode::Composition ? 22.0f : TextFontSize;
}

const char* TypingState::GetModeTitle() const {
    switch (mode) {
        case TypingMode::Tutorial:
            return uiLanguage == Language::Russian ? u8"Режим обучения" : "Tutorial Mode";
        case TypingMode::Composition:
            return uiLanguage == Language::Russian ? u8"Режим сочинения" : "Composition Mode";
        case TypingMode::Daily:
            return uiLanguage == Language::Russian ? u8"Испытание дня" : "Daily Challenge";
        case TypingMode::Practice:
        default:
            return uiLanguage == Language::Russian ? u8"Режим практики" : "Practice Mode";
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

void TypingState::DrawVirtualKeyboard(Font font, const Theme& theme) {
    const int nextChar = GetNextExpectedChar();
    const float keyboardScale = 1.02f;
    const float keySize = 34.0f * keyboardScale;
    const float gap = 5.0f * keyboardScale;
    const float startY = 506.0f;
    const float pulse = (std::sin(pulseTime * 6.0f) + 1.0f) * 0.5f;

    for (int row = 0; row <= 4; ++row) {
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

            const float labelSize = key->row == 0 ? 13.0f : (key->key == ' ' ? 12.0f : (language == Language::Russian ? 14.0f : 15.0f));
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
    const bool spaceActive = GetNextExpectedChar() == ' ';
    const float pulse = (std::sin(pulseTime * 6.0f) + 1.0f) * 0.5f;

    DrawTextScene(gamePtr, font, uiLanguage == Language::Russian ? u8"Подсказка" : "Hint", { 80.0f, 356.0f }, 19.0f, UiSpacing, theme.Title);

    auto drawFinger = [&](Rectangle rect, FingerType finger, const char* label) {
        const bool active = activeFinger == finger ||
            (spaceActive && (finger == FingerType::LeftThumb || finger == FingerType::RightThumb));
        const Color color = GetFingerColor(finger, theme);

        if (active) {
            DrawRoundedScene(gamePtr, Inflate(rect, 5.0f + pulse * 4.0f), 0.45f, 12, FadeColor(color, 0.28f));
        }

        DrawRoundedScene(gamePtr, rect, 0.45f, 12, FadeColor(color, active ? 0.88f : 0.46f));
        DrawRoundedLinesScene(gamePtr, rect, 0.45f, 12, FadeColor(active ? theme.TextCorrect : theme.PanelBorder, active ? 0.95f : 0.45f));

        const Vector2 labelSize = MeasureTextEx(font, label, 10.0f, UiSpacing);
        const bool thumb = rect.height <= 30.0f;
        const float labelY = thumb ? rect.y + (rect.height - 10.0f) * 0.5f : rect.y + rect.height - 18.0f;
        DrawTextScene(gamePtr, font, label, { rect.x + (rect.width - labelSize.x) * 0.5f, labelY }, 10.0f, UiSpacing, thumb ? theme.TextCorrect : FadeColor(theme.TextDefault, 0.72f));
    };

    auto drawHand = [&](float x, bool left) {
        constexpr float handLift = 30.0f;
        DrawRoundedScene(gamePtr, { x + 25.0f, 450.0f - handLift, 190.0f, 50.0f }, 0.32f, 12, FadeColor(theme.PanelBorder, 0.24f));

        if (left) {
            drawFinger({ x + 15.0f, 410.0f - handLift, 28.0f, 62.0f }, FingerType::LeftPinky, "P");
            drawFinger({ x + 56.0f, 399.0f - handLift, 30.0f, 74.0f }, FingerType::LeftRing, "R");
            drawFinger({ x + 99.0f, 391.0f - handLift, 30.0f, 82.0f }, FingerType::LeftMiddle, "M");
            drawFinger({ x + 142.0f, 403.0f - handLift, 32.0f, 70.0f }, FingerType::LeftIndex, "I");
            drawFinger({ x + 166.0f, 478.0f - handLift, 62.0f, 26.0f }, FingerType::LeftThumb, "T");
        } else {
            drawFinger({ x + 178.0f, 410.0f - handLift, 28.0f, 62.0f }, FingerType::RightPinky, "P");
            drawFinger({ x + 135.0f, 399.0f - handLift, 30.0f, 74.0f }, FingerType::RightRing, "R");
            drawFinger({ x + 92.0f, 391.0f - handLift, 30.0f, 82.0f }, FingerType::RightMiddle, "M");
            drawFinger({ x + 47.0f, 403.0f - handLift, 32.0f, 70.0f }, FingerType::RightIndex, "I");
            drawFinger({ x + 0.0f, 478.0f - handLift, 62.0f, 26.0f }, FingerType::RightThumb, "T");
        }
    };

    drawHand(350.0f, true);
    drawHand(705.0f, false);
}

void TypingState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    const ModeVisualStyle modeStyle = GetModeVisualStyle(mode);
    Font textFont = gamePtr->GetTypingTextFont();
    Font keyboardFont = gamePtr->GetKeyboardFont();
    Font uiFont = gamePtr->GetUiFont();
    const float scale = gamePtr->GetUiScale();
    float fontSize = GetTextFontSize();

    const auto& target = logic.GetTargetCodepoints();
    const auto& typed = logic.GetTypedCodepoints();

    DrawRoundedScene(gamePtr, { 60.0f, 28.0f, 1160.0f, 96.0f }, 0.18f, 12, FadeColor(theme.Panel, 0.70f));
    DrawRoundedLinesScene(gamePtr, { 60.0f, 28.0f, 1160.0f, 96.0f }, 0.18f, 12, FadeColor(modeStyle.Accent, 0.50f));
    DrawRoundedScene(gamePtr, { 78.0f, 48.0f, 7.0f, 56.0f }, 0.80f, 8, FadeColor(modeStyle.Accent, 0.58f));
    DrawRoundedScene(gamePtr, TextViewport, 0.12f, 12, FadeColor(theme.Panel, 0.54f));
    DrawRoundedLinesScene(gamePtr, TextViewport, 0.12f, 12, FadeColor(modeStyle.Accent, 0.34f));
    DrawTypingProgress(gamePtr, theme, modeStyle.Accent, TextViewport, progressFill, errorFeedback);
    if (errorFeedback > 0.0f) {
        DrawRoundedLinesScene(gamePtr, Inflate(TextViewport, 2.0f + errorFeedback * 2.0f), 0.12f, 12, FadeColor(theme.TextError, 0.18f + errorFeedback * 0.34f));
    }

    BeginScissorScene(gamePtr, TextViewport);
    for (size_t i = 0; i < target.size(); ++i) {
        if (target[i] == '\n') {
            continue;
        }

        Color color = theme.TextDefault;
        const float glyphWidth = GlyphWidth(textFont, target[i], fontSize, UiSpacing);
        const float drawX = charPositions[i].x;
        const float drawY = charPositions[i].y - textScrollY;
        
        if (i < typed.size()) {
            if (ToLowerCodepoint(typed[i]) == ToLowerCodepoint(target[i])) {
                color = theme.TextCorrect;
                const float age = static_cast<float>(typed.size() - i);
                const float trail = std::clamp((13.0f - age) / 12.0f, 0.0f, 1.0f);
                const float hitPulse = (!lastFeedbackWasError && i == lastFeedbackIndex) ? correctFeedback : 0.0f;
                const float trailAlpha = trail * 0.10f + hitPulse * 0.22f;
                if (trailAlpha > 0.01f) {
                    DrawRoundedScene(gamePtr, { drawX - 2.0f, drawY - 4.0f, glyphWidth + 5.0f, fontSize + 10.0f }, 0.28f, 8, FadeColor(modeStyle.Accent, trailAlpha));
                }
            } else {
                color = theme.TextError;
                const float pulse = (lastFeedbackWasError && i == lastFeedbackIndex) ? EaseOut(errorFeedback) : 0.0f;
                
                DrawRoundedScene(gamePtr, { drawX - 3.0f, drawY - 5.0f, glyphWidth + 7.0f, fontSize + 11.0f }, 0.22f, 8, FadeColor(theme.TextError, 0.16f + pulse * 0.28f));
                DrawRectScene(gamePtr, { drawX, drawY + fontSize + 3.0f, glyphWidth, 3.0f + pulse * 2.0f }, FadeColor(theme.TextError, 0.78f + pulse * 0.22f));
            }
        }

        const std::string glyph = CodepointToUtf8(target[i]);
        const float errorOffset = (lastFeedbackWasError && i == lastFeedbackIndex)
            ? std::sin(errorFeedback * 18.0f) * errorFeedback * 2.5f
            : 0.0f;
        DrawTextScene(gamePtr, textFont, glyph.c_str(), { drawX + errorOffset, drawY }, fontSize, UiSpacing, color);
    }
    
    // Рисуем плавную каретку
    float caretWidth = MeasureTextEx(textFont, "A", fontSize, UiSpacing).x;
    const float caretDrawY = caretY - textScrollY;
    const float blink = (std::sin(pulseTime * 5.5f) + 1.0f) * 0.5f;
    const int nextChar = GetNextExpectedChar();
    const Color caretColor = nextChar == 0 ? theme.Caret : GetFingerColor(GetFingerForKey(nextChar), theme);
    const float impact = EaseOut(caretImpact);
    DrawRoundedScene(gamePtr, Inflate({ caretX - 1.0f, caretDrawY - 5.0f, 3.5f, fontSize + 13.0f }, 3.0f + impact * 5.0f), 0.70f, 8, FadeColor(caretColor, 0.10f + impact * 0.16f));
    DrawRoundedScene(gamePtr, { caretX - 1.0f, caretDrawY - 5.0f, 3.5f, fontSize + 13.0f }, 0.75f, 8, FadeColor(caretColor, 0.64f + blink * 0.30f));
    DrawRoundedScene(gamePtr, { caretX, caretDrawY + fontSize + 5.0f, caretWidth, 4.0f }, 1.0f, 4, FadeColor(caretColor, 0.64f));
    EndScissorMode();

    // Заголовки (системным шрифтом или тем же, но для UI можно оставить обычный)
    const char* title = GetModeTitle();
    DrawTextScene(gamePtr, uiFont, title, {98.0f, 48.0f}, 32.0f, 0.0f, theme.Title);
    DrawFittedTextScene(gamePtr, uiFont, TextFormat("%s | %s | %s", LessonLibrary::GetLanguageLabel(language).c_str(), lessonTitle.c_str(), uiLanguage == Language::Russian ? u8"ESC Меню" : "ESC Menu"), {98.0f, 88.0f}, 500.0f, 16.0f, 0.0f, theme.TextDefault);

    DrawRoundedScene(gamePtr, { 642.0f, 51.0f, 220.0f, 48.0f }, 0.24f, 10, FadeColor(modeStyle.Accent, 0.13f));
    DrawRoundedLinesScene(gamePtr, { 642.0f, 51.0f, 220.0f, 48.0f }, 0.24f, 10, FadeColor(modeStyle.Accent, 0.38f));
    DrawCircleV(gamePtr->ScalePoint({ 666.0f, 75.0f }), 14.0f * scale, FadeColor(modeStyle.Accent, 0.72f));
    DrawFittedTextScene(gamePtr, uiFont, modeStyle.Mark, { 658.0f, 67.0f }, 18.0f, 12.0f, 0.0f, theme.Background);
    DrawFittedTextScene(gamePtr, uiFont, uiLanguage == Language::Russian ? modeStyle.LabelRu : modeStyle.LabelEn, { 688.0f, 61.0f }, 150.0f, 15.0f, 0.0f, theme.Title);
    DrawFittedTextScene(gamePtr, uiFont, uiLanguage == Language::Russian ? modeStyle.ToneRu : modeStyle.ToneEn, { 688.0f, 81.0f }, 150.0f, 12.0f, 0.0f, theme.TextDefault);

    // Live Metrics
    DrawTextScene(gamePtr, uiFont, TextFormat("WPM %.0f", logic.GetWPM()), {930.0f, 52.0f}, 20.0f, 0.0f, modeStyle.Accent);
    DrawTextScene(gamePtr, uiFont, TextFormat("ACC %.0f%%", logic.GetAccuracy()), {930.0f, 84.0f}, 20.0f, 0.0f, modeStyle.Accent);
    DrawTextScene(gamePtr, uiFont, TextFormat("COMBO %d", logic.GetCurrentStreak()), {1080.0f, 52.0f}, 20.0f, 0.0f, logic.GetCurrentStreak() > 20 ? modeStyle.Accent : theme.TextDefault);

    DrawRoundedScene(gamePtr, { 60.0f, 348.0f, 1160.0f, 356.0f }, 0.12f, 12, FadeColor(theme.Panel, 0.66f));
    DrawRoundedLinesScene(gamePtr, { 60.0f, 348.0f, 1160.0f, 356.0f }, 0.12f, 12, FadeColor(modeStyle.Accent, 0.28f));
    DrawHandsGuide(uiFont, theme);
    DrawVirtualKeyboard(keyboardFont, theme);
}
