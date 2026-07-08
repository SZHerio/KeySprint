#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "../core/AppTypes.h"
#include "../core/GameState.h"
#include "../core/KeyboardLayout.h"
#include "../core/Theme.h"
#include "../core/TypingLogic.h"
#include "../core/TypingMode.h"

class TypingState : public GameState {
public:
    explicit TypingState(TypingMode mode = TypingMode::Practice, int lessonOverride = -1);

    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    TypingLogic logic;
    TypingMode mode;
    Language language = Language::English;
    Language uiLanguage = Language::Russian;
    int lessonId = 0;
    int lessonOverride = -1;
    std::string lessonTitle;
    std::string lessonDescription;
    
    // Для плавности каретки
    float caretX = 50.0f;
    float caretY = 300.0f;
    float pulseTime = 0.0f;
    float textScrollY = 0.0f;
    float progressFill = 0.0f;
    float correctFeedback = 0.0f;
    float errorFeedback = 0.0f;
    float caretImpact = 0.0f;
    size_t visualTypedLength = 0;
    size_t lastFeedbackIndex = 0;
    bool lastFeedbackWasError = false;
    
    // Предрасчет позиций символов
    std::vector<Vector2> charPositions;
    std::vector<KeyLayout> keyboardKeys;

    void CalculateLayout(Font font, float fontSize);
    void DrawVirtualKeyboard(Font font, const Theme& theme);
    void DrawHandsGuide(Font font, const Theme& theme);
    float GetTextFontSize() const;
    const char* GetModeTitle() const;
    int GetNextExpectedChar() const;
};
