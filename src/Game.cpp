#include "Game.h"

#include <random>
#include <memory>
#include <iostream>

#include "Move.h"
#include "Pokemonster.h"
#include "BattleUI.h"

enum class BattleState 
{
    WAITING_FOR_PLAYER,    // Esperando que el jugador elija ataque
    PLAYER_ANIMATING,      // El jugador está reproduciendo su animación de ataque
    PLAYER_DELAY,          // Pausa después del ataque del jugador
    ENEMY_ANIMATING,       // El enemigo está reproduciendo su animación de ataque
    ENEMY_DELAY,           // Pausa después del ataque del enemigo
    CHECK_FAINTED          // Verificar si alguien se debilitó
};

class Battle
{
public:
    Battle(sf::RenderWindow& win)
        : window(win), ui(win.getSize()), player(nullptr), enemy(nullptr), 
          state(BattleState::WAITING_FOR_PLAYER), rng(std::random_device{}()),
          delayDuration(sf::seconds(1.5f)), pendingPlayerMove(-1), pendingEnemyMove(-1)
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

    // Temporizador y control de delays
    sf::Clock battleClock;
    sf::Time delayDuration;

    // Variables para almacenar el movimiento pendiente
    int pendingPlayerMove;
    int pendingEnemyMove;
};

// Implementación mínima de la batalla
void Battle::init()
{
    // Crear movimientos de ejemplo
    std::vector<Move> pMoves = {
    Move("Tackle", 10, MoveType::Normal, "assets/sounds/tackle.wav", 1, 9),
    Move("Flame", 14, MoveType::Fire, "assets/sounds/flame.wav", 2, 9),
    Move("Splash", 8, MoveType::Water, "assets/sounds/splash.wav", 3, 9),
    Move("Leaf Cut", 12, MoveType::Grass, "assets/sounds/leaf.wav", 4, 9)
    };

    std::vector<Move> eMoves = {
        Move("Bite", 11, MoveType::Normal, "assets/sounds/bite.wav"),
        Move("Ember", 13, MoveType::Fire, "assets/sounds/ember.wav"),
        Move("Water Jet", 9, MoveType::Water, "assets/sounds/waterjet.wav"),
        Move("Vine Lash", 12, MoveType::Grass, "assets/sounds/vinelash.wav")
    };

    static Pokemonster playerP("Pikachu", 100, 20, 10, pMoves);
    static Pokemonster enemyP("Bulbasaur", 90, 18, 8, eMoves);

    player = &playerP;
    enemy = &enemyP;

    // intentamos cargar texturas (si existen)
    player->loadSpriteSheet("assets/images/1.png", 50, 50);
    enemy->loadSpriteSheet("assets/images/2.png",100, 100);

    player->setPosition(100.f, 300.f);
    enemy->setPosition(500.f, 80.f);

    ui.setPlayerPokemon(player);
    ui.setEnemyPokemon(enemy);
    ui.update();
}

void Battle::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        
        // Solo permitir seleccionar ataque si estamos esperando al jugador
        if (state == BattleState::WAITING_FOR_PLAYER)
        {
            int choice = ui.handleMouseClick(mousePos);
            if (choice >= 0 && choice < 4)
            {
                // PASO 1: Guardar el movimiento pendiente
                pendingPlayerMove = choice;
                
                // PASO 2: Cambiar estado a PLAYER_ANIMATING
                state = BattleState::PLAYER_ANIMATING;
                
                // PASO 3: Iniciar la animación (SIN calcular daño aún)
                player->playAnimation(1, 4);
                battleClock.restart();
                
                const Move& mv = player->getMoves()[choice];
                std::cout << "Player uses " << mv.name << "!\n";
            }
        }
    }
}

