#include "Game.h"

#include <random>
#include <memory>
#include <iostream>

#include "Move.h"
#include "Pokemonster.h"
#include "BattleUI.h"

// ============= MÁQUINA DE ESTADOS MULTIJUGADOR =============
enum class BattleState 
{
    P1_TURN,          // Esperando clic del Jugador 1
    P1_ANIMATING,     // Animación del Jugador 1 en curso
    P2_TURN,          // Esperando clic del Jugador 2
    P2_ANIMATING      // Animación del Jugador 2 en curso
};

class Battle
{
public:
    Battle(sf::RenderWindow& win)
        : window(win), ui(win.getSize()), player(nullptr), enemy(nullptr), 
          state(BattleState::P1_TURN), rng(std::random_device{}()),
          pendingMove(-1)
    {
    }

    void init();
    void handleEvent(const sf::Event& event);
    void update();
    void render();
    bool isFinished() const { return finished; }

private:
    sf::RenderWindow& window;
    BattleUI ui;
    Pokemonster* player;
    Pokemonster* enemy;
    BattleState state;
    bool finished = false;

    std::mt19937 rng;

    // Movimiento pendiente del jugador actual
    int pendingMove;

    // Función helper para actualizar la UI con los movimientos del Pokémon actual
    void updateUIForCurrentPlayer();
};

// ============= INICIALIZACIÓN =============
void Battle::init()
{
    // Crear movimientos de Jugador 1 (Pikachu)
    std::vector<Move> pMoves = {
        Move("Tackle", 10, MoveType::Normal, "assets/sounds/tackle.wav", 1, 9),
        Move("Flame", 14, MoveType::Fire, "assets/sounds/flame.wav", 2, 9),
        Move("Splash", 8, MoveType::Water, "assets/sounds/splash.wav", 3, 9),
        Move("Leaf Cut", 12, MoveType::Grass, "assets/sounds/leaf.wav", 4, 9)
    };

    // Crear movimientos de Jugador 2 (Bulbasaur)
    std::vector<Move> eMoves = {
        Move("Bite", 11, MoveType::Normal, "assets/sounds/bite.wav"),
        Move("Ember", 13, MoveType::Fire, "assets/sounds/ember.wav"),
        Move("Water Jet", 9, MoveType::Water, "assets/sounds/waterjet.wav"),
        Move("Vine Lash", 12, MoveType::Grass, "assets/sounds/vinelash.wav")
    };

    // Crear Pokémons
    static Pokemonster playerP("Pikachu", 100, 20, 10, pMoves);
    static Pokemonster enemyP("Bulbasaur", 90, 18, 8, eMoves);

    player = &playerP;
    enemy = &enemyP;

    // Cargar sprites
    player->loadSpriteSheet("assets/images/1.png", 50, 50);
    enemy->loadSpriteSheet("assets/images/2.png", 100, 100);

    player->setPosition(100.f, 300.f);
    enemy->setPosition(500.f, 80.f);

    ui.setPlayerPokemon(player);
    ui.setEnemyPokemon(enemy);
    ui.update();

    // Iniciar mostrando los movimientos del Jugador 1
    updateUIForCurrentPlayer();
    std::cout << "\n=== MULTIJUGADOR LOCAL - ASIENTO CALIENTE ===\n";
    std::cout << "Player 1 (Pikachu) - Your turn!\n\n";
}

// ============= MANEJADOR DE EVENTOS =============
void Battle::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        int choice = ui.handleMouseClick(mousePos);
        
        // Turno del Jugador 1 (player)
        if (state == BattleState::P1_TURN && choice >= 0 && choice < 4)
        {
            pendingMove = choice;
            state = BattleState::P1_ANIMATING;
            player->playAnimation(1, 4);
            
            const Move& mv = player->getMoves()[choice];
            std::cout << "\n[P1 TURN] " << player->getName() << " uses " << mv.name << "!\n";
        }
        // Turno del Jugador 2 (enemy)
        else if (state == BattleState::P2_TURN && choice >= 0 && choice < 4)
        {
            pendingMove = choice;
            state = BattleState::P2_ANIMATING;
            enemy->playAnimation(1, 4);
            
            const Move& mv = enemy->getMoves()[choice];
            std::cout << "\n[P2 TURN] " << enemy->getName() << " uses " << mv.name << "!\n";
        }
    }
}

