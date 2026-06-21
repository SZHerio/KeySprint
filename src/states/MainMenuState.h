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
    std::array<const char*, 5> options = { "Practice", "Lessons", "Composition", "Progress", "Settings" };
    std::array<const char*, 5> descriptions = {
        "Short generated drills with live WPM and finger guidance.",
        "Pick a lesson card, replay older drills or unlock the next level.",
        "Long-form typing flow: larger text, scrolling canvas and endurance.",
        "Review best scores, weak keys, lesson unlocks and reset progress.",
        "Theme, language, sound profiles and progress reset."
    };
};
