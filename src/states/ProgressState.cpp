#include "ProgressState.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <raylib.h>
#include "../core/Game.h"
#include "../core/LessonLibrary.h"
#include "../core/TextUtils.h"
#include "MainMenuState.h"

namespace {
bool IsRu(Game* game) {
    return game->GetLanguage() == Language::Russian;
}

Color HoverOutline(Game* game) {
    return game->IsDarkTheme() ? WHITE : BLACK;
}

const char* LocalRank(const std::string& rank, bool ru) {
    if (!ru) return rank.c_str();
    if (rank == "Master") return u8"Мастер";
    if (rank == "Swift") return u8"Скоростной";
    if (rank == "Steady") return u8"Уверенный";
    return u8"Новичок";
}

const char* LocalDifficulty(const std::string& difficulty, bool ru) {
    if (!ru) return difficulty.c_str();
    if (difficulty == "Relaxed") return u8"Легкая";
    if (difficulty == "Strict") return u8"Строгая";
    return u8"Нормальная";
}

const char* LocalFinger(const std::string& finger, bool ru) {
    if (!ru) return finger.c_str();
    if (finger == "Left pinky") return u8"левый мизинец";
    if (finger == "Left ring") return u8"левый безымянный";
    if (finger == "Left middle") return u8"левый средний";
    if (finger == "Left index") return u8"левый указательный";
    if (finger == "Right index") return u8"правый указательный";
    if (finger == "Right middle") return u8"правый средний";
    if (finger == "Right ring") return u8"правый безымянный";
    if (finger == "Right pinky") return u8"правый мизинец";
    if (finger == "Thumb") return u8"большой палец";
    return u8"нет данных";
}

struct HeatKey {
    std::string id;
    std::string label;
    int row = 0;
    float width = 1.0f;
    int finger = 0;
};

int FingerBucketForKey(int key) {
    switch (ToLowerCodepoint(key)) {
        case 'q':
        case 0x0439:
        case 'a':
        case 0x0444:
        case 'z':
        case 0x044f:
        case 'p':
        case 0x0437:
        case 0x0445:
        case 0x044a:
        case ';':
        case '-':
        case 0x0436:
        case 0x044d:
        case 0x0431:
        case 0x044e:
        case '\n':
            return 0;
        case 'w':
        case 0x0446:
        case 's':
        case 0x044b:
        case 'x':
        case 0x0447:
        case 'o':
        case 0x0449:
        case 'l':
        case 0x0434:
        case '.':
            return 1;
        case 'e':
        case 0x0443:
        case 'd':
        case 0x0432:
        case 'c':
        case 0x0441:
        case ',':
        case 'i':
        case 0x0448:
        case 'k':
        case 0x043b:
            return 2;
        case 'r':
        case 0x043a:
        case 't':
        case 0x0435:
        case 'f':
        case 0x0430:
        case 'g':
        case 0x043f:
        case 'v':
        case 0x043c:
        case 'b':
        case 0x0438:
        case 'y':
        case 0x043d:
        case 'u':
        case 0x0433:
        case 'h':
        case 0x0440:
        case 'j':
        case 0x043e:
        case 'n':
        case 0x0442:
        case 'm':
        case 0x044c:
            return 3;
        case ' ':
        default:
            return 4;
    }
}

std::vector<HeatKey> BuildHeatKeys(Language language) {
    std::vector<HeatKey> keys;
    auto addRow = [&](int row, const std::string& source) {
        for (int codepoint : Utf8ToCodepoints(source)) {
            const std::string label = CodepointToUtf8(codepoint);
            keys.push_back({ label, label, row, 1.0f, FingerBucketForKey(codepoint) });
        }
    };

    if (language == Language::Russian) {
        addRow(0, u8"йцукенгшщзхъ");
        addRow(1, u8"фывапролджэ");
        addRow(2, u8"ячсмитьбю,.-");
        keys.push_back({ "Space", u8"ПРОБЕЛ", 3, 5.2f, 4 });
    } else {
        addRow(0, "qwertyuiop-");
        addRow(1, "asdfghjkl;");
        addRow(2, "zxcvbnm,.");
        keys.push_back({ "Space", "SPACE", 3, 5.2f, 4 });
    }
    keys.push_back({ "Enter", "ENTER", 3, 2.0f, 0 });
    return keys;
}

Color FingerColor(int finger, const Theme& theme) {
    switch (finger) {
        case 0: return theme.Fingers.Pinky;
        case 1: return theme.Fingers.Ring;
        case 2: return theme.Fingers.Middle;
        case 3: return theme.Fingers.Index;
        case 4:
        default: return theme.Fingers.Thumb;
    }
}

int WeakCountForKey(const std::map<std::string, int>& weakKeys, const std::string& id) {
    const auto found = weakKeys.find(id);
    return found == weakKeys.end() ? 0 : found->second;
}

void DrawSceneText(Game* game, Font font, const char* text, Vector2 position, float fontSize, Color color) {
    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), fontSize * scale, 1.0f * scale, color);
}

