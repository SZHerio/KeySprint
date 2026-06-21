#include "ProgressManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace {
const char* ProgressPath = "settings/progress.json";

int& UnlockedForLanguage(ProgressData& data, Language language) {
    return language == Language::Russian ? data.russianUnlockedLesson : data.englishUnlockedLesson;
}

int UnlockedForLanguage(const ProgressData& data, Language language) {
    return language == Language::Russian ? data.russianUnlockedLesson : data.englishUnlockedLesson;
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
    json["weakKeys"] = data.weakKeys;

    std::ofstream file(ProgressPath);
    file << json.dump(2);
}

void ProgressManager::Reset() {
    data = ProgressData{};
    Save();
}

void ProgressManager::RecordResult(Language language, int lessonId, float wpm, float accuracy, const std::map<std::string, int>& mistakes) {
    data.bestWpm = std::max(data.bestWpm, wpm);
    data.bestAccuracy = std::max(data.bestAccuracy, accuracy);

    for (const auto& [key, count] : mistakes) {
        data.weakKeys[key] += count;
    }

    // Completing with solid accuracy unlocks the next lesson.
    if (accuracy >= 85.0f) {
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
