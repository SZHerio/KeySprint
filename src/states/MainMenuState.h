#pragma once
#include <array>
#include "../core/GameState.h"

class MainMenuState : public GameState {
public:
    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    int selectedOption = 0;
    float highlightY = 250.0f;
    std::array<const char*, 3> options = { "Practice", "Tutorial", "Settings" };
};
