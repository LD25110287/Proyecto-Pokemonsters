#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Pokemonster.h"
#include "BattleUI.h"
#include "Move.h"
#include <cstdlib>
#include <ctime>

class Game
{
public:
    Game(int p1Index, int p2Index, int bgIndex = -1);
    ~Game();
    void run();

    // Devuelve quién ganó: 1 = Jugador 1, 2 = Jugador 2, 0 = sin resultado
    int getWinner() const { return winner; }

private:
    void handleEvents();
    void update();
    void render();

    Pokemonster buildCharacter(int index, bool isPlayer);

    enum class TurnState { PLAYER1_TURN, PLAYER2_TURN, BATTLE_OVER };

    sf::RenderWindow window;
    bool isRunning;
    int  winner;   // 0 = sin ganador, 1 = J1 ganó, 2 = J2 ganó

    TurnState currentTurn;
    bool      waitingForInput;
    bool      animationPlaying;

    sf::Clock animDelayClock;
    bool      delayScheduled;

    Pokemonster player;   // Jugador 1
    Pokemonster enemy;    // Jugador 2

    BattleUI battleUI;

    sf::Texture battleBgTexture;
    sf::Sprite  battleBg;
};

#endif // GAME_H
