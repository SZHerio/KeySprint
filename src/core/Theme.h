#pragma once
#include <raylib.h>

struct FingerPalette {
    Color Pinky;
    Color Ring;
    Color Middle;
    Color Index;
    Color Thumb;
};

struct Theme {
    Color Background;
    Color TextDefault;
    Color TextCorrect;
    Color TextError;
    Color Caret;
    Color Title;
    Color Panel;
    Color PanelBorder;
    Color Highlight;
    FingerPalette Fingers;
};

class ThemeManager {
public:
    static Theme GetLightTheme() {
        return {
            RAYWHITE,     // Background
            { 70, 72, 88, 255 },     // TextDefault
            { 28, 32, 46, 255 },     // TextCorrect
            RED,          // TextError
            DARKGRAY,     // Caret
            DARKBLUE,     // Title
            { 248, 248, 252, 255 }, // Panel
            { 205, 210, 225, 255 }, // PanelBorder
            { 70, 100, 220, 255 },  // Highlight
            {
                { 232, 86, 111, 255 },  // Pinky
                { 236, 137, 70, 255 },  // Ring
                { 221, 184, 65, 255 },  // Middle
                { 69, 184, 133, 255 },  // Index
                { 89, 140, 224, 255 }   // Thumb
            }
        };
    }

    static Theme GetMidnightTheme() {
        return {
            { 15, 15, 25, 255 },    // Background
            { 180, 182, 210, 255 }, // TextDefault
            { 240, 240, 255, 255 }, // TextCorrect
            { 255, 80, 80, 255 },   // TextError
            { 200, 200, 255, 255 }, // Caret
            { 205, 202, 255, 255 }, // Title
            { 25, 26, 42, 255 },    // Panel
            { 56, 58, 82, 255 },    // PanelBorder
            { 159, 166, 255, 255 }, // Highlight
            {
                { 255, 94, 128, 255 },  // Pinky
                { 255, 158, 79, 255 },  // Ring
                { 246, 204, 92, 255 },  // Middle
                { 89, 214, 160, 255 },  // Index
                { 111, 159, 255, 255 }  // Thumb
            }
        };
    }
};
