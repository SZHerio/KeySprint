#pragma once
#include <raylib.h>

class AudioManager {
public:
    static void Init();
    static void Unload();

    static void PlayClick();
    static void PlayError();
    static void ToggleEnabled();
    static void IncreaseVolume();
    static void DecreaseVolume();
    static void CycleClickProfile();

    static bool IsEnabled() { return enabled; }
    static float GetVolume() { return volume; }
    static int GetClickProfile() { return clickProfile; }

private:
    static Sound clickSoundSoft;
    static Sound clickSoundBright;
    static Sound errorSound;
    static bool isLoaded;
    static bool enabled;
    static float volume;
    static int clickProfile;
};
