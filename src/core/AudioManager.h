#pragma once
#include <raylib.h>

class AudioManager {
public:
    static void Init();
    static void Unload();

    static void PlayClick();
    static void PlayError();

private:
    static Sound clickSound;
    static Sound errorSound;
    static bool isLoaded;
};
