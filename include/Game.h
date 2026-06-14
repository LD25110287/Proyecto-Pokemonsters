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
    // Constructor normal (primera ronda o personaje nuevo para ambos)
    Game(int p1Index, int p2Index, int bgIndex = -1);

    // Constructor KoF: uno o ambos personajes vienen de la ronda anterior
    // con su vida/energía actuales. Si el índice es -1 se reutiliza el
    // Pokemonster pasado directamente.
    Game(int p1Index, int p2Index, int bgIndex,
         Pokemonster* survivorP1,   // nullptr = crear nuevo
         Pokemonster* survivorP2,   // nullptr = crear nuevo
         int roundNumber);

    ~Game();
    void run();

    int getWinner() const { return winner; }

    // Acceso al sobreviviente para pasarlo a la siguiente ronda
    Pokemonster& getPlayer()  { return player; }
    Pokemonster& getEnemy()   { return enemy;  }

private:
    void handleEvents();
    void update();
    void render();
    void drawRoundAnnouncement(); // pantalla "Ronda X"

    Pokemonster buildCharacter(int index, bool isPlayer);
    void        initBattleUI(int p1Index, int p2Index);

    enum class TurnState  { PLAYER1_TURN, PLAYER2_TURN, BATTLE_OVER };
    enum class GamePhase  { ANNOUNCING, BATTLE };   // fase actual

    sf::RenderWindow window;
    bool isRunning;
    int  winner;

    GamePhase phase;
    int       roundNumber;
    sf::Clock announceClock;   // temporizador para la pantalla de anuncio

    TurnState currentTurn;
    bool      waitingForInput;
    bool      animationPlaying;

    sf::Clock animDelayClock;
    bool      delayScheduled;

    Pokemonster player;
    Pokemonster enemy;

    BattleUI battleUI;

    sf::Texture battleBgTexture;
    sf::Sprite  battleBg;
    int         bgIndex_;

    sf::Font font_;   // fuente para el anuncio de ronda
};

#endif // GAME_H