void DrawFittedSceneText(Game* game, Font font, const char* text, Vector2 position, float maxWidth, float fontSize, Color color) {
    float adjustedSize = fontSize;
    while (adjustedSize > 11.0f && MeasureTextEx(font, text, adjustedSize, 0.0f).x > maxWidth) {
        adjustedSize -= 1.0f;
    }

    const float scale = game->GetUiScale();
    DrawTextEx(font, text, game->ScalePoint(position), adjustedSize * scale, 0.0f, color);
}

void DrawMetricCard(Game* game, Font font, const Theme& theme, Rectangle rect, const char* label, const char* value, Color accent) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.14f, 12, Fade(theme.PanelBorder, 0.18f));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.14f, 12, Fade(accent, 0.65f));
    DrawSceneText(game, font, label, { rect.x + 24.0f, rect.y + 22.0f }, 16.0f, theme.TextDefault);
    DrawSceneText(game, font, value, { rect.x + 24.0f, rect.y + 54.0f }, 30.0f, accent);
}

void DrawKeyboardHeatmap(Game* game, Font font, const Theme& theme, Rectangle rect, Language language, const std::map<std::string, int>& weakKeys, bool ru) {
    const std::vector<HeatKey> keys = BuildHeatKeys(language);
    int maxCount = 0;
    for (const HeatKey& key : keys) {
        maxCount = std::max(maxCount, WeakCountForKey(weakKeys, key.id));
    }

    DrawRectangleRounded(game->ScaleRect(rect), 0.08f, 12, Fade(theme.PanelBorder, 0.14f));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.08f, 12, Fade(theme.PanelBorder, 0.50f));
    DrawSceneText(game, font, ru ? u8"Карта клавиш" : "Keyboard Heatmap", { rect.x + 24.0f, rect.y + 20.0f }, 23.0f, theme.Title);
    DrawSceneText(game, font, ru ? u8"Ярче = больше промахов" : "Brighter keys have more misses", { rect.x + 24.0f, rect.y + 48.0f }, 14.0f, theme.TextDefault);

    const float keySize = 27.0f;
    const float gap = 5.0f;
    const float startY = rect.y + 84.0f;

    for (int row = 0; row <= 3; ++row) {
        std::vector<HeatKey> rowKeys;
        for (const HeatKey& key : keys) {
            if (key.row == row) {
                rowKeys.push_back(key);
            }
        }

        float totalWidth = 0.0f;
        for (const HeatKey& key : rowKeys) {
            totalWidth += key.width * keySize;
        }
        totalWidth += std::max(0, static_cast<int>(rowKeys.size()) - 1) * gap;

        float x = rect.x + (rect.width - totalWidth) * 0.5f;
        const float y = startY + row * (keySize + gap);

        for (const HeatKey& key : rowKeys) {
            const int count = WeakCountForKey(weakKeys, key.id);
            const float intensity = maxCount > 0 ? std::sqrt(static_cast<float>(count) / static_cast<float>(maxCount)) : 0.0f;
            const Color finger = FingerColor(key.finger, theme);
            const Rectangle keyRect = { x, y, key.width * keySize, keySize };
            const Color fill = count > 0 ? Fade(finger, 0.20f + intensity * 0.56f) : Fade(theme.PanelBorder, 0.10f);
            const Color border = count > 0 ? Fade(theme.TextError, 0.25f + intensity * 0.55f) : Fade(theme.PanelBorder, 0.35f);

            DrawRectangleRounded(game->ScaleRect(keyRect), 0.22f, 8, fill);
            DrawRectangleRoundedLines(game->ScaleRect(keyRect), 0.22f, 8, border);

            const float labelSize = key.width > 2.0f ? 10.0f : (language == Language::Russian ? 13.0f : 14.0f);
            const Vector2 labelSizeVec = MeasureTextEx(font, key.label.c_str(), labelSize, 0.0f);
            DrawTextEx(font, key.label.c_str(), game->ScalePoint({ keyRect.x + (keyRect.width - labelSizeVec.x) * 0.5f, keyRect.y + 7.0f }), labelSize * game->GetUiScale(), 0.0f, count > 0 ? theme.TextCorrect : Fade(theme.TextDefault, 0.72f));

            if (count > 0 && key.width <= 1.2f) {
                DrawCircleV(game->ScalePoint({ keyRect.x + keyRect.width - 5.0f, keyRect.y + 5.0f }), (2.0f + intensity * 2.0f) * game->GetUiScale(), Fade(theme.TextError, 0.78f));
            }

            x += keyRect.width + gap;
        }
    }

    if (maxCount == 0) {
        DrawSceneText(game, font, ru ? u8"Ошибок пока нет. Пройди тренировку, и карта оживет." : "No misses yet. Complete a drill to light up the map.", { rect.x + 24.0f, rect.y + rect.height - 34.0f }, 14.0f, theme.TextDefault);
    } else {
        DrawSceneText(game, font, TextFormat(ru ? u8"Самая горячая клавиша: %d ошибок" : "Hottest key: %d misses", maxCount), { rect.x + 24.0f, rect.y + rect.height - 34.0f }, 14.0f, theme.TextDefault);
    }
}

