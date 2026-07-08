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
    static constexpr int TypingFontCount = 4;

    Game(int width, int height, const char* title);
    ~Game();

    void Run();
    void ChangeState(std::shared_ptr<GameState> newState);
    void Quit();
    void ToggleFullscreenMode();

    const Theme& GetTheme() const { return currentTheme; }
    int GetThemeIndex() const { return themeIndex; }
    int GetThemeCount() const { return ThemeManager::ThemeCount; }
    const char* GetThemeLabel(int index) const { return ThemeManager::GetThemeLabel(index); }
    bool IsDarkTheme() const { return themeIndex != ThemeManager::DaylightThemeIndex; }
    void SetThemeIndex(int index);
    void CycleTheme();
    void SetDarkTheme(bool darkTheme) { SetThemeIndex(darkTheme ? ThemeManager::MidnightThemeIndex : ThemeManager::DaylightThemeIndex); }
    void ToggleTheme() { CycleTheme(); }
    
    Font GetFont() const;
    Font GetTypingFontByIndex(int index) const;
    Font GetTypingTextFont() const;
    Font GetKeyboardFont() const;
    Font GetUiFont() const;
    int GetTypingFontCount() const { return TypingFontCount; }
    const char* GetTypingFontLabel(int index) const;
    const char* GetTypingTextFontLabel() const;
    const char* GetKeyboardFontLabel() const;
    void CycleTypingTextFont();
    void CycleKeyboardFont();
    Language GetLanguage() const { return language; }
    void SetLanguage(Language newLanguage) { language = newLanguage; }
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
    bool fullscreen = false;
    int windowedWidth;
    int windowedHeight;
    int windowedX = 80;
    int windowedY = 80;

    Theme currentTheme;
    int themeIndex = ThemeManager::MidnightThemeIndex;
    Font typingFonts[TypingFontCount] = {};
    bool typingFontLoaded[TypingFontCount] = {};
    Font uiFont = {};
    bool uiFontLoaded = false;
    Language language;
    ProgressManager progress;
};
