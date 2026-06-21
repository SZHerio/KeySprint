#include "Game.h"
#include <raylib.h>
#include <algorithm>
#include <string>
#include <vector>
#include "AudioManager.h"

Game::Game(int width, int height, const char* title) 
    : screenWidth(width), screenHeight(height), isRunning(true), windowedWidth(width), windowedHeight(height), isDarkTheme(true), language(Language::English) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, title);
    SetExitKey(KEY_NULL);
    SetWindowMinSize(960, 540);
    InitAudioDevice();
    AudioManager::Init();
    SetTargetFPS(60);
    currentTheme = ThemeManager::GetMidnightTheme();
    progress.Load();
    
    // Загружаем жирный системный шрифт с кириллицей. Assets могут отсутствовать в чистой сборке.
    std::vector<int> glyphs;
    for (int codepoint = 32; codepoint <= 126; ++codepoint) glyphs.push_back(codepoint);
    for (int codepoint = 0x0400; codepoint <= 0x052F; ++codepoint) glyphs.push_back(codepoint);
    glyphs.push_back(0x00AB);
    glyphs.push_back(0x00BB);
    glyphs.push_back(0x2014);

    const std::vector<std::string> fontCandidates = {
        "assets/fonts/JetBrainsMono-Bold.ttf",
        "assets/fonts/JetBrainsMono-Regular.ttf",
        "C:/Windows/Fonts/consolab.ttf",
        "C:/Windows/Fonts/arialbd.ttf"
    };

    for (const std::string& path : fontCandidates) {
        if (!FileExists(path.c_str())) {
            continue;
        }
        mainFont = LoadFontEx(path.c_str(), 64, glyphs.data(), static_cast<int>(glyphs.size()));
        if (mainFont.texture.id != 0) {
            break;
        }
    }

    if (mainFont.texture.id != 0) {
        SetTextureFilter(mainFont.texture, TEXTURE_FILTER_BILINEAR);
    }
}

Game::~Game() {
    UnloadFont(mainFont);
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
    language = language == Language::English ? Language::Russian : Language::English;
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
    return std::max(0.65f, std::min(scaleX, scaleY));
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
