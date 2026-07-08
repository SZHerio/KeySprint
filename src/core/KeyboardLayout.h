#pragma once

#include <string>
#include <vector>
#include <raylib.h>
#include "AppTypes.h"
#include "Theme.h"

enum class FingerType {
    None,
    LeftPinky,
    LeftRing,
    LeftMiddle,
    LeftIndex,
    LeftThumb,
    RightThumb,
    RightIndex,
    RightMiddle,
    RightRing,
    RightPinky
};

struct KeyLayout {
    int key;
    std::string label;
    int row;
    float width;
    FingerType finger;
};

std::vector<KeyLayout> BuildKeyboardLayout(Language language);
FingerType GetFingerForKey(int key);
Color GetFingerColor(FingerType finger, const Theme& theme);
