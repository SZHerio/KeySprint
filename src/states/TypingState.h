#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "../core/GameState.h"
#include "../core/Theme.h"
#include "../core/TypingLogic.h"
#include "../core/TypingMode.h"

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
    
    // Предрасчет позиций символов
    std::vector<Vector2> charPositions;
    void CalculateLayout(Font font, float fontSize);
    void DrawVirtualKeyboard(Font font, const Theme& theme);
    char GetNextExpectedChar() const;
};
