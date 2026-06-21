#pragma once
#include <map>
#include <string>
#include "../core/AppTypes.h"
#include "../core/GameState.h"
#include "../core/TypingMode.h"

class ResultsState : public GameState {
public:
    ResultsState(
        float finalWPM,
        float finalAccuracy,
        TypingMode mode = TypingMode::Practice,
        Language language = Language::English,
        int lessonId = -1,
        const std::string& lessonTitle = "",
        int bestStreak = 0,
        const std::map<std::string, int>& mistakes = {}
    );

    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    Game* gamePtr = nullptr;
    float wpm;
    float accuracy;
    TypingMode retryMode;
    Language language;
    int lessonId;
    std::string lessonTitle;
    int bestStreak;
    std::map<std::string, int> mistakes;
    bool recorded = false;
};
