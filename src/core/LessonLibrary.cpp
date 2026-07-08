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

const std::vector<Lesson> EnglishLessons = {
    { 0, "Home Anchors", "Find the fixed home position: left and right anchors.", "asdf jkl; asdf jkl; fj dk sl a; fj dk sl a; aa ss dd ff jj kk ll ;;" },
    { 1, "Home Alternation", "Keep the hands still and switch fingers cleanly.", "as sa ad da af fa jk kj jl lj j; ;j asdf fdsa jkl; ;lkj fj dk sl a;" },
    { 2, "Index Bridge", "Train the busy index fingers before adding full rows.", "fg gf fr rf ft tf fv vf gb bg jh hj ju uj jy yj jm mj hn nh fg jh gb hn" },
    { 3, "Top Left", "Add qwert with short reaches back to home.", "qa aq ws sw ed de rf fr tg gt qwe wer ert tre qwert trewq red fed free water" },
    { 4, "Top Right", "Add yuiop and keep the wrist quiet.", "yh hy uj ju ik ki ol lo p; ;p yui iop poi oil hill join point loop" },
    { 5, "Bottom Left", "Add zxcvb with controlled downward reaches.", "za az xs sx dc cd fv vf bg gb zxc cvb bad cab fade vase cave brave" },
    { 6, "Bottom Right", "Add nm,./ and return each finger to home.", "hn nh jm mj k, ,k l. .l ;/ /; nm, m,. pin lion milk join moon line" },
    { 7, "Hand Balance", "Alternate hands so speed comes from rhythm, not force.", "far jump safe look desk point water milk brave union field calm rhythm keyboard" },
    { 8, "Common Pairs", "Practice frequent two-letter motions and double letters.", "th he in er an re on at en nd st ou ea to or it is ha ng as ll ee oo tt ss" },
    { 9, "Word Groups", "Move from drills to short useful words.", "the and you that have with this from they will your about would there their which people" },
    { 10, "Punctuation Flow", "Add comma, period and semicolon without losing posture.", "type clean words, pause briefly, then continue. accuracy first; speed follows. steady hands, clear eyes, calm rhythm." },
    { 11, "Sentence Control", "Read ahead and keep corrections small.", "slow practice builds fast typing. keep your hands quiet and let each finger return home. the next word should feel expected before you press it." },
    { 12, "Endurance Block", "Hold accuracy through several connected thoughts.", "reliable typing starts with a stable hand position.\nconsistent fingers make each key easier to predict.\nwhen the rhythm stays calm, speed grows without tension." },
    { 13, "Final Mixed Run", "A full course check with rows, words and punctuation.", "focus on accuracy, posture, and even timing. use the same finger for the same key, prepare the next movement early, and keep both hands close to home." }
};

const std::vector<Lesson> RussianLessons = {
    { 0, u8"Домашние опоры", u8"Найди устойчивую позицию ФЫВА и ОЛДЖ.", u8"фыва олдж фыва олдж аж вд ыо фл аж вд ыо фл фф ыы вв аа оо лл дд жж" },
    { 1, u8"Чередование дома", u8"Держи кисти спокойно и меняй пальцы без рывка.", u8"фы ыф ыв вы ва ав ол ло лд дл дж жд фыва авыф олдж ждло аж вд ыо фл" },
    { 2, u8"Мост указательных", u8"Отработай самые занятые указательные пальцы.", u8"ап па пр рп ак ка км мк ке ек ен не нг гн ои ио нг пр ак ке ап ои" },
    { 3, u8"Верх слева", u8"Добавь йцукен и возвращай пальцы домой.", u8"йф фй цы ыц ув ву ка ак еп пе нр рн йцу цук уке кен рука река цена урок" },
    { 4, u8"Верх справа", u8"Добавь гшщзхъ без смещения кисти.", u8"го ог шл лш щд дщ зж жз хэ эх гш шг щз зщ день окно школа город взгляд" },
    { 5, u8"Низ слева", u8"Добавь ячсми короткими контролируемыми движениями.", u8"яф фя чы ыч св вс ма ам ип пи ячс чсм сми мама сила часы миска число" },
    { 6, u8"Низ справа", u8"Добавь тьбю и точку, сохраняя домашний ряд.", u8"то от ьл ль бд дб юж жю .э э. ть ьт б ю быть люди путь июль блюдо" },
    { 7, u8"Баланс рук", u8"Чередуй руки, чтобы ритм вел скорость.", u8"рука поле вода лист дом сила урок экран клавиша точность ритм спокойствие" },
    { 8, u8"Частые пары", u8"Повтори частые переходы русского текста.", u8"ст то но на ен ов ни ра ко ро по ер ос ого ени про при что как или" },
    { 9, u8"Группы слов", u8"Переход от слогов к коротким частым словам.", u8"это как для что они она его был уже еще при все или если когда после" },
    { 10, u8"Знаки и паузы", u8"Добавь запятую и точку без потери осанки.", u8"печатай ровно, делай короткую паузу, затем продолжай. сначала точность; скорость придет. спокойные руки, ясный взгляд, ровный ритм." },
    { 11, u8"Контроль фразы", u8"Читай вперед и исправляй ошибки спокойно.", u8"медленная практика строит быструю печать. держи руки спокойно и возвращай каждый палец домой. следующее слово должно быть ожидаемым." },
    { 12, u8"Выносливость", u8"Сохрани точность в нескольких связанных строках.", u8"уверенная печать начинается с устойчивой позиции рук.\nодин палец отвечает за одну клавишу и путь становится предсказуемым.\nкогда ритм остается спокойным, скорость растет без напряжения." },
    { 13, u8"Финальный проход", u8"Проверка курса: ряды, слова и знаки.", u8"сохраняй точность, осанку и ровный темп. используй один и тот же палец для той же клавиши, готовь следующее движение заранее и держи руки рядом с домом." }
};