void DrawSessionTrend(Game* game, Font font, const Theme& theme, Rectangle rect, const std::vector<SessionRecord>& sessions, bool ru) {
    DrawRectangleRounded(game->ScaleRect(rect), 0.08f, 12, Fade(theme.PanelBorder, 0.14f));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.08f, 12, Fade(theme.PanelBorder, 0.50f));
    DrawSceneText(game, font, ru ? u8"Последние сессии" : "Recent Sessions", { rect.x + 22.0f, rect.y + 18.0f }, 21.0f, theme.Title);

    if (sessions.size() < 2) {
        DrawFittedSceneText(game, font, ru ? u8"Нужно хотя бы 2 завершенных прохода для графика." : "Finish at least 2 runs to see the trend.", { rect.x + 22.0f, rect.y + 60.0f }, rect.width - 44.0f, 15.0f, theme.TextDefault);
        return;
    }

    const Rectangle plot = { rect.x + 28.0f, rect.y + 58.0f, rect.width - 56.0f, rect.height - 92.0f };
    DrawRectangleRounded(game->ScaleRect(plot), 0.05f, 8, Fade(theme.Panel, 0.28f));
    DrawLineEx(game->ScalePoint({ plot.x, plot.y + plot.height }), game->ScalePoint({ plot.x + plot.width, plot.y + plot.height }), 1.0f * game->GetUiScale(), Fade(theme.PanelBorder, 0.45f));

    const int count = static_cast<int>(sessions.size());
    auto pointFor = [&](int i, float value) {
        const float x = plot.x + (count == 1 ? 0.0f : static_cast<float>(i) / static_cast<float>(count - 1) * plot.width);
        const float y = plot.y + plot.height * (1.0f - std::clamp(value / 100.0f, 0.0f, 1.0f));
        return game->ScalePoint({ x, y });
    };

    for (int i = 1; i < count; ++i) {
        DrawLineEx(pointFor(i - 1, sessions[i - 1].wpm), pointFor(i, sessions[i].wpm), 2.0f * game->GetUiScale(), Fade(theme.Highlight, 0.82f));
        DrawLineEx(pointFor(i - 1, sessions[i - 1].accuracy), pointFor(i, sessions[i].accuracy), 2.0f * game->GetUiScale(), Fade(theme.Fingers.Index, 0.78f));
    }

    const SessionRecord& previous = sessions[count - 2];
    const SessionRecord& current = sessions[count - 1];
    const float wpmDelta = current.wpm - previous.wpm;
    const float accDelta = current.accuracy - previous.accuracy;
    DrawSceneText(game, font, TextFormat("WPM %+.0f", wpmDelta), { rect.x + 24.0f, rect.y + rect.height - 26.0f }, 14.0f, wpmDelta >= 0.0f ? theme.Highlight : theme.TextError);
    DrawSceneText(game, font, TextFormat("ACC %+.0f%%", accDelta), { rect.x + 120.0f, rect.y + rect.height - 26.0f }, 14.0f, accDelta >= 0.0f ? theme.Fingers.Index : theme.TextError);
}

