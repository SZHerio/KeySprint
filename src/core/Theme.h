#pragma once
#include <raylib.h>

struct Theme {
    Color Background;
    Color TextDefault;
    Color TextCorrect;
    Color TextError;
    Color Caret;
    Color Title;
};

class ThemeManager {
public:
    static Theme GetLightTheme() {
        return {
            RAYWHITE,     // Background
            LIGHTGRAY,    // TextDefault
            DARKGRAY,     // TextCorrect
            RED,          // TextError
            DARKGRAY,     // Caret
            DARKBLUE      // Title
        };
    }

    static Theme GetMidnightTheme() {
        return {
            { 15, 15, 25, 255 },    // Background
            { 100, 100, 120, 255 }, // TextDefault
            { 220, 220, 240, 255 }, // TextCorrect
            { 255, 80, 80, 255 },   // TextError
            { 200, 200, 255, 255 }, // Caret
            { 150, 150, 200, 255 }  // Title
        };
    }
};
