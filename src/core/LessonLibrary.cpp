#include "LessonLibrary.h"

#include <algorithm>
#include <random>
#include <sstream>

namespace {
std::mt19937& Rng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

std::vector<std::string> Shuffled(std::vector<std::string> items) {
    std::shuffle(items.begin(), items.end(), Rng());
    return items;
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

const std::vector<std::string> EnglishPracticeSentences = {
    "calm focus builds reliable typing speed",
    "keep your fingers near the home row and watch the rhythm",
    "short clear sentences help you train accuracy before speed",
    "steady motion makes each key easier to find",
    "a relaxed hand returns to the center after every reach",
    "small corrections today become strong typing habits tomorrow",
    "the cursor moves best when your shoulders stay loose",
    "practice with care and let speed follow the pattern",
    "good typing feels smooth instead of forced",
    "each mistake points to a key that needs attention",
    "balanced hands make long text less tiring",
    "read the next word before your fingers arrive",
    "slow rhythm is useful when you are learning a new row",
    "clean accuracy turns into speed after enough repetition",
    "focus on the line and keep the sound of keys even",
    "your best run starts with the first careful letter",
    "the lesson is easier when every finger has one clear job",
    "longer phrases teach endurance and stable attention",
    "typing becomes faster when the path is familiar",
    "a clear mind helps the hands stay organized",
    "soft key presses reduce tension during practice",
    "the next word should feel expected rather than surprising",
    "let your eyes lead and your fingers follow",
    "a steady tempo helps prevent repeated errors"
};

const std::vector<std::string> RussianPracticeSentences = {
    u8"спокойный фокус помогает печатать быстрее",
    u8"держи пальцы рядом с домашним рядом и следи за ритмом",
    u8"короткие ясные фразы тренируют точность перед скоростью",
    u8"ровное движение делает каждую клавишу понятнее",
    u8"расслабленная рука возвращается в центр после каждого движения",
    u8"маленькие исправления сегодня становятся сильной привычкой завтра",
    u8"курсор движется лучше когда плечи остаются свободными",
    u8"тренируйся аккуратно и скорость придет за ритмом",
    u8"хорошая печать ощущается плавной а не напряженной",
    u8"каждая ошибка показывает клавишу которой нужно внимание",
    u8"сбалансированные руки меньше устают на длинном тексте",
    u8"читай следующее слово до того как пальцы пришли к нему",
    u8"медленный ритм полезен когда изучаешь новый ряд",
    u8"чистая точность превращается в скорость после повторений",
    u8"следи за строкой и держи звук клавиш ровным",
    u8"лучший проход начинается с первой аккуратной буквы",
    u8"урок проще когда у каждого пальца есть понятная роль",
    u8"длинные фразы тренируют выносливость и внимание",
    u8"печать ускоряется когда путь пальцев становится знакомым",
    u8"ясная голова помогает рукам оставаться организованными",
    u8"мягкие нажатия уменьшают напряжение во время практики",
    u8"следующее слово должно ощущаться ожидаемым",
    u8"пусть глаза ведут а пальцы спокойно следуют",
    u8"ровный темп помогает не повторять одни и те же ошибки"
};

const std::vector<std::string> EnglishCompositionParagraphs = {
    "Fast typing is not only about speed. It starts with calm rhythm, relaxed hands, and a clear return to the home row after every movement. When attention stops jumping from key to key, the sentence begins to flow naturally.",
    "Mistakes are useful signals. Each one shows which finger needs a focused drill, which row feels uncertain, and where the next short practice block should begin.",
    "When the paragraph ends, press Enter and continue with the next thought without breaking posture. The goal is to keep the same tempo after the line changes.",
    "Long-form typing teaches endurance. It asks you to keep the same pressure, the same focus, and the same soft return to the home row for several connected ideas.",
    "A reliable rhythm will always beat a tense burst of random fast typing. Accuracy makes speed repeatable, and repeatable speed is what matters in real work.",
    "The best practice session feels almost quiet. Your hands move, the cursor advances, and the mind stays a few words ahead of the keys.",
    "Connected text is harder than isolated words because it asks for memory, punctuation, spacing, and patience. That is why composition mode matters.",
    "If accuracy stays high through multiple paragraphs, speed becomes a natural result instead of a forced sprint. The hands learn to trust the pattern."
};

const std::vector<std::string> RussianCompositionParagraphs = {
    u8"Слепая печать начинается не со скорости, а со спокойного ритма. Пальцы должны возвращаться на домашний ряд после каждого движения, а внимание должно оставаться на строке.",
    u8"Ошибки полезны, если смотреть на них как на подсказки. Каждая ошибка показывает, какой палец устал, какая клавиша выпадает и какой блок стоит повторить.",
    u8"Когда абзац заканчивается, нажми Enter и продолжай новый фрагмент без спешки. Важно сохранить тот же темп после перехода на новую строку.",
    u8"Длинный текст учит держать внимание дольше нескольких слов. Здесь важны устойчивость, мягкая посадка пальцев и ровное дыхание.",
    u8"Надежный ритм всегда лучше резкого рывка. Точность делает скорость повторяемой, а повторяемая скорость нужна в настоящей работе.",
    u8"Хорошая тренировка почти не ощущается шумной. Руки двигаются, курсор идет вперед, а взгляд заранее читает следующие слова.",
    u8"Связный текст сложнее набора отдельных слов, потому что требует памяти, пробелов, знаков и терпения. Поэтому режим сочинения особенно полезен.",
    u8"Если точность остается высокой несколько абзацев подряд, скорость начинает расти сама. Пальцы привыкают доверять знакомому движению."
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
    lesson.text = RandomizeDrillText(lesson.text);

    const std::string focusText = GenerateFocusText(weakKeys);
    if (!focusText.empty()) {
        lesson.title += " + Focus";
        lesson.description = "Adaptive drill added for your weakest keys.";
        lesson.text += " " + focusText;
    }
    return lesson;
}

std::string LessonLibrary::BuildPracticeText(Language language) {
    const auto& sentences = language == Language::Russian ? RussianPracticeSentences : EnglishPracticeSentences;
    return JoinSample(sentences, 5, ". ") + ".\n" + JoinSample(sentences, 3, ". ") + ".";
}

std::string LessonLibrary::BuildCompositionText(Language language) {
    const auto& paragraphs = language == Language::Russian ? RussianCompositionParagraphs : EnglishCompositionParagraphs;
    return JoinSample(paragraphs, 4, "\n");
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
