#include "AudioManager.h"
#include <vector>
#include <cmath>

Sound AudioManager::clickSoundSoft = { 0 };
Sound AudioManager::clickSoundBright = { 0 };
Sound AudioManager::errorSound = { 0 };
bool AudioManager::isLoaded = false;
bool AudioManager::enabled = true;
float AudioManager::volume = 0.65f;
int AudioManager::clickProfile = 0;

// Генерация простого звука "клик" (очень мягкий механический звук)
static Wave GenerateClickWave() {
    int sampleRate = 44100;
    float duration = 0.03f; // 30ms - очень короткий
    int sampleCount = (int)(sampleRate * duration);
    
    std::vector<short> data(sampleCount);
    for (int i = 0; i < sampleCount; ++i) {
        float t = (float)i / sampleRate;
        float env = expf(-t * 200.0f); // Быстрое затухание
        
        // Низкочастотный "стук" + немного высокочастотного клика
        float val = sinf(2.0f * PI * 350.0f * t) * env; 
        
        data[i] = (short)(val * 12000.0f); // Уменьшенная громкость
    }
    
    Wave wave = { 0 };
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));
    
    for (int i = 0; i < sampleCount; ++i) {
        ((short*)wave.data)[i] = data[i];
    }
    
    return wave;
}

static Wave GenerateBrightClickWave() {
    int sampleRate = 44100;
    float duration = 0.025f;
    int sampleCount = (int)(sampleRate * duration);

    std::vector<short> data(sampleCount);
    for (int i = 0; i < sampleCount; ++i) {
        float t = (float)i / sampleRate;
        float env = expf(-t * 260.0f);
        float val = (sinf(2.0f * PI * 720.0f * t) * 0.65f + sinf(2.0f * PI * 1180.0f * t) * 0.35f) * env;
        data[i] = (short)(val * 9500.0f);
    }

    Wave wave = { 0 };
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));

    for (int i = 0; i < sampleCount; ++i) {
        ((short*)wave.data)[i] = data[i];
    }

    return wave;
}

// Генерация простого звука "ошибка" (мягкий приглушенный сигнал)
static Wave GenerateErrorWave() {
    int sampleRate = 44100;
    float duration = 0.15f; // 150ms
    int sampleCount = (int)(sampleRate * duration);
    
    std::vector<short> data(sampleCount);
    for (int i = 0; i < sampleCount; ++i) {
        float t = (float)i / sampleRate;
        
        float env = expf(-t * 30.0f); 
        // Мягкий низкий тон, как "туп"
        float val = sinf(2.0f * PI * 120.0f * t) * env;
        
        data[i] = (short)(val * 16000.0f);
    }
    
    Wave wave = { 0 };
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));
    
    for (int i = 0; i < sampleCount; ++i) {
        ((short*)wave.data)[i] = data[i];
    }
    
    return wave;
}

void AudioManager::Init() {
    if (isLoaded) return;
    
    Wave clickWave = GenerateClickWave();
    clickSoundSoft = LoadSoundFromWave(clickWave);
    UnloadWave(clickWave);

    Wave brightClickWave = GenerateBrightClickWave();
    clickSoundBright = LoadSoundFromWave(brightClickWave);
    UnloadWave(brightClickWave);
    
    Wave errorWave = GenerateErrorWave();
    errorSound = LoadSoundFromWave(errorWave);
    UnloadWave(errorWave);
    
    isLoaded = true;
}

void AudioManager::Unload() {
    if (!isLoaded) return;
    UnloadSound(clickSoundSoft);
    UnloadSound(clickSoundBright);
    UnloadSound(errorSound);
    isLoaded = false;
}

void AudioManager::PlayClick() {
    if (!isLoaded || !enabled) return;

    Sound& sound = clickProfile == 0 ? clickSoundSoft : clickSoundBright;
    SetSoundVolume(sound, volume);
    PlaySound(sound);
}

void AudioManager::PlayError() {
    if (!isLoaded || !enabled) return;

    SetSoundVolume(errorSound, volume);
    PlaySound(errorSound);
}

void AudioManager::ToggleEnabled() {
    SetEnabled(!enabled);
}

void AudioManager::SetEnabled(bool value) {
    enabled = value;
}

void AudioManager::IncreaseVolume() {
    SetVolume(volume + 0.1f);
}

void AudioManager::DecreaseVolume() {
    SetVolume(volume - 0.1f);
}

void AudioManager::SetVolume(float value) {
    volume = fmaxf(0.0f, fminf(1.0f, value));
}

void AudioManager::CycleClickProfile() {
    SetClickProfile(clickProfile + 1);
}

void AudioManager::SetClickProfile(int profile) {
    clickProfile = profile <= 0 ? 0 : 1;
}