void DrawFingerLoad(Game* game, Font font, const Theme& theme, Rectangle rect, Language language, const std::map<std::string, int>& weakKeys, bool ru) {
    int loads[5] = {};
    const std::vector<HeatKey> keys = BuildHeatKeys(language);
    for (const HeatKey& key : keys) {
        loads[key.finger] += WeakCountForKey(weakKeys, key.id);
    }

    int maxLoad = 0;
    for (int load : loads) {
        maxLoad = std::max(maxLoad, load);
    }

    DrawRectangleRounded(game->ScaleRect(rect), 0.08f, 12, Fade(theme.PanelBorder, 0.14f));
    DrawRectangleRoundedLines(game->ScaleRect(rect), 0.08f, 12, Fade(theme.PanelBorder, 0.50f));
    DrawSceneText(game, font, ru ? u8"Нагрузка пальцев" : "Finger Load", { rect.x + 22.0f, rect.y + 18.0f }, 21.0f, theme.Title);

    const char* labelsEn[5] = { "Pinky", "Ring", "Middle", "Index", "Thumb" };
    const char* labelsRu[5] = { u8"Мизинец", u8"Безым.", u8"Средний", u8"Указ.", u8"Большой" };
    for (int i = 0; i < 5; ++i) {
        const float y = rect.y + 58.0f + i * 24.0f;
        const float ratio = maxLoad > 0 ? static_cast<float>(loads[i]) / static_cast<float>(maxLoad) : 0.0f;
        DrawSceneText(game, font, ru ? labelsRu[i] : labelsEn[i], { rect.x + 22.0f, y - 2.0f }, 13.0f, theme.TextDefault);
        DrawRectangleRounded(game->ScaleRect({ rect.x + 98.0f, y + 3.0f, 180.0f, 10.0f }), 0.5f, 8, Fade(theme.PanelBorder, 0.20f));
        DrawRectangleRounded(game->ScaleRect({ rect.x + 98.0f, y + 3.0f, 180.0f * ratio, 10.0f }), 0.5f, 8, Fade(FingerColor(i, theme), 0.36f + ratio * 0.48f));
        DrawSceneText(game, font, TextFormat("%d", loads[i]), { rect.x + 288.0f, y - 2.0f }, 13.0f, loads[i] > 0 ? theme.TextCorrect : Fade(theme.TextDefault, 0.62f));
    }
}
}

void ProgressState::Init(Game* game) {
    gamePtr = game;
}

