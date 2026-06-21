#include "Game.h"
#include <raylib.h>
#include <algorithm>
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
    
    // Загружаем латиницу и кириллицу, чтобы EN/RU уроки рисовались одним шрифтом.
    std::vector<int> glyphs;
    for (int codepoint = 32; codepoint <= 126; ++codepoint) glyphs.push_back(codepoint);
    for (int codepoint = 0x0401; codepoint <= 0x0451; ++codepoint) glyphs.push_back(codepoint);
    mainFont = LoadFontEx("assets/fonts/JetBrainsMono-Regular.ttf", 64, glyphs.data(), static_cast<int>(glyphs.size()));
    SetTextureFilter(mainFont.texture, TEXTURE_FILTER_BILINEAR);
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
        const int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
        fullscreen = true;
    } else {
        ToggleFullscreen();
        SetWindowSize(windowedWidth, windowedHeight);
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
