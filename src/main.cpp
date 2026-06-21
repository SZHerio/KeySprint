#include "core/Game.h"
#include "states/MainMenuState.h"

int main() {
    Game game(800, 600, "KeySprint - Prototype");
    
    // Начальное состояние
    game.ChangeState(std::make_shared<MainMenuState>());
    
    game.Run();

    return 0;
}

