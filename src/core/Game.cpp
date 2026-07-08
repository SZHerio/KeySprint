#include "Game.h"
#include <raylib.h>
#include <algorithm>
#include <string>
#include <vector>
#include "AudioManager.h"

namespace {
const char* TypingFontLabels[Game::TypingFontCount] = {
    "JetBrains Mono",
    "Cascadia Mono",
    "Cascadia Code",
    "Consolas"
};

Font LoadFirstAvailableFont(const std::vector<std::string>& candidates, std::vector<int>& glyphs) {
    Font font = {};
    for (const std::string& path : candidates) {
        if (!FileExists(path.c_str())) {
            continue;
        }
        font = LoadFontEx(path.c_str(), 64, glyphs.data(), static_cast<int>(glyphs.size()));
        if (font.texture.id != 0) {
            SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
            break;
        }
    }
    return font;
}

int ClampFontIndex(int index) {
    return std::clamp(index, 0, Game::TypingFontCount - 1);
}
}

Game::Game(int width, int height, const char* title) 
    : screenWidth(width), screenHeight(height), isRunning(true), windowedWidth(width), windowedHeight(height), language(Language::English) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, title);
    Image windowIcon = LoadImage("assets/icons/key-sprint-icon.png");
    if (windowIcon.data != nullptr) {
        SetWindowIcon(windowIcon);
        UnloadImage(windowIcon);
    }
    SetExitKey(KEY_NULL);
    SetWindowMinSize(960, 540);
    InitAudioDevice();
    AudioManager::Init();
    SetTargetFPS(60);
    progress.Load();
    SetThemeIndex(progress.GetThemeIndex());
    
    // Загружаем жирный системный шрифт с кириллицей. Assets могут отсутствовать в чистой сборке.
    std::vector<int> glyphs;
    for (int codepoint = 32; codepoint <= 126; ++codepoint) glyphs.push_back(codepoint);
    for (int codepoint = 0x0400; codepoint <= 0x052F; ++codepoint) glyphs.push_back(codepoint);
    glyphs.push_back(0x00AB);
    glyphs.push_back(0x00BB);
    glyphs.push_back(0x2014);

    const std::vector<std::vector<std::string>> typingFontCandidates = {
        {
            "assets/fonts/JetBrainsMono-Regular.ttf",
            "assets/fonts/JetBrainsMono-Bold.ttf",
            "C:/Windows/Fonts/consola.ttf",
            "C:/Windows/Fonts/consolab.ttf"
        },
        {
            "assets/fonts/CascadiaMono.ttf",
            "C:/Windows/Fonts/CascadiaMono.ttf",
            "C:/Windows/Fonts/consola.ttf"
        },
        {
            "assets/fonts/CascadiaCode.ttf",
            "C:/Windows/Fonts/CascadiaCode.ttf",
            "C:/Windows/Fonts/consola.ttf"
        },
        {
            "C:/Windows/Fonts/consola.ttf",
            "C:/Windows/Fonts/consolab.ttf",
            "assets/fonts/JetBrainsMono-Regular.ttf"
        }
    };

    const std::vector<std::string> uiFontCandidates = {
        "assets/fonts/Inter-Variable.ttf",
        "assets/fonts/Manrope-Variable.ttf",
        "assets/fonts/IBMPlexSans-Regular.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (int index = 0; index < TypingFontCount; ++index) {
        typingFonts[index] = LoadFirstAvailableFont(typingFontCandidates[index], glyphs);
        typingFontLoaded[index] = typingFonts[index].texture.id != 0;
    }

    uiFont = LoadFirstAvailableFont(uiFontCandidates, glyphs);
    uiFontLoaded = uiFont.texture.id != 0;
}

Game::~Game() {
    if (uiFontLoaded) {
        UnloadFont(uiFont);
    }
    for (int index = 0; index < TypingFontCount; ++index) {
        if (typingFontLoaded[index]) {
            UnloadFont(typingFonts[index]);
        }
    }
    AudioManager::Unload();
    CloseAudioDevice();
    CloseWindow();
}

void Game::ChangeState(std::shared_ptr<GameState> newState) {
    currentState = newState;
    if (currentState) {
        currentState->Init(this);
    }
}

