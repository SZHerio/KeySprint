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
    void ApplyDraft();
    bool HasPendingChanges() const;

    Game* gamePtr = nullptr;
    int draftThemeIndex = 0;
    Language draftLanguage = Language::English;
    Difficulty draftDifficulty = Difficulty::Normal;
    int draftTypingTextFontIndex = 0;
    int draftKeyboardFontIndex = 0;
    bool draftAudioEnabled = true;
    int draftClickProfile = 0;
    float draftVolume = 0.65f;
};