const std::vector<std::string> EnglishPracticeSentences = {
    R"(A quiet desk can feel like a small studio for attention.)",
    R"(Before breakfast, Mira labels the jars: tea, oats, salt, and cocoa.)",
    R"(The lamp hummed softly; the notebook stayed open beside the cup.)",
    R"(Theo asked, "Can we test the new shortcut before lunch?")",
    R"(A good recipe needs patience, heat, and one clean spoon.)",
    R"(The folder [archive] contains sketches, receipts, and a map.)",
    R"(If the plan fails, try the smaller version first.)",
    R"(Use the path C:\drafts\scene-4 before exporting the file.)",
    R"(The sign above the door read <studio>, but nobody whispered.)",
    R"(A weather app, a pencil, and a glass of water sat on the table.)",
    R"(Nora saved three colors: blue, amber, and violet.)",
    R"(The garden looked simple; every path had been measured twice.)",
    R"(When the kettle clicked, the room became suddenly cheerful.)",
    R"(Please compare the lists {morning} and {evening} after practice.)",
    R"(A clear question is better than a dramatic answer.)",
    R"(The artist's label used 'draft' for the unfinished frame.)",
    R"(Sam wrote "focus / rest / repeat" on a sticky note.)",
    R"(The bookshelf leaned left, so we placed a wedge under it.)",
    R"(Why does a tiny bell make the whole hallway feel awake?)",
    R"(The bakery opened early, and the first tray smelled like cinnamon.)",
    R"(Keep the cable straight; the left port is marked with a thin line.)",
    R"(A calm hand can cross a difficult key without panic.)",
    R"(The museum guide said, "Look at the frame, not only the painting.")",
    R"(A small lantern lit the path between the porch and the shed.)",
    R"(The note said: return the blue box before sunset.)",
    R"(I found the missing card behind the keyboard, under a receipt.)",
    R"(The task was simple, but the timing had to be exact.)",
    R"(Every shelf had a label: tools, paper, clay, thread.)",
    R"(The printer clicked once, paused, and then finished the page.)",
    R"(A narrow bridge crossed the stream; it looked stronger than it sounded.)",
    R"(Use A | B only when both choices are truly equal.)",
    R"(The final sketch included (hands), [keys], and {light}.)"
};

const std::vector<std::string> RussianPracticeSentences = {
    u8R"(Тихий стол похож на маленькую мастерскую внимания.)",
    u8R"(Перед завтраком Лина подписала банки: чай, овес, соль и какао.)",
    u8R"(Лампа гудела мягко; блокнот лежал рядом с чашкой.)",
    u8R"(Илья спросил: "Проверим новый набросок до обеда?")",
    u8R"(Хорошему рецепту нужны терпение, тепло и чистая ложка.)",
    u8R"(Папка [архив] хранит эскизы, чеки и старую карту.)",
    u8R"(Если план не сработал, попробуй меньшую версию.)",
    u8R"(Маршрут дом\сад\мастерская помог не забыть порядок дел.)",
    u8R"(Над дверью висела табличка <студия>, но внутри говорили спокойно.)",
    u8R"(На столе лежали карандаш, стакан воды и маленький календарь.)",
    u8R"(Нина выбрала три цвета: синий, янтарный и фиолетовый.)",
    u8R"(Сад казался простым; каждую дорожку измерили дважды.)",
    u8R"(Когда чайник щелкнул, комната сразу стала уютнее.)",
    u8R"(Сравни списки {утро} и {вечер} после тренировки.)",
    u8R"(Ясный вопрос лучше шумного ответа.)",
    u8R"(Мастер поставил метку 'черновик' на первой странице.)",
    u8R"(На стикере было написано: "фокус / отдых / повтор".)",
    u8R"(Полка наклонилась влево, и мы подложили тонкий клин.)",
    u8R"(Почему маленький звонок делает весь коридор бодрее?)",
    u8R"(Пекарня открылась рано, и первый противень пах корицей.)",
    u8R"(Держи кабель ровно; левый разъем отмечен тонкой линией.)",
    u8R"(Спокойная рука проходит сложную клавишу без паники.)",
    u8R"(Экскурсовод сказал: "Смотрите на раму, а не только на картину.")",
    u8R"(Маленький фонарь освещал путь от крыльца до сарая.)",
    u8R"(Записка напоминала: вернуть синюю коробку до заката.)",
    u8R"(Я нашел карточку за клавиатурой, под старым чеком.)",
    u8R"(Задача была простой, но время нужно было выбрать точно.)",
    u8R"(На каждой полке была метка: инструменты, бумага, глина, нить.)",
    u8R"(Принтер щелкнул, замолчал, а потом закончил страницу.)",
    u8R"(Узкий мост пересекал ручей; он был крепче, чем казался.)",
    u8R"(Используй А | Б только тогда, когда оба выбора равны.)",
    u8R"(Финальный эскиз включал (руки), [клавиши] и {свет}.)"
};

