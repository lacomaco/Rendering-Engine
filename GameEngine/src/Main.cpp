#include "./Game/Game.h"
#include <sol/sol.hpp>
#include <iostream>
#include "./Input/Input.h"

int main(int argc, char* argv[]) {
    Game game;

    Input* input = Input::GetInstance();
    game.Initialize();
    game.Run();
    game.Destroy();
    
    return 0;
}
