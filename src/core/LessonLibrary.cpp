#include "LessonLibrary.h"

#include <algorithm>
#include <sstream>

namespace {
const std::vector<Lesson> EnglishLessons = {
    { 0, "Home Row", "Master ASDF and JKL; first.", "asdf jkl; asdf jkl; fj fj dk dk sl sl aa ss dd ff jj kk ll ;;" },
    { 1, "Left Alternation", "Alternate left-hand fingers without rushing.", "as sa ad da af fa sd ds df fd asdf fdsa asdf fdsa" },
    { 2, "Right Alternation", "Alternate right-hand fingers on the home row.", "jk kj jl lj j; ;j kl lk k; ;k jkl; ;lkj jkl; ;lkj" },
    { 3, "Mirror Pairs", "Train both hands with mirrored home-row pairs.", "fj jf dk kd sl ls a; ;a fj dk sl a; jf kd ls ;a" },
    { 4, "Index Crossovers", "Strengthen index finger transitions.", "fg gf fr rf ft tf jh hj jy yj ju uj fg jh fr ju ft jy" },
    { 5, "Top Row Alternation", "Add top row with finger-to-finger rhythm.", "qw wq er re ty yt ui iu op po qwer rewq yuiop poiuy" },
    { 6, "Bottom Row Alternation", "Add bottom row jumps from home row.", "zx xz cv vc bn nb mn nm az sx dc fv jn km lm ;m" },
    { 7, "Hand Switching", "Switch hands cleanly on every beat.", "fa j; fa j; dr ki dr ki se lo se lo aq p; aq p;" },
    { 8, "Mixed Rhythm", "Blend all rows into short rhythm groups.", "fast type code sprint rhythm focus lesson result keyboard program. steady hands make clean words.\nclear motion keeps every finger ready for the next key." },
    { 9, "Accuracy Run", "Longer mixed drill for control and endurance.", "focus on accuracy first then let speed grow with rhythm and relaxed fingers.\npress enter after this paragraph and keep the same calm tempo. reliable typing comes from patient practice and careful correction." }
};

const std::vector<Lesson> RussianLessons = {
    { 0, "Домашний ряд", "Освой ФЫВА и ОЛДЖ.", u8"фыва олдж фыва олдж аж аж вд вд ыо ыо фф ыы вв аа оо лл дд жж" },
    { 1, "Левая рука", "Чередование пальцев левой руки.", u8"фы ыф ыв вы ва ав фыва авыф фыва авыф" },
    { 2, "Правая рука", "Чередование пальцев правой руки.", u8"ол ло лд дл дж жд олдж ждло олдж ждло" },
    { 3, "Зеркальные пары", "Две руки работают симметрично.", u8"аж жа вд дв ыл лы фж жф аж вд ыл фж жа дв лы жф" },
    { 4, "Указательные", "Переходы указательных пальцев.", u8"ап па пр рп ке ек нг гн ап рп ке нг пр ек па гн" },
    { 5, "Верхний ряд", "Добавь ЙЦУКЕН и ГШЩЗ.", u8"йц цй ук ку ен не гш шг щз зщ йцук кущз енгш шгне" },
    { 6, "Нижний ряд", "Добавь ЯЧСМ и ИТЬБЮ.", u8"яч чя см мс ит ти ьб бь юж жю фя ыч вс ам ои лт дь жб" },
    { 7, "Смена рук", "Каждый такт переносит внимание на другую руку.", u8"фа ол фа ол вы дж вы дж ак нг ак нг фж жа вд дв" },
    { 8, "Смешанный ритм", "Короткие слова на все изученные зоны.", u8"мама дом школа код урок рука палец клавиша текст скорость точность. спокойные руки дают чистый набор.\nровный ритм помогает каждому пальцу вернуться домой." },
    { 9, "Длинный проход", "Тренировка выносливости и точности.", u8"сначала точность потом скорость пальцы двигаются спокойно и возвращаются домой.\nнажми enter после этого абзаца и сохрани тот же темп. уверенная печать растет из терпеливой практики." }
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
