#pragma once

#include "../core/GameState.h"

class ProgressState : public GameState {
public:
    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    float animTime = 0.0f;
};
