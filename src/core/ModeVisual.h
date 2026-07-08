#pragma once
#include <raylib.h>
#include "TypingMode.h"

struct ModeVisualStyle {
    Color Accent;
    Color SoftAccent;
    const char* LabelEn;
    const char* LabelRu;
    const char* ToneEn;
    const char* ToneRu;
    const char* Mark;
};

inline ModeVisualStyle GetModeVisualStyle(TypingMode mode) {
    switch (mode) {
        case TypingMode::Tutorial:
            return {
                { 112, 169, 255, 255 },
                { 65, 94, 152, 255 },
                "Course Map",
                u8"Карта курса",
                "structured route",
                u8"маршрут уроков",
                "LS"
            };
        case TypingMode::Daily:
            return {
                { 255, 197, 87, 255 },
                { 144, 101, 45, 255 },
                "Daily Event",
                u8"Событие дня",
                "one focused run",
                u8"одна цель дня",
                "DY"
            };
        case TypingMode::Composition:
            return {
                { 199, 151, 255, 255 },
                { 119, 79, 154, 255 },
                "Long Distance",
                u8"Длинная дистанция",
                "endurance flow",
                u8"длинный ритм",
                "CP"
            };
        case TypingMode::Practice:
        default:
            return {
                { 99, 230, 213, 255 },
                { 47, 121, 130, 255 },
                "Free Run",
                u8"Свободный забег",
                "short focus drills",
                u8"короткий фокус",
                "PR"
            };
    }
}
