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
                "Lesson flow",
                u8"Поток уроков",
                "LS"
            };
        case TypingMode::Daily:
            return {
                { 255, 197, 87, 255 },
                { 144, 101, 45, 255 },
                "Daily Event",
                u8"Событие дня",
                "One focused run",
                u8"Одна цель дня",
                "DY"
            };
        case TypingMode::Composition:
            return {
                { 199, 151, 255, 255 },
                { 119, 79, 154, 255 },
                "Long Distance",
                u8"Длинная дистанция",
                "Endurance flow",
                u8"Длинный ритм",
                "CP"
            };
        case TypingMode::Practice:
        default:
            return {
                { 99, 230, 213, 255 },
                { 47, 121, 130, 255 },
                "Free Run",
                u8"Свободный забег",
                "Short focus drills",
                u8"Короткий фокус",
                "PR"
            };
    }
}
