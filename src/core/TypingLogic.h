#pragma once
#include <map>
#include <string>
#include <vector>

class TypingLogic {
public:
    TypingLogic();
    
    void StartNewTest(int wordCount);
    void StartCustomTest(const std::string& text);
    void HandleInput();
    void Update(float deltaTime);

    const std::string& GetTargetText() const { return targetText; }
    const std::string& GetTypedText() const { return typedText; }
    const std::vector<int>& GetTargetCodepoints() const { return targetCodepoints; }
    const std::vector<int>& GetTypedCodepoints() const { return typedCodepoints; }
    std::map<std::string, int> GetMistakeCountsUtf8() const;

    bool IsFinished() const { return isFinished; }
    
    // Метрики
    float GetWPM() const;
    float GetAccuracy() const;
    float GetTimeElapsed() const { return timeElapsed; }

private:
    std::vector<std::string> wordList;
    std::string targetText;
    std::string typedText;
    std::vector<int> targetCodepoints;
    std::vector<int> typedCodepoints;
    std::map<int, int> mistakeCounts;
    bool isFinished;
    
    float timeElapsed;
    int totalKeystrokes;
    int correctKeystrokes;

    void GenerateText(int wordCount);
    void ResetProgress();
    void RebuildTypedText();
};
