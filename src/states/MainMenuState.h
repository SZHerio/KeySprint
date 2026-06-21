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
    float highlightY = 232.0f;
    float menuTime = 0.0f;
    std::array<const char*, 7> options = { "Practice", "Lessons", "Daily Challenge", "Composition", "Progress", "Settings", "Exit" };
    std::array<const char*, 7> descriptions = {
        "Short generated drills with live WPM and finger guidance.",
        "Pick a lesson card, replay older drills or unlock the next level.",
        "A fresh mixed challenge with a daily mission mindset.",
        "Long-form typing flow: larger text, scrolling canvas and endurance.",
        "Review rank, missions, weak keys, lesson unlocks and reset progress.",
        "Theme, language, difficulty, sound profiles and progress reset.",
        "Close KeySprint and return to desktop."
    };
};
