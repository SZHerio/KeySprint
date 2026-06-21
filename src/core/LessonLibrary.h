#pragma once

#include <map>
#include <string>
#include <vector>
#include "AppTypes.h"

struct Lesson {
    int id;
    std::string title;
    std::string description;
    std::string text;
};

class LessonLibrary {
public:
    static const std::vector<Lesson>& GetLessons(Language language);
    static Lesson GetLesson(Language language, int lessonId);
    static Lesson BuildAdaptiveLesson(Language language, int lessonId, const std::map<std::string, int>& weakKeys);
    static std::string GetLanguageLabel(Language language);

private:
    static std::string GenerateFocusText(const std::map<std::string, int>& weakKeys);
};
