#pragma once
#include <memory>
#include <raylib.h>
#include "GameState.h"
#include "Theme.h"

class Game {
public:
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

private:
    std::shared_ptr<GameState> currentState;
    int screenWidth;
    int screenHeight;
    bool isRunning;

    Theme currentTheme;
    bool isDarkTheme;
    Font mainFont;
};
