#include "KeyboardLayout.h"
#include "TextUtils.h"

#include <algorithm>
#include <initializer_list>

std::vector<KeyLayout> BuildKeyboardLayout(Language language) {
    std::vector<KeyLayout> keys;

    auto addKey = [&keys](int row, int key, const std::string& label, float width = 1.0f, std::initializer_list<int> aliases = {}) {
        std::vector<int> codepoints{ key };
        for (int alias : aliases) {
            if (std::find(codepoints.begin(), codepoints.end(), alias) == codepoints.end()) {
                codepoints.push_back(alias);
            }
        }

        keys.push_back({
            key,
            label,
            row,
            width,
            GetFingerForKey(key),
            codepoints
        });
    };

    auto addPlainUtf8Row = [&addKey](int row, const std::string& rowText) {
        for (int key : Utf8ToCodepoints(rowText)) {
            addKey(row, key, CodepointToUtf8(key));
        }
    };

    auto addPlainCodepointRow = [&addKey](int row, std::initializer_list<int> rowKeys) {
        for (int key : rowKeys) {
            addKey(row, key, CodepointToUtf8(key));
        }
    };

    addKey(0, '`', "`", 1.0f, { '~' });
    addKey(0, '1', "1\n!", 1.0f, { '!' });
    addKey(0, '2', "2", 1.0f);
    addKey(0, '3', "3", 1.0f);
    addKey(0, '4', "4", 1.0f);
    addKey(0, '5', "5", 1.0f);
    addKey(0, '6', "6", 1.0f);
    addKey(0, '7', "7", 1.0f);
    addKey(0, '8', "8", 1.0f);
    addKey(0, '9', "9\n(", 1.0f, { '(' });
    addKey(0, '0', "0\n)", 1.0f, { ')' });
    addKey(0, '-', "-\n_", 1.0f, { '_' });
    addKey(0, '=', "=", 1.0f);
    addKey(0, '\b', "BKSP", 1.8f);

    if (language == Language::Russian) {
        addPlainCodepointRow(1, { 0x0439, 0x0446, 0x0443, 0x043a, 0x0435, 0x043d, 0x0433, 0x0448, 0x0449, 0x0437 });
        addKey(1, 0x0445, u8"х\n[", 1.0f, { '[', '{' });
        addKey(1, 0x044a, u8"ъ\n]", 1.0f, { ']', '}' });
        addKey(1, '\\', "\\\n|", 1.0f, { '|' });

        addPlainCodepointRow(2, { 0x0444, 0x044b, 0x0432, 0x0430, 0x043f, 0x0440, 0x043e, 0x043b, 0x0434 });
        addKey(2, 0x0436, u8"ж\n;", 1.0f, { ';', ':' });
        addKey(2, 0x044d, u8"э\n'", 1.0f, { '\'', '"' });

        addPlainCodepointRow(3, { 0x044f, 0x0447, 0x0441, 0x043c, 0x0438, 0x0442, 0x044c });
        addKey(3, 0x0431, u8"б\n<", 1.0f, { ',', '<' });
        addKey(3, 0x044e, u8"ю\n>", 1.0f, { '.', '>' });
        addKey(3, '/', "/\n?", 1.0f, { '?' });
    } else {
        addPlainUtf8Row(1, "qwertyuiop");
        addKey(1, '[', "[\n{", 1.0f, { '{' });
        addKey(1, ']', "]\n}", 1.0f, { '}' });
        addKey(1, '\\', "\\\n|", 1.0f, { '|' });

        addPlainUtf8Row(2, "asdfghjkl");
        addKey(2, ';', ";\n:", 1.0f, { ':' });
        addKey(2, '\'', "'\n\"", 1.0f, { '"' });

        addPlainUtf8Row(3, "zxcvbnm");
        addKey(3, ',', ",\n<", 1.0f, { '<' });
        addKey(3, '.', ".\n>", 1.0f, { '>' });
        addKey(3, '/', "/\n?", 1.0f, { '?' });
    }

    addKey(4, ' ', language == Language::Russian ? u8"ПРОБЕЛ" : "SPACE", 5.4f);
    addKey(4, '\n', "ENTER", 2.0f);

    return keys;
}

