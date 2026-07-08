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
    static constexpr int MidnightThemeIndex = 0;
    static constexpr int DaylightThemeIndex = 1;
    static constexpr int GraphiteThemeIndex = 2;
    static constexpr int EmberThemeIndex = 3;
    static constexpr int NeonThemeIndex = 4;
    static constexpr int RubyThemeIndex = 5;
    static constexpr int ThemeCount = 6;

    static int ClampIndex(int index) {
        if (index < 0) return MidnightThemeIndex;
        if (index >= ThemeCount) return ThemeCount - 1;
        return index;
    }

    static const char* GetThemeLabel(int index) {
        switch (ClampIndex(index)) {
            case DaylightThemeIndex: return "Daylight";
            case GraphiteThemeIndex: return "Graphite";
            case EmberThemeIndex: return "Ember";
            case NeonThemeIndex: return "Neon";
            case RubyThemeIndex: return "Ruby";
            case MidnightThemeIndex:
            default: return "Midnight";
        }
    }

    static const char* GetThemeLabelRu(int index) {
        switch (ClampIndex(index)) {
            case DaylightThemeIndex: return u8"Светлая";
            case GraphiteThemeIndex: return u8"Графит";
            case EmberThemeIndex: return u8"Теплая";
            case NeonThemeIndex: return u8"Неон";
            case RubyThemeIndex: return u8"Рубин";
            case MidnightThemeIndex:
            default: return u8"Ночная";
        }
    }

    static Theme GetTheme(int index) {
        switch (ClampIndex(index)) {
            case DaylightThemeIndex: return GetLightTheme();
            case GraphiteThemeIndex: return GetGraphiteTheme();
            case EmberThemeIndex: return GetEmberTheme();
            case NeonThemeIndex: return GetNeonTheme();
            case RubyThemeIndex: return GetRubyTheme();
            case MidnightThemeIndex:
            default: return GetMidnightTheme();
        }
    }

    static Theme GetLightTheme() {
        return {
            { 245, 247, 251, 255 },
            { 70, 76, 92, 255 },
            { 28, 34, 46, 255 },
            { 218, 63, 82, 255 },
            { 64, 77, 102, 255 },
            { 33, 46, 80, 255 },
            { 255, 255, 255, 255 },
            { 207, 215, 229, 255 },
            { 72, 113, 228, 255 },
            {
                { 232, 86, 111, 255 },
                { 236, 137, 70, 255 },
                { 221, 184, 65, 255 },
                { 69, 184, 133, 255 },
                { 89, 140, 224, 255 }
            }
        };
    }

    static Theme GetMidnightTheme() {
        return {
            { 15, 15, 25, 255 },
            { 180, 182, 210, 255 },
            { 240, 240, 255, 255 },
            { 255, 80, 80, 255 },
            { 200, 200, 255, 255 },
            { 205, 202, 255, 255 },
            { 25, 26, 42, 255 },
            { 56, 58, 82, 255 },
            { 159, 166, 255, 255 },
            {
                { 255, 94, 128, 255 },
                { 255, 158, 79, 255 },
                { 246, 204, 92, 255 },
                { 89, 214, 160, 255 },
                { 111, 159, 255, 255 }
            }
        };
    }

    static Theme GetGraphiteTheme() {
        return {
            { 18, 21, 24, 255 },
            { 188, 196, 204, 255 },
            { 244, 247, 249, 255 },
            { 255, 92, 116, 255 },
            { 108, 224, 204, 255 },
            { 241, 246, 248, 255 },
            { 29, 34, 39, 255 },
            { 69, 78, 88, 255 },
            { 93, 219, 196, 255 },
            {
                { 233, 92, 126, 255 },
                { 234, 147, 83, 255 },
                { 226, 199, 88, 255 },
                { 91, 201, 145, 255 },
                { 116, 162, 236, 255 }
            }
        };
    }

    static Theme GetEmberTheme() {
        return {
            { 24, 20, 20, 255 },
            { 204, 191, 181, 255 },
            { 255, 246, 236, 255 },
            { 255, 88, 92, 255 },
            { 255, 184, 96, 255 },
            { 255, 231, 207, 255 },
            { 38, 31, 30, 255 },
            { 86, 69, 61, 255 },
            { 255, 157, 76, 255 },
            {
                { 255, 92, 115, 255 },
                { 255, 151, 76, 255 },
                { 236, 196, 92, 255 },
                { 94, 203, 151, 255 },
                { 112, 166, 240, 255 }
            }
        };
    }

    static Theme GetNeonTheme() {
        return {
            { 7, 10, 24, 255 },
            { 206, 196, 238, 255 },
            { 252, 244, 255, 255 },
            { 255, 78, 146, 255 },
            { 217, 92, 255, 255 },
            { 236, 220, 255, 255 },
            { 14, 18, 42, 255 },
            { 71, 50, 104, 255 },
            { 190, 89, 255, 255 },
            {
                { 255, 83, 157, 255 },
                { 255, 176, 74, 255 },
                { 247, 230, 96, 255 },
                { 119, 99, 255, 255 },
                { 207, 118, 255, 255 }
            }
        };
    }

    static Theme GetRubyTheme() {
        return {
            { 21, 8, 15, 255 },
            { 218, 190, 200, 255 },
            { 255, 244, 248, 255 },
            { 255, 91, 112, 255 },
            { 255, 113, 148, 255 },
            { 255, 226, 234, 255 },
            { 36, 16, 25, 255 },
            { 91, 43, 61, 255 },
            { 225, 57, 95, 255 },
            {
                { 255, 88, 131, 255 },
                { 244, 138, 83, 255 },
                { 229, 188, 93, 255 },
                { 95, 208, 151, 255 },
                { 126, 160, 239, 255 }
            }
        };
    }
};
