#pragma once
#include "../core/GameState.h"
#include "../core/TypingMode.h"

class ResultsState : public GameState {
public:
    ResultsState(float finalWPM, float finalAccuracy, TypingMode mode = TypingMode::Practice);

    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    float wpm;
    float accuracy;
    TypingMode retryMode;
};
