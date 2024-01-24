#include <iostream>
#include "Game/Game.h"

/*
* This engine structure has been extended and customized based on the foundation introduced in pikuma's game programming series.
*/


int main(int argc, char* argv[]) {
    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

    return 0;
}
