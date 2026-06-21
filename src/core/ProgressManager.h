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
    std::map<std::string, int> weakKeys;
};

class ProgressManager {
public:
    void Load();
    void Save() const;
    void Reset();

    void RecordResult(Language language, int lessonId, float wpm, float accuracy, const std::map<std::string, int>& mistakes);

    int GetUnlockedLesson(Language language) const;
    int GetCurrentLesson(Language language, int lessonCount) const;
    float GetBestWpm() const { return data.bestWpm; }
    float GetBestAccuracy() const { return data.bestAccuracy; }
    int GetCompletedLessons() const { return data.completedLessons; }
    const std::map<std::string, int>& GetWeakKeys() const { return data.weakKeys; }

private:
    ProgressData data;
};