void ProgressState::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        gamePtr->ChangeState(std::make_shared<MainMenuState>());
    }

    if (IsKeyPressed(KEY_R)) {
        gamePtr->GetProgress().Reset();
    }

    const Vector2 mouse = GetMousePosition();
    const bool resetHover = CheckCollisionPointRec(mouse, gamePtr->ScaleRect({ 830.0f, 94.0f, 250.0f, 32.0f }));
    if (resetHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        gamePtr->GetProgress().Reset();
    }
    SetMouseCursor(resetHover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void ProgressState::Update(float deltaTime) {
    animTime += deltaTime;
}

void ProgressState::Draw() {
    const Theme& theme = gamePtr->GetTheme();
    const Font font = gamePtr->GetUiFont();
    const ProgressManager& progress = gamePtr->GetProgress();
    const Language language = gamePtr->GetTypingLanguage();
    const bool ru = IsRu(gamePtr);
    DrawRectangleRounded(gamePtr->ScaleRect({ 70.0f, 54.0f, 1140.0f, 610.0f }), 0.04f, 16, Fade(theme.Panel, 0.80f));
    DrawRectangleRoundedLines(gamePtr->ScaleRect({ 70.0f, 54.0f, 1140.0f, 610.0f }), 0.04f, 16, Fade(theme.PanelBorder, 0.78f));

    DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"Статистика" : "Progress Stats", { 105.0f, 92.0f }, 38.0f, theme.Title);
    DrawSceneText(gamePtr, font, IsRu(gamePtr) ? u8"ESC Меню | R Сброс прогресса" : "ESC Menu | R Reset Progress", { 800.0f, 105.0f }, 16.0f, theme.TextDefault);
    const Rectangle resetRect = { 830.0f, 94.0f, 250.0f, 32.0f };
    if (CheckCollisionPointRec(GetMousePosition(), gamePtr->ScaleRect(resetRect))) {
        DrawRectangleRoundedLines(gamePtr->ScaleRect(resetRect), 0.28f, 8, Fade(HoverOutline(gamePtr), 0.86f));
    }

    const std::string rank = progress.GetRankLabel();
    const std::string difficulty = progress.GetDifficultyLabel();
    DrawMetricCard(gamePtr, font, theme, { 110.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Ранг" : "Rank", LocalRank(rank, IsRu(gamePtr)), theme.Fingers.Index);
    DrawMetricCard(gamePtr, font, theme, { 365.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Рекорд WPM" : "Best WPM", TextFormat("%.0f", progress.GetBestWpm()), theme.Fingers.Middle);
    DrawMetricCard(gamePtr, font, theme, { 620.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Лучшая серия" : "Best Streak", TextFormat("%d", progress.GetBestStreak()), theme.Fingers.Ring);
    DrawMetricCard(gamePtr, font, theme, { 875.0f, 165.0f, 235.0f, 118.0f }, IsRu(gamePtr) ? u8"Сложность" : "Difficulty", LocalDifficulty(difficulty, IsRu(gamePtr)), theme.Fingers.Thumb);

    DrawKeyboardHeatmap(gamePtr, font, theme, { 110.0f, 306.0f, 610.0f, 295.0f }, language, progress.GetWeakKeys(), ru);
    DrawSessionTrend(gamePtr, font, theme, { 750.0f, 306.0f, 350.0f, 140.0f }, progress.GetRecentSessions(), ru);
    DrawFingerLoad(gamePtr, font, theme, { 750.0f, 461.0f, 350.0f, 140.0f }, language, progress.GetWeakKeys(), ru);

    const int englishCount = static_cast<int>(LessonLibrary::GetLessons(Language::English).size());
    const int russianCount = static_cast<int>(LessonLibrary::GetLessons(Language::Russian).size());
    const int englishUnlocked = std::min(progress.GetUnlockedLesson(Language::English) + 1, englishCount);
    const int russianUnlocked = std::min(progress.GetUnlockedLesson(Language::Russian) + 1, russianCount);
    DrawSceneText(gamePtr, font, TextFormat(IsRu(gamePtr) ? u8"Курс: EN %d/%d | RU %d/%d | Порог %.0f%% точности" : "Course: EN %d/%d | RU %d/%d | %.0f%% unlock threshold", englishUnlocked, englishCount, russianUnlocked, russianCount, progress.GetUnlockAccuracyThreshold()), { 125.0f, 622.0f }, 15.0f, theme.TextDefault);
}