const std::vector<std::string> EnglishCompositionParagraphs = {
    R"(A long typing session is a small expedition through attention. At first, the hands notice every reach, every pause, and every uncertain letter. After a few minutes, the body begins to understand the route: the eyes read ahead, the fingers prepare early, and the mind stops arguing with each key.)",
    R"(The goal is not to attack the text. The goal is to stay available to it. A calm typist can correct a mistake without turning the next word into another mistake, and that calmness matters more than one brilliant burst of speed.)",
    R"(Imagine a workshop in the evening. Tools are returned to their places, a lamp warms the center of the table, and someone checks a list before closing the door. Good typing has the same quiet order: each finger leaves home, does one job, and comes back.)",
    R"(A useful practice text should not feel like random noise. It should contain ordinary images, clear punctuation, and enough variety to make the hands solve real problems. Commas, quotes, brackets, and questions are not decorations; they are part of everyday writing.)",
    R"(When you type for ten minutes or more, posture becomes a character in the story. Shoulders creep upward, wrists search for shortcuts, and the eyes start skipping. Notice those changes early, relax the grip, and let the next sentence begin with a smaller movement.)",
    R"(There is a difference between speed and hurry. Speed is organized; hurry is scattered. Speed knows where the next word is going before it arrives. Hurry presses harder, misses more often, and then spends its energy repairing the damage.)",
    R"(A page of prose trains more than fingers. It trains memory, spacing, punctuation, breath, and the small patience required to finish a paragraph without losing the thread. That is why long-form typing belongs next to short drills rather than replacing them.)",
    R"(The room in this exercise has no dramatic event. A chair is pulled closer, a cable is straightened, a window is opened, and a notebook waits. These plain details are useful because they let the hands practice rhythm without fighting complicated names or rare words.)",
    R"(If your accuracy begins to fall, do not chase the old pace. Let the tempo shrink until the letters become predictable again. The fastest route back to confidence is usually a slower sentence typed without panic.)",
    R"(A skilled typist does not merely react to the current key. The unused fingers prepare for what comes next: a shift, a bracket, an enter press, or a return to the home row. Preparation makes difficult combinations feel less sudden.)",
    R"(Consider the punctuation marks as small turns in a road. A colon opens a gate; a semicolon asks for balance; a question mark changes the voice; parentheses make a quiet aside. The hands should learn these turns as naturally as they learn letters.)",
    R"(Endurance grows from honest repetition. One paragraph teaches nothing by itself, but sixteen paragraphs can reveal a pattern: the same finger hesitates, the same hand drifts, or the same punctuation mark breaks the rhythm.)",
    R"(During this passage, let your eyes lead by two or three words. If the gaze waits on the exact letter being typed, the fingers receive instructions too late. Reading ahead gives the body time to arrange the next motion.)",
    R"(The best corrections are small. Backspace, breathe, and continue; do not punish the sentence for one wrong letter. A clean recovery is a skill, and it is often the difference between a fragile sprint and a reliable working pace.)",
    R"(By the final paragraphs, the text should feel familiar but not automatic. Keep listening to the rhythm of the keys. If the sound becomes uneven, soften the hands; if the sound becomes steady, trust it and continue.)",
    R"(When the exercise ends, notice what survived the distance: accuracy, posture, breathing, and attention. Those are the real gains. Speed is welcome, but it should arrive as a result of order, not as a demand shouted at the fingers.)"
};

