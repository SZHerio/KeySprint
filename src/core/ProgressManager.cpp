#include "ProgressManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

namespace {
const char* ProgressPath = "settings/progress.json";

int& UnlockedForLanguage(ProgressData& data, Language language) {
    return language == Language::Russian ? data.russianUnlockedLesson : data.englishUnlockedLesson;
}

int UnlockedForLanguage(const ProgressData& data, Language language) {
    return language == Language::Russian ? data.russianUnlockedLesson : data.englishUnlockedLesson;
}

Difficulty DifficultyFromString(const std::string& value) {
    if (value == "relaxed") return Difficulty::Relaxed;
    if (value == "strict") return Difficulty::Strict;
    return Difficulty::Normal;
}

std::string DifficultyToString(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Relaxed: return "relaxed";
        case Difficulty::Strict: return "strict";
        case Difficulty::Normal:
        default: return "normal";
    }
}
}

void ProgressManager::Load() {
    std::ifstream file(ProgressPath);
    if (!file.is_open()) {
        data = ProgressData{};
        return;
    }

    try {
        nlohmann::json json;
        file >> json;

        data.englishUnlockedLesson = json.value("englishUnlockedLesson", 0);
        data.russianUnlockedLesson = json.value("russianUnlockedLesson", 0);
        data.completedLessons = json.value("completedLessons", 0);
        data.bestWpm = json.value("bestWpm", 0.0f);
        data.bestAccuracy = json.value("bestAccuracy", 0.0f);
        data.bestStreak = json.value("bestStreak", 0);
        data.difficulty = DifficultyFromString(json.value("difficulty", std::string("normal")));
        data.weakKeys.clear();

        if (json.contains("weakKeys") && json["weakKeys"].is_object()) {
            for (const auto& item : json["weakKeys"].items()) {
                data.weakKeys[item.key()] = item.value().get<int>();
            }
        }
    } catch (...) {
        data = ProgressData{};
    }
}

void ProgressManager::Save() const {
    std::filesystem::create_directories("settings");

    nlohmann::json json;
    json["englishUnlockedLesson"] = data.englishUnlockedLesson;
    json["russianUnlockedLesson"] = data.russianUnlockedLesson;
    json["completedLessons"] = data.completedLessons;
    json["bestWpm"] = data.bestWpm;
    json["bestAccuracy"] = data.bestAccuracy;
    json["bestStreak"] = data.bestStreak;
    json["difficulty"] = DifficultyToString(data.difficulty);
    json["weakKeys"] = data.weakKeys;

    std::ofstream file(ProgressPath);
    file << json.dump(2);
}

void ProgressManager::Reset() {
    data = ProgressData{};
    Save();
}

void ProgressManager::RecordResult(Language language, int lessonId, float wpm, float accuracy, int bestStreak, const std::map<std::string, int>& mistakes) {
    data.bestWpm = std::max(data.bestWpm, wpm);
    data.bestAccuracy = std::max(data.bestAccuracy, accuracy);
    data.bestStreak = std::max(data.bestStreak, bestStreak);

    for (const auto& [key, count] : mistakes) {
        data.weakKeys[key] += count;
    }

    // Completing with solid accuracy unlocks the next lesson.
    if (lessonId >= 0 && accuracy >= GetUnlockAccuracyThreshold()) {
        int& unlocked = UnlockedForLanguage(data, language);
        if (lessonId >= unlocked) {
            unlocked = lessonId + 1;
            data.completedLessons++;
        }
    }

    Save();
}

int ProgressManager::GetUnlockedLesson(Language language) const {
    return UnlockedForLanguage(data, language);
}

int ProgressManager::GetCurrentLesson(Language language, int lessonCount) const {
    if (lessonCount <= 0) {
        return 0;
    }

    return std::clamp(GetUnlockedLesson(language), 0, lessonCount - 1);
}

void ProgressManager::CycleDifficulty() {
    switch (data.difficulty) {
        case Difficulty::Relaxed:
            data.difficulty = Difficulty::Normal;
            break;
        case Difficulty::Normal:
            data.difficulty = Difficulty::Strict;
            break;
        case Difficulty::Strict:
            data.difficulty = Difficulty::Relaxed;
            break;
    }
    Save();
}

float ProgressManager::GetUnlockAccuracyThreshold() const {
    switch (data.difficulty) {
        case Difficulty::Relaxed: return 75.0f;
        case Difficulty::Strict: return 92.0f;
        case Difficulty::Normal:
        default: return 85.0f;
    }
}

std::string ProgressManager::GetDifficultyLabel() const {
    switch (data.difficulty) {
        case Difficulty::Relaxed: return "Relaxed";
        case Difficulty::Strict: return "Strict";
        case Difficulty::Normal:
        default: return "Normal";
    }
}

std::string ProgressManager::GetRankLabel() const {
    if (data.bestWpm >= 80.0f && data.bestAccuracy >= 95.0f) return "Master";
    if (data.bestWpm >= 55.0f && data.bestAccuracy >= 92.0f) return "Swift";
    if (data.bestWpm >= 35.0f && data.bestAccuracy >= 88.0f) return "Steady";
    return "Novice";
}

std::string ProgressManager::GetWeakKeyOfDay() const {
    if (data.weakKeys.empty()) {
        return "?";
    }

    std::vector<std::pair<std::string, int>> ranked(data.weakKeys.begin(), data.weakKeys.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });
    return ranked.front().first;
}

std::string ProgressManager::GetWeakFingerOfDay() const {
    const std::string key = GetWeakKeyOfDay();
    if (key.empty() || key == "?") return "No data";

    if (key == "Space") return "Thumb";
    if (key == "Enter") return "Right pinky";
    if (key == "a" || key == "q" || key == "z" || key == u8"ф" || key == u8"й" || key == u8"я") return "Left pinky";
    if (key == "s" || key == "w" || key == "x" || key == u8"ы" || key == u8"ц" || key == u8"ч") return "Left ring";
    if (key == "d" || key == "e" || key == "c" || key == u8"в" || key == u8"у" || key == u8"с") return "Left middle";
    if (key == "f" || key == "g" || key == "r" || key == "t" || key == "v" || key == "b" || key == u8"а" || key == u8"п" || key == u8"к" || key == u8"е" || key == u8"м" || key == u8"и") return "Left index";
    if (key == "j" || key == "h" || key == "y" || key == "u" || key == "n" || key == "m" || key == u8"о" || key == u8"р" || key == u8"н" || key == u8"г" || key == u8"т" || key == u8"ь") return "Right index";
    if (key == "k" || key == "i" || key == u8"л" || key == u8"ш") return "Right middle";
    if (key == "l" || key == "o" || key == u8"д" || key == u8"щ") return "Right ring";
    if (key == ";" || key == "p" || key == u8"ж" || key == u8"э" || key == u8"з" || key == u8"х" || key == u8"б" || key == u8"ю") return "Right pinky";
    return "Mixed finger";
}
