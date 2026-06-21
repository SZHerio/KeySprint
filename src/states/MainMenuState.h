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
    float menuTime = 0.0f;
    std::array<const char*, 4> options = { "Practice", "Tutorial", "Composition", "Settings" };
    std::array<const char*, 4> descriptions = {
        "Short generated drills with live WPM and finger guidance.",
        "Structured lessons with unlocks, weak-key focus and colored hands.",
        "Long-form typing flow: larger text, scrolling canvas and endurance.",
        "Theme, language, sound profiles and progress reset."
    };
};
