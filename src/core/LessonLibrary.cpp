#include "LessonLibrary.h"

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <nlohmann/json.hpp>

namespace {
using json = nlohmann::json;

constexpr const char* ContentPath = "assets/content/typing_content.json";

struct LanguageContent {
    std::vector<Lesson> lessons;
    std::vector<std::string> practiceSentences;
    std::vector<std::string> compositionParagraphs;
};

struct TypingContent {
    LanguageContent english;
    LanguageContent russian;
};

std::mt19937& Rng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

std::vector<std::string> Shuffled(std::vector<std::string> items) {
    std::shuffle(items.begin(), items.end(), Rng());
    return items;
}

std::string JsonString(const json& object, const char* key) {
    const auto found = object.find(key);
    return found != object.end() && found->is_string() ? found->get<std::string>() : "";
}

std::vector<Lesson> ParseLessons(const json& languageNode) {
    std::vector<Lesson> lessons;
    const auto found = languageNode.find("lessons");
    if (found == languageNode.end() || !found->is_array()) {
        return lessons;
    }

    int fallbackId = 0;
    for (const json& item : *found) {
        if (!item.is_object()) {
            continue;
        }

        lessons.push_back({
            item.value("id", fallbackId),
            JsonString(item, "title"),
            JsonString(item, "description"),
            JsonString(item, "text")
        });
        ++fallbackId;
    }
    return lessons;
}

std::vector<std::string> ParseStringList(const json& languageNode, const char* key) {
    std::vector<std::string> items;
    const auto found = languageNode.find(key);
    if (found == languageNode.end() || !found->is_array()) {
        return items;
    }

    for (const json& item : *found) {
        if (item.is_string()) {
            items.push_back(item.get<std::string>());
        }
    }
    return items;
}

LanguageContent ParseLanguageContent(const json& root, const char* key) {
    const auto languages = root.find("languages");
    if (languages == root.end() || !languages->is_object()) {
        return {};
    }

    const auto language = languages->find(key);
    if (language == languages->end() || !language->is_object()) {
        return {};
    }

    return {
        ParseLessons(*language),
        ParseStringList(*language, "practice_sentences"),
        ParseStringList(*language, "composition_paragraphs")
    };
}

LanguageContent EmergencyLanguageContent(Language language) {
    if (language == Language::Russian) {
        return {
            {
                { 0, u8"Контент не найден", u8"Проверь файл assets/content/typing_content.json.", u8"контент не найден" }
            },
            { u8"Контент не найден; проверь файл assets/content/typing_content.json." },
            { u8"Контент не найден. Проверь, что рядом с программой есть папка assets/content и файл typing_content.json." }
        };
    }

    return {
        {
            { 0, "Content Missing", "Check assets/content/typing_content.json.", "content missing" }
        },
        { "Content is missing; check assets/content/typing_content.json." },
        { "Content is missing. Make sure the assets/content folder and typing_content.json file are shipped with the program." }
    };
}

TypingContent EmergencyContent() {
    return {
        EmergencyLanguageContent(Language::English),
        EmergencyLanguageContent(Language::Russian)
    };
}

bool IsUsable(const LanguageContent& content) {
    return !content.lessons.empty() &&
        !content.practiceSentences.empty() &&
        !content.compositionParagraphs.empty();
}

TypingContent LoadContent() {
    std::ifstream input(ContentPath);
    if (!input) {
        return EmergencyContent();
    }

    try {
        const json root = json::parse(input);
        TypingContent content {
            ParseLanguageContent(root, "english"),
            ParseLanguageContent(root, "russian")
        };

        if (!IsUsable(content.english)) {
            content.english = EmergencyLanguageContent(Language::English);
        }
        if (!IsUsable(content.russian)) {
            content.russian = EmergencyLanguageContent(Language::Russian);
        }

        return content;
    } catch (...) {
        return EmergencyContent();
    }
}

const TypingContent& Content() {
    static const TypingContent content = LoadContent();
    return content;
}

const LanguageContent& DataFor(Language language) {
    return language == Language::Russian ? Content().russian : Content().english;
}

std::string JoinSample(const std::vector<std::string>& source, int count, const std::string& separator) {
    if (source.empty() || count <= 0) {
        return "";
    }

    std::vector<std::string> items = Shuffled(source);
    std::ostringstream out;
    const int actualCount = std::min(count, static_cast<int>(items.size()));
    for (int i = 0; i < actualCount; ++i) {
        if (i > 0) {
            out << separator;
        }
        out << items[i];
    }
    return out.str();
}

std::string JoinSampleLines(const std::vector<std::string>& source, int count, bool trailingNewline) {
    if (source.empty() || count <= 0) {
        return "";
    }

    std::vector<std::string> items = Shuffled(source);
    std::ostringstream out;
    const int actualCount = std::min(count, static_cast<int>(items.size()));
    for (int i = 0; i < actualCount; ++i) {
        if (i > 0) {
            out << '\n';
        }
        out << items[i];
    }
    if (trailingNewline) {
        out << '\n';
    }
    return out.str();
}

bool IsSharedSymbolKey(const std::string& key) {
    static const std::string symbols = R"(,.-:;' "<>()[]{}\|/?!)";
    return key.size() == 1 && symbols.find(key[0]) != std::string::npos;
}