FingerType GetFingerForKey(int key) {
    switch (ToLowerCodepoint(key)) {
        case 'q':
        case '`':
        case '~':
        case '1':
        case '!':
        case 0x0439:
        case 'a':
        case 0x0444:
        case 'z':
        case 0x044f:
            return FingerType::LeftPinky;
        case 'w':
        case '2':
        case 0x0446:
        case 's':
        case 0x044b:
        case 'x':
        case 0x0447:
            return FingerType::LeftRing;
        case 'e':
        case '3':
        case 0x0443:
        case 'd':
        case 0x0432:
        case 'c':
        case 0x0441:
            return FingerType::LeftMiddle;
        case 'r':
        case '4':
        case 0x043a:
        case 't':
        case '5':
        case 0x0435:
        case 'f':
        case 0x0430:
        case 'g':
        case 0x043f:
        case 'v':
        case 0x043c:
        case 'b':
        case 0x0438:
            return FingerType::LeftIndex;
        case ' ':
            return FingerType::RightThumb;
        case 'y':
        case '6':
        case 0x043d:
        case 'u':
        case '7':
        case 0x0433:
        case 'h':
        case 0x0440:
        case 'j':
        case 0x043e:
        case 'n':
        case 0x0442:
        case 'm':
        case 0x044c:
            return FingerType::RightIndex;
        case 'i':
        case '8':
        case 0x0448:
        case 'k':
        case 0x043b:
        case ',':
        case '<':
        case 0x0431:
            return FingerType::RightMiddle;
        case 'o':
        case '9':
        case '(':
        case 0x0449:
        case 'l':
        case 0x0434:
        case '.':
        case '>':
        case 0x044e:
            return FingerType::RightRing;
        case 'p':
        case '0':
        case ')':
        case '-':
        case '_':
        case '=':
        case '\b':
        case '\n':
        case '\'':
        case '"':
        case '[':
        case '{':
        case ']':
        case '}':
        case '\\':
        case '|':
        case '/':
        case '?':
        case ':':
        case ';':
        case 0x0437:
        case 0x0445:
        case 0x044a:
        case 0x0436:
        case 0x044d:
            return FingerType::RightPinky;
        default:
            return FingerType::None;
    }
}

Color GetFingerColor(FingerType finger, const Theme& theme) {
    switch (finger) {
        case FingerType::LeftPinky:
        case FingerType::RightPinky:
            return theme.Fingers.Pinky;
        case FingerType::LeftRing:
        case FingerType::RightRing:
            return theme.Fingers.Ring;
        case FingerType::LeftMiddle:
        case FingerType::RightMiddle:
            return theme.Fingers.Middle;
        case FingerType::LeftIndex:
        case FingerType::RightIndex:
            return theme.Fingers.Index;
        case FingerType::LeftThumb:
        case FingerType::RightThumb:
            return theme.Fingers.Thumb;
        default:
            return theme.TextDefault;
    }
}

float GetKeyboardRowOffsetUnits(int row) {
    switch (row) {
        case 0: return 0.0f;
        case 1: return 1.25f;
        case 2: return 1.62f;
        case 3: return 2.12f;
        case 4: return 4.65f;
        default: return 0.0f;
    }
}

bool KeyMatchesCodepoint(const KeyLayout& key, int codepoint) {
    const int normalized = ToLowerCodepoint(codepoint);
    for (int candidate : key.codepoints) {
        if (ToLowerCodepoint(candidate) == normalized) {
            return true;
        }
    }
    return false;
}

std::string WeakIdForCodepoint(int codepoint) {
    if (codepoint == ' ') {
        return "Space";
    }
    if (codepoint == '\n') {
        return "Enter";
    }
    return CodepointToUtf8(codepoint);
}

int WeakCountForKey(const KeyLayout& key, const std::map<std::string, int>& weakKeys) {
    int total = 0;
    for (int codepoint : key.codepoints) {
        const auto found = weakKeys.find(WeakIdForCodepoint(codepoint));
        if (found != weakKeys.end()) {
            total += found->second;
        }
    }
    return total;
}
