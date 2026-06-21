#include "Game.h"
#include <raylib.h>
#include "AudioManager.h"

Game::Game(int width, int height, const char* title) 
    : screenWidth(width), screenHeight(height), isRunning(true), isDarkTheme(true) {
    InitWindow(screenWidth, screenHeight, title);
    InitAudioDevice();
    AudioManager::Init();
    SetTargetFPS(60);
    currentTheme = ThemeManager::GetMidnightTheme();
    
    // Загружаем шрифт (размер 64 для четкости, потом будем скейлить)
    mainFont = LoadFontEx("assets/fonts/JetBrainsMono-Regular.ttf", 64, 0, 250);
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
