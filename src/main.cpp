#include "../include/Game.h"
#include "../include/MenuScreen.h"

int main()
{
    MenuScreen menu;
    menu.run();

    if (menu.shouldLaunchGame())
    {
        Game game;
        game.run();
    }

    return 0;
}