Font Game::GetFont() const {
    return GetTypingTextFont();
}

Font Game::GetTypingFontByIndex(int index) const {
    const int selected = ClampFontIndex(index);
    if (typingFontLoaded[selected]) {
        return typingFonts[selected];
    }

    for (int index = 0; index < TypingFontCount; ++index) {
        if (typingFontLoaded[index]) {
            return typingFonts[index];
        }
    }

    return GetFontDefault();
}

Font Game::GetTypingTextFont() const {
    return GetTypingFontByIndex(progress.GetTypingTextFontIndex());
}

Font Game::GetKeyboardFont() const {
    return GetTypingFontByIndex(progress.GetKeyboardFontIndex());
}

Font Game::GetUiFont() const {
    return uiFontLoaded ? uiFont : GetTypingTextFont();
}

const char* Game::GetTypingFontLabel(int index) const {
    return TypingFontLabels[ClampFontIndex(index)];
}

const char* Game::GetTypingTextFontLabel() const {
    return GetTypingFontLabel(progress.GetTypingTextFontIndex());
}

const char* Game::GetKeyboardFontLabel() const {
    return GetTypingFontLabel(progress.GetKeyboardFontIndex());
}

void Game::CycleTypingTextFont() {
    progress.CycleTypingTextFont(TypingFontCount);
}

void Game::CycleKeyboardFont() {
    progress.CycleKeyboardFont(TypingFontCount);
}

void Game::SetThemeIndex(int index) {
    themeIndex = ThemeManager::ClampIndex(index);
    currentTheme = ThemeManager::GetTheme(themeIndex);
    progress.SetThemeIndex(themeIndex, ThemeManager::ThemeCount);
}

void Game::CycleTheme() {
    SetThemeIndex((themeIndex + 1) % ThemeManager::ThemeCount);
}

void Game::Run() {
    while (!WindowShouldClose() && isRunning) {
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreenMode();
        }

        if (currentState) {
            currentState->HandleInput();
            currentState->Update(GetFrameTime());

            BeginDrawing();
            ClearBackground(currentTheme.Background);
            currentState->Draw();
            EndDrawing();
        } else {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("No state initialized!", 10, 10, 20, RED);
            EndDrawing();
        }
    }
}

void Game::Quit() {
    isRunning = false;
}

void Game::ToggleLanguage() {
    SetLanguage(language == Language::English ? Language::Russian : Language::English);
}

void Game::ToggleFullscreenMode() {
    if (!fullscreen) {
        windowedWidth = GetScreenWidth();
        windowedHeight = GetScreenHeight();
        const Vector2 position = GetWindowPosition();
        windowedX = static_cast<int>(position.x);
        windowedY = static_cast<int>(position.y);
        const int monitor = GetCurrentMonitor();
        const Vector2 monitorPosition = GetMonitorPosition(monitor);
        SetWindowState(FLAG_WINDOW_UNDECORATED);
        SetWindowPosition(static_cast<int>(monitorPosition.x), static_cast<int>(monitorPosition.y));
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        fullscreen = true;
    } else {
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
        SetWindowSize(windowedWidth, windowedHeight);
        SetWindowPosition(windowedX, windowedY);
        fullscreen = false;
    }
}

int Game::GetWindowWidth() const {
    return GetScreenWidth();
}

int Game::GetWindowHeight() const {
    return GetScreenHeight();
}

float Game::GetUiScale() const {
    const float scaleX = static_cast<float>(GetWindowWidth()) / VirtualWidth;
    const float scaleY = static_cast<float>(GetWindowHeight()) / VirtualHeight;
    return std::min(scaleX, scaleY);
}

Vector2 Game::ScalePoint(Vector2 point) const {
    const float scale = GetUiScale();
    const float offsetX = (GetWindowWidth() - VirtualWidth * scale) * 0.5f;
    const float offsetY = (GetWindowHeight() - VirtualHeight * scale) * 0.5f;
    return { offsetX + point.x * scale, offsetY + point.y * scale };
}

Rectangle Game::ScaleRect(Rectangle rect) const {
    const float scale = GetUiScale();
    const Vector2 origin = ScalePoint({ rect.x, rect.y });
    return { origin.x, origin.y, rect.width * scale, rect.height * scale };
}
