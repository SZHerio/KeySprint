#include "core/Game.h"
#include "states/MainMenuState.h"

int main() {
    Game game(1280, 720, "Key Sprint");
    
    // Начальное состояние
    game.ChangeState(std::make_shared<MainMenuState>());
    
    game.Run();

    return 0;
}

