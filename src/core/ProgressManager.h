#pragma once

#include <map>
#include <string>
#include "AppTypes.h"

struct ProgressData {
    int englishUnlockedLesson = 0;
    int russianUnlockedLesson = 0;
    int completedLessons = 0;
    float bestWpm = 0.0f;
    float bestAccuracy = 0.0f;
    int bestStreak = 0;
    Difficulty difficulty = Difficulty::Normal;
    std::map<std::string, int> weakKeys;
};

class ProgressManager {
public:
    void Load();
    void Save() const;
    void Reset();

    void RecordResult(Language language, int lessonId, float wpm, float accuracy, int bestStreak, const std::map<std::string, int>& mistakes);

    int GetUnlockedLesson(Language language) const;
    int GetCurrentLesson(Language language, int lessonCount) const;
    float GetBestWpm() const { return data.bestWpm; }
    float GetBestAccuracy() const { return data.bestAccuracy; }
    int GetBestStreak() const { return data.bestStreak; }
    int GetCompletedLessons() const { return data.completedLessons; }
    const std::map<std::string, int>& GetWeakKeys() const { return data.weakKeys; }
    Difficulty GetDifficulty() const { return data.difficulty; }
    void CycleDifficulty();
    float GetUnlockAccuracyThreshold() const;
    std::string GetDifficultyLabel() const;
    std::string GetRankLabel() const;
    std::string GetWeakKeyOfDay() const;
    std::string GetWeakFingerOfDay() const;

private:
    ProgressData data;
};
