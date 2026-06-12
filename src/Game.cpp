#include "../include/Game.h"
#include "../include/Move.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode(800, 600), "Pokemon Game")
    , isRunning(true)
    , currentTurn(TurnState::PLAYER_TURN)
    , waitingForPlayer(true)
    , animationPlaying(false)
    , enemyAttackScheduled(false)
    , player()
    , enemy()
    , battleUI(window.getSize())
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Cada Move: nombre, poder, tipo, sonido, animationRow, frameCount
    // animationRow distinto por habilidad = BUG 2 FIX
    player = Pokemonster("Pikachu", 100, 20, 5, {
        Move("Impactrueno", 40, MoveType::Electric, "", 1, 4),
        Move("Rayo",        55, MoveType::Electric, "", 2, 3),
        Move("Voltio Cruel",70, MoveType::Electric, "", 3, 5),
        Move("Gruñido",      0, MoveType::Normal,   "", 0, 2)
    });

    enemy = Pokemonster("Charmander", 90, 18, 4, {
        Move("Ascuas",    40, MoveType::Fire,   "", 1, 4),
        Move("Llamarada", 60, MoveType::Fire,   "", 2, 4),
        Move("Arañazo",   35, MoveType::Normal, "", 3, 3),
        Move("Rugido",     0, MoveType::Normal, "", 0, 2)
    });

    player.setPosition(150.f, 300.f);
    enemy.setPosition(500.f, 150.f);

    battleUI.setPlayerPokemon(&player);
    battleUI.setEnemyPokemon(&enemy);
    battleUI.updateMoveButtons(&player);
    battleUI.update();
}

Game::~Game()
{
    window.close();
}

void Game::run()
{
    while (window.isOpen() && isRunning && currentTurn != TurnState::BATTLE_OVER)
    {
        // ── API SFML 2 ────────────────────────────────────────────────────
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                isRunning = false;
            }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            {
                window.close();
                isRunning = false;
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                // BUG 1 FIX: solo actúa si es turno del jugador,
                // está esperando input y ninguna animación está activa.
                if (currentTurn     == TurnState::PLAYER_TURN &&
                    waitingForPlayer                           &&
                    !animationPlaying)
                {
                    sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                    int moveIndex = battleUI.handleMouseClick(mousePos);
                    if (moveIndex >= 0)
                    {
                        player.attack(enemy, moveIndex);
                        animationPlaying     = true;
                        waitingForPlayer     = false;
                        currentTurn          = TurnState::ENEMY_TURN;
                        enemyAttackScheduled = true;
                        turnClock.restart();
                    }
                }
            }
        }
        // ─────────────────────────────────────────────────────────────────

        update();
        render();
    }
}

void Game::handleEvents() { /* manejo en run() */ }

void Game::update()
{
    // 1. Actualizar animaciones
    player.updateAnimation();
    enemy.updateAnimation();

    // 2. Detectar fin de animación
    if (animationPlaying && !player.isAnimating() && !enemy.isAnimating())
        animationPlaying = false;

    // 3. Turno del enemigo — espera a que animación del jugador termine
    // BUG 1 FIX: el enemigo solo ataca cuando !animationPlaying
    if (currentTurn          == TurnState::ENEMY_TURN &&
        enemyAttackScheduled                           &&
        !animationPlaying                              &&
        turnClock.getElapsedTime().asMilliseconds() >= 800)
    {
        int moveIndex = std::rand() % static_cast<int>(enemy.getMoves().size());
        enemy.attack(player, moveIndex);
        animationPlaying     = true;
        enemyAttackScheduled = false;
        currentTurn          = TurnState::PLAYER_TURN;
        waitingForPlayer     = true;
    }

    // 4. Verificar fin de batalla
    if (player.isFainted() || enemy.isFainted())
    {
        currentTurn = TurnState::BATTLE_OVER;
        isRunning   = false;
    }

    // 5. Actualizar UI
    battleUI.update();
}

void Game::render()
{
    window.clear(sf::Color(30, 30, 50));
    window.draw(player);
    window.draw(enemy);
    battleUI.draw(window);
    window.display();
}
