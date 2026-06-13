#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Pokemonster.h"
#include "BattleUI.h"
#include "Move.h"
#include <cstdlib>
#include <ctime>

// Datos del personaje seleccionado pasados desde CharacterSelect
struct CharacterData;

class Game
{
public:
    // Constructor que recibe los índices de personaje y el fondo elegido (0-2)
    Game(int p1Index, int p2Index, int bgIndex = -1);
    ~Game();
    void run();

private:
    void handleEvents();
    void update();
    void render();

    // Inicializa un Pokemonster según el índice (0-5)
    Pokemonster buildCharacter(int index, bool isPlayer);

    enum class TurnState { PLAYER_TURN, ENEMY_TURN, BATTLE_OVER };

    sf::RenderWindow window;
    bool isRunning;

    TurnState currentTurn;
    bool waitingForPlayer;
    bool animationPlaying;

    sf::Clock turnClock;
    bool enemyAttackScheduled;

    Pokemonster player;
    Pokemonster enemy;

    BattleUI battleUI;

    // Fondo de batalla
    sf::Texture battleBgTexture;
    sf::Sprite  battleBg;
};

#endif // GAME_H
