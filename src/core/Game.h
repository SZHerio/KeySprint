#pragma once
#include <memory>
#include <raylib.h>
#include "AppTypes.h"
#include "GameState.h"
#include "ProgressManager.h"
#include "Theme.h"

class Game {
public:
    static constexpr float VirtualWidth = 1280.0f;
    static constexpr float VirtualHeight = 720.0f;

    Game(int width, int height, const char* title);
    ~Game();

    void Run();
    void ChangeState(std::shared_ptr<GameState> newState);
    void Quit();

    const Theme& GetTheme() const { return currentTheme; }
    void ToggleTheme() { 
        isDarkTheme = !isDarkTheme;
        currentTheme = isDarkTheme ? ThemeManager::GetMidnightTheme() : ThemeManager::GetLightTheme();
    }
    
    Font GetFont() const { return mainFont; }
    Language GetLanguage() const { return language; }
    void ToggleLanguage();
    ProgressManager& GetProgress() { return progress; }
    const ProgressManager& GetProgress() const { return progress; }
    int GetWindowWidth() const;
    int GetWindowHeight() const;
    float GetUiScale() const;
    Vector2 ScalePoint(Vector2 point) const;
    Rectangle ScaleRect(Rectangle rect) const;

private:
    std::shared_ptr<GameState> currentState;
    int screenWidth;
    int screenHeight;
    bool isRunning;

    Theme currentTheme;
    bool isDarkTheme;
    Font mainFont;
    Language language;
    ProgressManager progress;
};