void Battle::update()
{
    // SIEMPRE actualizar las animaciones en cada frame (no se congelan durante delays)
    if (player) player->updateAnimation();
    if (enemy) enemy->updateAnimation();

    // Máquina de estados para el flujo del combate - SECUENCIA ESTRICTA
    switch (state)
    {
        case BattleState::WAITING_FOR_PLAYER:
            // No hace nada, espera a que el jugador haga clic
            break;

        // ========== TURNO DEL JUGADOR ==========
        case BattleState::PLAYER_ANIMATING:
            // Esperamos a que la animación del jugador termine
            if (!player->isAnimating())
            {
                // ← AQUÍ aplicamos el daño del jugador (JUSTO después de la animación)
                if (pendingPlayerMove >= 0 && pendingPlayerMove < 4)
                {
                    const Move& mv = player->getMoves()[pendingPlayerMove];
                    int damage = std::max(1, mv.power + player->getAttack() - enemy->getDefense());
                    
                    // Aplicar daño al enemigo
                    enemy->takeDamage(damage);
                    ui.update();
                    std::cout << "[DAMAGE] " << damage << " dealt to Enemy! Enemy HP: " 
                              << enemy->getHP() << "/" << enemy->getHPMax() << "\n";
                }

                // Verificar si el enemigo se debilitó
                if (enemy->isFainted())
                {
                    std::cout << "\n🎉 Enemy fainted! You win!\n";
                    finished = true;
                    state = BattleState::WAITING_FOR_PLAYER;
                }
                else
                {
                    // Transicionar a ENEMY_DELAY (esperar antes del ataque del enemigo)
                    state = BattleState::ENEMY_DELAY;
                    battleClock.restart();
                    std::cout << "[WAIT] Enemy will attack after 1 second...\n";
                }
                
                pendingPlayerMove = -1;  // Limpiar
            }
            break;

        case BattleState::ENEMY_DELAY:
            // Esperar 1 segundo antes de que el enemigo ataque
            if (battleClock.getElapsedTime().asSeconds() >= 1.0f)
            {
                // Seleccionar movimiento del enemigo
                std::uniform_int_distribution<int> dist(0, 3);
                pendingEnemyMove = dist(rng);
                
                const Move& mv = enemy->getMoves()[pendingEnemyMove];
                std::cout << "\nEnemy uses " << mv.name << "!\n";
                
                // Iniciar animación del enemigo
                enemy->playAnimation(1, 4);
                battleClock.restart();
                
                // Transicionar a ENEMY_ANIMATING
                state = BattleState::ENEMY_ANIMATING;
            }
            break;

        // ========== TURNO DEL ENEMIGO ==========
        case BattleState::ENEMY_ANIMATING:
            // Esperamos a que la animación del enemigo termine
            if (!enemy->isAnimating())
            {
                // ← AQUÍ aplicamos el daño del enemigo (JUSTO después de la animación)
                if (pendingEnemyMove >= 0 && pendingEnemyMove < 4)
                {
                    const Move& mv = enemy->getMoves()[pendingEnemyMove];
                    int damage = std::max(1, mv.power + enemy->getAttack() - player->getDefense());
                    
                    // Aplicar daño al jugador
                    player->takeDamage(damage);
                    ui.update();
                    std::cout << "[DAMAGE] " << damage << " dealt to Player! Player HP: " 
                              << player->getHP() << "/" << player->getHPMax() << "\n";
                }

                // Verificar si el jugador se debilitó
                if (player->isFainted())
                {
                    std::cout << "\n💔 Player fainted! You lose!\n";
                    finished = true;
                    state = BattleState::WAITING_FOR_PLAYER;
                }
                else
                {
                    // Transicionar a PLAYER_DELAY (esperar antes del próximo turno del jugador)
                    state = BattleState::PLAYER_DELAY;
                    battleClock.restart();
                    std::cout << "[WAIT] Preparing for next turn...\n";
                }
                
                pendingEnemyMove = -1;  // Limpiar
            }
            break;

        case BattleState::PLAYER_DELAY:
            // Esperar 1 segundo antes de volver a esperar al jugador
            if (battleClock.getElapsedTime().asSeconds() >= 1.0f)
            {
                // Volver al estado inicial del turno
                state = BattleState::WAITING_FOR_PLAYER;
                std::cout << "\n=== NEW TURN: Waiting for Player Action ===\n";
            }
            break;

        case BattleState::CHECK_FAINTED:
            // Estado reservado para verificaciones adicionales
            state = BattleState::WAITING_FOR_PLAYER;
            break;
    }
}

void Battle::render()
{
    window.clear(sf::Color(30, 30, 30));

    // dibujar pokemons
    if (enemy) window.draw(*enemy);
    if (player) window.draw(*player);

    // dibujar UI encima
    ui.draw(window);

    window.display();
}


Game::Game() : isRunning(true)
{
    window.create(sf::VideoMode({800, 600}), "Pokemon Game");
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

            // pasar evento a la batalla
            battle.handleEvent(event);
        }

        battle.update();
        battle.render();
    }
}

void Game::handleEvents()
{
    // ahora manejado dentro de run/battle
}

void Game::update()
{
}

void Game::render()
{
}

