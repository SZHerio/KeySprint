#include "LessonLibrary.h"

#include <algorithm>
#include <sstream>

namespace {
const std::vector<Lesson> EnglishLessons = {
    { 0, "Home Row", "Master ASDF and JKL; first.", "asdf jkl; asdf jkl; fj fj dk dk sl sl aa ss dd ff jj kk ll ;;" },
    { 1, "Top Row", "Add QWERTY and UIOP.", "qwer uiop qwer uiop aq sw de fr ju ki lo p; qwerty uiop" },
    { 2, "Bottom Row", "Add ZXCV and BNM.", "zxcv bnm zxcv bnm az sx dc fv jn km lm vb nm" },
    { 3, "Mixed Words", "Train all learned keys with real words.", "type fast code sprint keyboard program focus rhythm lesson result" }
};

const std::vector<Lesson> RussianLessons = {
    { 0, "Домашний ряд", "Освой ФЫВА и ОЛДЖ.", u8"фыва олдж фыва олдж аж аж вд вд ыо ыо фф ыы вв аа оо лл дд жж" },
    { 1, "Верхний ряд", "Добавь ЙЦУКЕН и ГШЩЗ.", u8"йцук енгш йцук енгш фй ыц ву ак он лг дш жз йцукен гшщз" },
    { 2, "Нижний ряд", "Добавь ЯЧСМ и ИТЬБЮ.", u8"ячсм итьбю ячсм итьбю фя ыч вс ам ои лт дь жб эю" },
    { 3, "Смешанные слова", "Переход к коротким русским словам.", u8"мама дом школа код урок рука палец клавиша текст скорость точность" }
};
}

const std::vector<Lesson>& LessonLibrary::GetLessons(Language language) {
    return language == Language::Russian ? RussianLessons : EnglishLessons;
}

Lesson LessonLibrary::GetLesson(Language language, int lessonId) {
    const auto& lessons = GetLessons(language);
    const int clampedId = std::clamp(lessonId, 0, static_cast<int>(lessons.size()) - 1);
    return lessons[clampedId];
}

Lesson LessonLibrary::BuildAdaptiveLesson(Language language, int lessonId, const std::map<std::string, int>& weakKeys) {
    Lesson lesson = GetLesson(language, lessonId);
    const std::string focusText = GenerateFocusText(weakKeys);
    if (!focusText.empty()) {
        lesson.title += " + Focus";
        lesson.description = "Adaptive drill added for your weakest keys.";
        lesson.text += " " + focusText;
    }
    return lesson;
}

std::string LessonLibrary::GetLanguageLabel(Language language) {
    return language == Language::Russian ? "RU" : "EN";
}

std::string LessonLibrary::GenerateFocusText(const std::map<std::string, int>& weakKeys) {
    if (weakKeys.empty()) {
        return "";
    }

    std::vector<std::pair<std::string, int>> ranked(weakKeys.begin(), weakKeys.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    std::ostringstream text;
    const int count = std::min(3, static_cast<int>(ranked.size()));
    for (int i = 0; i < count; ++i) {
        const std::string& key = ranked[i].first;
        text << key << key << " " << key << key << " ";
    }

    for (int repeat = 0; repeat < 4; ++repeat) {
        for (int i = 0; i < count; ++i) {
            text << ranked[i].first;
        }
        text << " ";
    }

    return text.str();
}
