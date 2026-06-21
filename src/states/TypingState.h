#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "../core/GameState.h"
#include "../core/Theme.h"
#include "../core/TypingLogic.h"
#include "../core/TypingMode.h"

enum class FingerType {
    None,
    LeftPinky,
    LeftRing,
    LeftMiddle,
    LeftIndex,
    LeftThumb,
    RightThumb,
    RightIndex,
    RightMiddle,
    RightRing,
    RightPinky
};

enum class HandSide {
    Left,
    Right,
    Both
};

struct KeyLayout {
    char key;
    std::string label;
    int row;
    float width;
    FingerType finger;
    HandSide hand;
};

class TypingState : public GameState {
public:
    explicit TypingState(TypingMode mode = TypingMode::Practice);

    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    TypingLogic logic;
    TypingMode mode;
    
    // Для плавности каретки
    float caretX = 50.0f;
    float caretY = 300.0f;
    float pulseTime = 0.0f;
    
    // Предрасчет позиций символов
    std::vector<Vector2> charPositions;
    std::vector<KeyLayout> keyboardKeys;

    void BuildKeyboardModel();
    void CalculateLayout(Font font, float fontSize);
    void DrawVirtualKeyboard(Font font, const Theme& theme);
    void DrawHandsGuide(Font font, const Theme& theme);
    char GetNextExpectedChar() const;
    FingerType GetFingerForKey(char key) const;
    Color GetFingerColor(FingerType finger, const Theme& theme) const;
};