const std::vector<std::string> RussianCompositionParagraphs = {
    u8R"(Длинная тренировка печати похожа на маленькое путешествие через внимание. В первые минуты руки замечают каждое движение, каждую паузу и каждую сомнительную букву. Потом тело постепенно понимает маршрут: взгляд читает вперед, пальцы готовятся заранее, а мысль перестает спорить с каждой клавишей.)",
    u8R"(Цель не в том, чтобы нападать на текст. Цель в том, чтобы оставаться с ним в контакте. Спокойный набор позволяет исправить ошибку и не превратить следующее слово в новую ошибку, а это важнее одного яркого рывка скорости.)",
    u8R"(Представь вечернюю мастерскую. Инструменты возвращены на места, лампа согревает середину стола, и перед уходом кто-то проверяет список. Хорошая печать устроена так же: палец уходит с домашней позиции, выполняет одну задачу и возвращается.)",
    u8R"(Полезный тренировочный текст не должен быть случайным шумом. В нем нужны обычные образы, ясные знаки препинания и достаточное разнообразие, чтобы руки решали настоящие задачи. Запятые, кавычки, скобки и вопросы не украшения; это часть живого письма.)",
    u8R"(Когда печатаешь десять минут или дольше, осанка становится отдельным участником истории. Плечи поднимаются, запястья ищут короткие пути, а взгляд начинает перескакивать. Замечай это рано, ослабляй хватку и начинай следующее предложение меньшим движением.)",
    u8R"(Есть разница между скоростью и спешкой. Скорость организована; спешка рассыпается. Скорость знает, куда идет следующее слово. Спешка давит сильнее, ошибается чаще и тратит силы на ремонт того, что сама сломала.)",
    u8R"(Страница связного текста тренирует не только пальцы. Она тренирует память, пробелы, пунктуацию, дыхание и терпение, которое нужно, чтобы закончить абзац и не потерять мысль. Поэтому длинный режим должен жить рядом с короткими упражнениями.)",
    u8R"(В этом упражнении нет громкого события. Стул придвинут ближе, кабель расправлен, окно приоткрыто, а блокнот ждет на столе. Такие простые детали полезны: они дают рукам тренировать ритм без борьбы с редкими словами.)",
    u8R"(Если точность начинает падать, не догоняй старый темп. Уменьши скорость до тех пор, пока буквы снова не станут предсказуемыми. Самая короткая дорога к уверенности часто проходит через более медленную фразу без паники.)",
    u8R"(Опытный наборщик не только реагирует на текущую клавишу. Свободные пальцы заранее готовятся к следующему действию: к знаку, скобке, нажатию Enter или возвращению на домашний ряд. Подготовка делает трудные сочетания менее внезапными.)",
    u8R"(Относись к знакам препинания как к поворотам дороги. Двоеточие открывает ворота; точка с запятой просит равновесия; вопрос меняет голос; скобки добавляют тихое отступление. Руки должны знать эти повороты так же естественно, как буквы.)",
    u8R"(Выносливость растет из честного повторения. Один абзац почти ничего не показывает, но шестнадцать абзацев обнаруживают закономерность: один палец медлит, одна рука смещается, один знак снова ломает ритм.)",
    u8R"(Во время этого текста веди взгляд на два или три слова вперед. Если глаза ждут ту самую букву, которую пальцы уже печатают, команда приходит слишком поздно. Чтение вперед дает телу время подготовить движение.)",
    u8R"(Лучшие исправления выглядят маленькими. Backspace, вдох, продолжение; не нужно наказывать всю фразу за одну неверную букву. Чистое восстановление тоже навык, и оно отличает хрупкий рывок от надежного рабочего темпа.)",
    u8R"(К финальным абзацам текст должен казаться знакомым, но не автоматическим. Продолжай слушать ритм клавиш. Если звук становится неровным, смягчи руки; если звук стабилен, доверься ему и продолжай.)",
    u8R"(Когда упражнение закончится, обрати внимание, что выдержало дистанцию: точность, осанка, дыхание и внимание. Это настоящая добыча длинной тренировки. Скорость приятна, но лучше, когда она приходит как результат порядка, а не как приказ пальцам.)"
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
    const auto& sentences = language == Language::Russian ? RussianPracticeSentences : EnglishPracticeSentences;
    return JoinSampleLines(sentences, 32, true);
}

std::string LessonLibrary::BuildCompositionText(Language language) {
    const auto& paragraphs = language == Language::Russian ? RussianCompositionParagraphs : EnglishCompositionParagraphs;
    return JoinSample(paragraphs, 16, "\n");
}

std::string LessonLibrary::BuildDailyChallengeText(Language language) {
    const auto& practice = language == Language::Russian ? RussianPracticeSentences : EnglishPracticeSentences;
    const auto& paragraphs = language == Language::Russian ? RussianCompositionParagraphs : EnglishCompositionParagraphs;
    return JoinSampleLines(practice, 6, false) + "\n" + JoinSample(paragraphs, 1, "\n");
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
