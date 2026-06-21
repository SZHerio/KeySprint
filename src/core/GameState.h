#pragma once

class Game;

class GameState {
public:
    virtual ~GameState() = default;
    virtual void Init(Game* game) = 0;
    virtual void HandleInput() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
};
