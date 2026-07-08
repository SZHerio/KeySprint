#pragma once
#include "../core/AppTypes.h"
#include "../core/GameState.h"

class SettingsState : public GameState {
public:
    void Init(Game* game) override;
    void HandleInput() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    void LoadDraftFromCurrent();
    void SetDraftThemeIndex(int index, int direction);
    void SetDraftInterfaceLanguage(Language language);
    void SetDraftTypingLanguage(Language language);
    void SetDraftDifficulty(Difficulty difficulty);
    void SetDraftUiFontIndex(int index, int direction);
    void SetDraftTypingTextFontIndex(int index, int direction);
    void SetDraftKeyboardFontIndex(int index, int direction);
    void SetDraftFocusModeEnabled(bool enabled);
    void SetDraftUiDensity(UiDensity density);
    void SetDraftAudioEnabled(bool enabled);
    void SetDraftClickProfile(int profile);
    void SetDraftVolume(float volume);

    Game* gamePtr = nullptr;
    int draftThemeIndex = 0;
    Language draftInterfaceLanguage = Language::Russian;
    Language draftTypingLanguage = Language::English;
    Difficulty draftDifficulty = Difficulty::Normal;
    int draftUiFontIndex = 0;
    int draftTypingTextFontIndex = 0;
    int draftKeyboardFontIndex = 0;
    bool draftFocusModeEnabled = true;
    UiDensity draftUiDensity = UiDensity::Normal;
    bool draftAudioEnabled = true;
    int draftClickProfile = 0;
    float draftVolume = 0.65f;
    int selectedRow = 0;
    float interfaceLanguagePosition = 1.0f;
    float typingLanguagePosition = 0.0f;
    float difficultyPosition = 1.0f;
    float focusModePosition = 1.0f;
    float uiDensityPosition = 1.0f;
    float clickProfilePosition = 0.0f;
    float soundPosition = 1.0f;
    float volumeVisual = 0.65f;
    float themePulse = 0.0f;
    float themeDirection = 1.0f;
    float uiFontPulse = 0.0f;
    float uiFontDirection = 1.0f;
    float typingFontPulse = 0.0f;
    float typingFontDirection = 1.0f;
    float keyboardFontPulse = 0.0f;
    float keyboardFontDirection = 1.0f;
};
