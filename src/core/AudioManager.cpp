#include "AudioManager.h"
#include <vector>
#include <cmath>

Sound AudioManager::clickSound = { 0 };
Sound AudioManager::errorSound = { 0 };
bool AudioManager::isLoaded = false;

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
    clickSound = LoadSoundFromWave(clickWave);
    UnloadWave(clickWave);
    
    Wave errorWave = GenerateErrorWave();
    errorSound = LoadSoundFromWave(errorWave);
    UnloadWave(errorWave);
    
    isLoaded = true;
}

void AudioManager::Unload() {
    if (!isLoaded) return;
    UnloadSound(clickSound);
    UnloadSound(errorSound);
    isLoaded = false;
}

void AudioManager::PlayClick() {
    if (isLoaded) PlaySound(clickSound);
}

void AudioManager::PlayError() {
    if (isLoaded) PlaySound(errorSound);
}
