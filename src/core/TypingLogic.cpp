#include "TypingLogic.h"
#include <raylib.h>
#include <random>
#include "AudioManager.h"

TypingLogic::TypingLogic() : isFinished(false) {
    wordList = {
        "the", "be", "to", "of", "and", "a", "in", "that", "have", "I",
        "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
        "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
        "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
        "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
        "code", "program", "keyboard", "developer", "type", "fast", "sprint"
    };
}

void TypingLogic::GenerateText(int wordCount) {
    targetText.clear();
    for (int i = 0; i < wordCount; ++i) {
        int idx = GetRandomValue(0, wordList.size() - 1);
        targetText += wordList[idx];
        if (i < wordCount - 1) {
            targetText += " ";
        }
    }
}

void TypingLogic::StartNewTest(int wordCount) {
    GenerateText(wordCount);
    ResetProgress();
}

void TypingLogic::StartCustomTest(const std::string& text) {
    targetText = text;
    ResetProgress();
}

void TypingLogic::ResetProgress() {
    typedText.clear();
    isFinished = false;
    timeElapsed = 0.0f;
    totalKeystrokes = 0;
    correctKeystrokes = 0;
}

void TypingLogic::Update(float deltaTime) {
    if (!isFinished && !typedText.empty()) {
        timeElapsed += deltaTime;
    }
}

float TypingLogic::GetWPM() const {
    if (timeElapsed == 0.0f) return 0.0f;
    // WPM = (correct_chars / 5) / time_in_minutes
    float minutes = timeElapsed / 60.0f;
    return (correctKeystrokes / 5.0f) / minutes;
}

float TypingLogic::GetAccuracy() const {
    if (totalKeystrokes == 0) return 100.0f;
    return (static_cast<float>(correctKeystrokes) / totalKeystrokes) * 100.0f;
}

void TypingLogic::HandleInput() {
    if (isFinished) return;

    // Чтение вводимых символов
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125)) {
            if (typedText.length() < targetText.length()) {
                totalKeystrokes++;
                if (key == targetText[typedText.length()]) {
                    correctKeystrokes++;
                    AudioManager::PlayClick();
                } else {
                    AudioManager::PlayError();
                }
                typedText += (char)key;
            }
        }
        key = GetCharPressed();
    }

    // Обработка Backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!typedText.empty()) {
            typedText.pop_back();
            // Мы не уменьшаем totalKeystrokes при удалении для точности метрик
        }
    }

    if (typedText == targetText || typedText.length() == targetText.length()) {
        isFinished = true;
    }
}