std::string RandomizeDrillText(const std::string& text) {
    std::istringstream input(text);
    std::vector<std::string> tokens;
    std::string token;

    while (input >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() <= 4) {
        return text;
    }

    tokens = Shuffled(tokens);
    std::ostringstream out;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) {
            out << ' ';
        }
        out << tokens[i];
    }
    return out.str();
}
}

const std::vector<Lesson>& LessonLibrary::GetLessons(Language language) {
    return DataFor(language).lessons;
}

Lesson LessonLibrary::GetLesson(Language language, int lessonId) {
    const auto& lessons = GetLessons(language);
    if (lessons.empty()) {
        return EmergencyLanguageContent(language).lessons.front();
    }

    const int clampedId = std::clamp(lessonId, 0, static_cast<int>(lessons.size()) - 1);
    return lessons[clampedId];
}

Lesson LessonLibrary::BuildAdaptiveLesson(Language language, int lessonId, const std::map<std::string, int>& weakKeys) {
    Lesson lesson = GetLesson(language, lessonId);
    if (lessonId <= 9) {
        lesson.text = RandomizeDrillText(lesson.text);
    }

    const std::string focusText = GenerateFocusText(language, weakKeys);
    if (!focusText.empty()) {
        lesson.title += language == Language::Russian ? u8" + Фокус" : " + Focus";
        lesson.description = language == Language::Russian
            ? u8"Добавлен короткий блок по слабым клавишам."
            : "Adaptive drill added for your weakest keys.";
        lesson.text += " " + focusText;
    }
    return lesson;
}

std::string LessonLibrary::BuildPracticeText(Language language) {
    return JoinSampleLines(DataFor(language).practiceSentences, 32, true);
}

std::string LessonLibrary::BuildCompositionText(Language language) {
    return JoinSample(DataFor(language).compositionParagraphs, 16, "\n");
}

std::string LessonLibrary::BuildDailyChallengeText(Language language) {
    const LanguageContent& content = DataFor(language);
    return JoinSampleLines(content.practiceSentences, 6, false) + "\n" +
        JoinSample(content.compositionParagraphs, 1, "\n");
}

std::string LessonLibrary::GetLanguageLabel(Language language) {
    return language == Language::Russian ? "RU" : "EN";
}

bool LessonLibrary::IsKeyForLanguage(const std::string& key, Language language) {
    if (key == "Space" || key == "Enter") {
        return true;
    }

    if (key.empty()) {
        return false;
    }

    if (IsSharedSymbolKey(key)) {
        return true;
    }

    const bool asciiKey = key.size() == 1 && static_cast<unsigned char>(key[0]) < 128;
    return language == Language::English ? asciiKey : !asciiKey;
}

std::string LessonLibrary::FormatKeyLabel(const std::string& key, Language language) {
    if (key.empty()) {
        return language == Language::Russian ? u8"пока нет" : "none yet";
    }
    if (key == "Space") {
        return language == Language::Russian ? u8"Пробел" : "Space";
    }
    if (key == "Enter") {
        return language == Language::Russian ? u8"Ввод" : "Enter";
    }
    return key;
}

std::string LessonLibrary::GenerateFocusText(Language language, const std::map<std::string, int>& weakKeys) {
    if (weakKeys.empty()) {
        return "";
    }

    std::vector<std::pair<std::string, int>> ranked;
    for (const auto& [key, count] : weakKeys) {
        if (key != "Space" && key != "Enter" && IsKeyForLanguage(key, language)) {
            ranked.push_back({ key, count });
        }
    }

    if (ranked.empty()) {
        return "";
    }

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
