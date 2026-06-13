#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Pokemonster.h"
#include "BattleUI.h"
#include "Move.h"
#include <cstdlib>
#include <ctime>

struct CharacterData;

class Game
{
public:
    Game(int p1Index, int p2Index, int bgIndex = -1);
    ~Game();
    void run();

private:
    void handleEvents();
    void update();
    void render();

    Pokemonster buildCharacter(int index, bool isPlayer);

    // PLAYER1_TURN: espera click de J1 en sus botones (derecha)
    // PLAYER2_TURN: espera click de J2 en sus botones (izquierda)
    // BATTLE_OVER : alguien ganó
    enum class TurnState { PLAYER1_TURN, PLAYER2_TURN, BATTLE_OVER };

    sf::RenderWindow window;
    bool isRunning;

    TurnState currentTurn;
    bool      waitingForInput;    // true = esperando click del jugador activo
    bool      animationPlaying;

    sf::Clock animDelayClock;     // pausa entre turno y turno
    bool      delayScheduled;     // esperando la pausa antes de cambiar turno

    Pokemonster player;   // Jugador 1
    Pokemonster enemy;    // Jugador 2

    BattleUI battleUI;

    sf::Texture battleBgTexture;
    sf::Sprite  battleBg;
};

#endif // GAME_H