// ============= ACTUALIZAR UI PARA JUGADOR ACTUAL =============
void Battle::updateUIForCurrentPlayer()
{
    if (state == BattleState::P1_TURN)
    {
        // Mostrar movimientos del Jugador 1 (player)
        ui.updateMoveButtons(player);
        std::cout << "[UI UPDATE] Showing Player 1 moves\n";
    }
    else if (state == BattleState::P2_TURN)
    {
        // Mostrar movimientos del Jugador 2 (enemy)
        ui.updateMoveButtons(enemy);
        std::cout << "[UI UPDATE] Showing Player 2 moves\n";
    }
}

// ============= BUCLE DE ACTUALIZACIÓN =============
void Battle::update()
{
    // SIEMPRE actualizar las animaciones en cada frame
    if (player) player->updateAnimation();
    if (enemy) enemy->updateAnimation();

    // Máquina de estados para multijugador local
    switch (state)
    {
        case BattleState::P1_TURN:
            // Esperando que el Jugador 1 haga clic
            break;

        case BattleState::P1_ANIMATING:
            // Animación del Jugador 1 ejecutándose
            if (!player->isAnimating())
            {
                // ✅ Animación terminó -> Aplicar daño al Jugador 2
                if (pendingMove >= 0 && pendingMove < 4)
                {
                    const Move& mv = player->getMoves()[pendingMove];
                    int damage = std::max(1, mv.power + player->getAttack() - enemy->getDefense());
                    
                    enemy->takeDamage(damage);
                    ui.update();
                    std::cout << "[DAMAGE] " << damage << " to Player 2! P2 HP: " 
                              << enemy->getHP() << "/" << enemy->getHPMax() << "\n";
                }

                // Verificar si P2 se debilitó
                if (enemy->isFainted())
                {
                    std::cout << "\n🎉 Player 2 fainted! Player 1 wins!\n";
                    finished = true;
                }
                else
                {
                    // Transicionar al turno del Jugador 2
                    state = BattleState::P2_TURN;
                    updateUIForCurrentPlayer();
                    std::cout << "\n>>> Passing control to Player 2 <<<\n";
                }
                
                pendingMove = -1;
            }
            break;

        case BattleState::P2_TURN:
            // Esperando que el Jugador 2 haga clic
            break;

        case BattleState::P2_ANIMATING:
            // Animación del Jugador 2 ejecutándose
            if (!enemy->isAnimating())
            {
                // ✅ Animación terminó -> Aplicar daño al Jugador 1
                if (pendingMove >= 0 && pendingMove < 4)
                {
                    const Move& mv = enemy->getMoves()[pendingMove];
                    int damage = std::max(1, mv.power + enemy->getAttack() - player->getDefense());
                    
                    player->takeDamage(damage);
                    ui.update();
                    std::cout << "[DAMAGE] " << damage << " to Player 1! P1 HP: " 
                              << player->getHP() << "/" << player->getHPMax() << "\n";
                }

                // Verificar si P1 se debilitó
                if (player->isFainted())
                {
                    std::cout << "\n🎉 Player 1 fainted! Player 2 wins!\n";
                    finished = true;
                }
                else
                {
                    // Transicionar al turno del Jugador 1
                    state = BattleState::P1_TURN;
                    updateUIForCurrentPlayer();
                    std::cout << "\n>>> Passing control to Player 1 <<<\n";
                }
                
                pendingMove = -1;
            }
            break;
    }
}

// ============= RENDERIZADO =============
void Battle::render()
{
    window.clear(sf::Color(30, 30, 30));

    // Dibujar pokémons
    if (enemy) window.draw(*enemy);
    if (player) window.draw(*player);

    // Dibujar UI encima
    ui.draw(window);

    window.display();
}

// ============= CLASE GAME =============
Game::Game() : isRunning(true)
{
    window.create(sf::VideoMode({800, 600}), "Pokemon Game - Multiplayer");
    window.setFramerateLimit(60);
}

Game::~Game()
{
    window.close();
}

void Game::run()
{
    Battle battle(window);
    battle.init();

    while (window.isOpen() && isRunning && !battle.isFinished())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                isRunning = false;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
                isRunning = false;
            }

            // Pasar evento a la batalla
            battle.handleEvent(event);
        }

        battle.update();
        battle.render();
    }
}

void Game::handleEvents()
{
    // Ahora manejado dentro de run/battle
}

void Game::update()
{
}

void Game::render()
{
}
