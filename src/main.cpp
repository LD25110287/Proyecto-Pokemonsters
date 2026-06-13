#include "../include/Game.h"
#include "../include/MenuScreen.h"
#include "../include/CharacterSelect.h"

int main()
{
    // 1. Menú principal
    MenuScreen menu;
    menu.run();

    if (!menu.shouldLaunchGame())
        return 0;

    // 2. Selección de personajes
    CharacterSelect selector;
    selector.run();

    if (!selector.shouldLaunchBattle())
        return 0;

    // 3. Batalla con los personajes elegidos
    Game game(selector.getPlayer1Choice(), selector.getPlayer2Choice());
    game.run();

    return 0;
}
