#pragma once

#include <raylib.h>
#include "../core/AppTypes.h"
#include "../core/GameState.h"

class LessonSelectState : public GameState {
public:
    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    Language language = Language::English;
    int selectedLesson = 0;
    float cursorX = 0.0f;
    float cursorY = 0.0f;
    float animTime = 0.0f;

    int GetLessonCount() const;
    bool IsLessonUnlocked(int lessonId) const;
    Rectangle GetCardRect(int index) const;
    void StartSelectedLesson();
};
