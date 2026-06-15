#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Pokemonster.h"
#include "BattleUI.h"
#include "Move.h"
#include <array>
#include <cstdlib>
#include <ctime>

class Game
{
public:
    // Constructor normal (primera ronda, sin equipo previo — usa slot 0 de cada equipo)
    Game(int p1Index, int p2Index, int bgIndex = -1);

    // Constructor con equipos completos de 3 personajes (desde CharacterSelect)
    Game(const std::array<int,3>& p1Team,
         const std::array<int,3>& p2Team,
         int bgIndex = -1);

    // Constructor KoF: uno o ambos personajes vienen de la ronda anterior
    Game(int p1Index, int p2Index, int bgIndex,
         Pokemonster* survivorP1,
         Pokemonster* survivorP2,
         int roundNumber);

    // Constructor KoF con equipos completos
    Game(const std::array<int,3>& p1Team,
         const std::array<int,3>& p2Team,
         int bgIndex,
         Pokemonster* survivorP1,
         Pokemonster* survivorP2,
         int roundNumber,
         int p1ActiveSlot = 0,
         int p2ActiveSlot = 0);

    ~Game();
    void run();

    int getWinner() const { return winner; }

    Pokemonster& getPlayer() { return player; }
    Pokemonster& getEnemy()  { return enemy;  }

    // Equipos para pasar a la siguiente ronda
    const std::array<int,3>& getP1Team() const { return p1Team_; }
    const std::array<int,3>& getP2Team() const { return p2Team_; }
    int getP1ActiveSlot() const { return p1ActiveSlot_; }
    int getP2ActiveSlot() const { return p2ActiveSlot_; }

private:
    void handleEvents();
    void update();
    void render();
    void drawRoundAnnouncement();

    Pokemonster buildCharacter(int index, bool isPlayer);
    void        initBattleUI(int p1Index, int p2Index);

    // Ejecuta el cambio de personaje para el jugador activo
    // slot: índice 0-2 dentro del equipo del jugador
    // Devuelve false si el slot está inválido o el personaje ya está activo
    bool executeSwitch(bool isP1, int slot);

    enum class TurnState  { PLAYER1_TURN, PLAYER2_TURN, BATTLE_OVER };
    enum class GamePhase  { ANNOUNCING, BATTLE };

    sf::RenderWindow window;
    bool isRunning;
    int  winner;

    GamePhase phase;
    int       roundNumber;
    sf::Clock announceClock;

    TurnState currentTurn;
    bool      waitingForInput;
    bool      animationPlaying;

    sf::Clock animDelayClock;
    bool      delayScheduled;

    // ── Equipos ───────────────────────────────────────────────────────────────
    std::array<int,3> p1Team_;       // índices de los 3 personajes de J1
    std::array<int,3> p2Team_;       // índices de los 3 personajes de J2
    int               p1ActiveSlot_; // cuál slot (0-2) está en campo J1
    int               p2ActiveSlot_; // cuál slot (0-2) está en campo J2

    Pokemonster player;
    Pokemonster enemy;

    BattleUI battleUI;

    sf::Texture battleBgTexture;
    sf::Sprite  battleBg;
    int         bgIndex_;

    sf::Font font_;
};

#endif // GAME_H
