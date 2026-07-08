#pragma once

#include <map>
#include <string>
#include <vector>
#include "AppTypes.h"

struct SessionRecord {
    Language language = Language::English;
    float wpm = 0.0f;
    float accuracy = 0.0f;
    int bestStreak = 0;
};

struct ProgressData {
    int englishUnlockedLesson = 0;
    int russianUnlockedLesson = 0;
    int completedLessons = 0;
    float bestWpm = 0.0f;
    float bestAccuracy = 0.0f;
    int bestStreak = 0;
    Difficulty difficulty = Difficulty::Normal;
    Language uiLanguage = Language::Russian;
    Language typingLanguage = Language::English;
    int themeIndex = 0;
    int uiFontIndex = 0;
    int typingTextFontIndex = 0;
    int keyboardFontIndex = 0;
    std::map<std::string, int> weakKeys;
    std::vector<SessionRecord> recentSessions;
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
    const std::vector<SessionRecord>& GetRecentSessions() const { return data.recentSessions; }
    Difficulty GetDifficulty() const { return data.difficulty; }
    void SetDifficulty(Difficulty difficulty);
    void CycleDifficulty();
    Language GetUiLanguage() const { return data.uiLanguage; }
    Language GetTypingLanguage() const { return data.typingLanguage; }
    void SetUiLanguage(Language language);
    void SetTypingLanguage(Language language);
    int GetThemeIndex() const { return data.themeIndex; }
    void SetThemeIndex(int index, int themeCount);
    int GetUiFontIndex() const { return data.uiFontIndex; }
    void SetUiFontIndex(int index, int fontCount);
    int GetTypingTextFontIndex() const { return data.typingTextFontIndex; }
    int GetKeyboardFontIndex() const { return data.keyboardFontIndex; }
    void SetTypingTextFontIndex(int index, int fontCount);
    void SetKeyboardFontIndex(int index, int fontCount);
    void CycleTypingTextFont(int fontCount);
    void CycleKeyboardFont(int fontCount);
    float GetUnlockAccuracyThreshold() const;
    std::string GetDifficultyLabel() const;
    std::string GetRankLabel() const;
    std::string GetWeakKeyOfDay(Language language) const;
    std::string GetWeakFingerOfDay(Language language) const;

private:
    ProgressData data;
};
