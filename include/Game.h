#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Pokemonster.h"
#include "BattleUI.h"
#include <cstdlib>
#include <ctime>

class Game
{
public:
    Game();
    ~Game();
    void run();
    
private:
    void handleEvents();
    void update();
    void render();

    enum class TurnState { PLAYER_TURN, ENEMY_TURN, BATTLE_OVER };

    sf::RenderWindow window;
    bool isRunning;

    // Estado del turno
    TurnState currentTurn;

    // Flags de control
    bool waitingForPlayer;
    bool animationPlaying;

    // Temporizador para pausa entre turnos
    sf::Clock turnClock;
    bool enemyAttackScheduled;

    // Personajes
    Pokemonster player;
    Pokemonster enemy;

    // UI de batalla
    BattleUI battleUI;
};

#endif // GAME_H
