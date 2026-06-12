#include "Game.h"
#include "Move.h"

#include <iostream>

Game::Game()
    : window(sf::VideoMode(800, 600), "Pokemon Game - Multiplayer")
    , isRunning(true)
    , currentTurn(TurnState::PLAYER_TURN)
    , waitingForPlayer(true)
    , animationPlaying(false)
    , enemyAttackScheduled(false)
    , player()
    , enemy()
    , battleUI(window.getSize())
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Crear jugador con 4 movimientos
    player = Pokemonster("Pikachu", 100, 20, 5, {
        Move("Impactrueno", 40, MoveType::Electric, "", 0, 4),
        Move("Rayo", 55, MoveType::Electric, "", 1, 3),
        Move("Voltio Cruel", 70, MoveType::Electric, "", 2, 5),
        Move("Gruñido", 0, MoveType::Normal, "", 3, 2)
    });

    // Crear enemigo con 4 movimientos
    enemy = Pokemonster("Charmander", 90, 18, 4, {
        Move("Ascuas", 40, MoveType::Fire, "", 0, 4),
        Move("Llamarada", 60, MoveType::Fire, "", 1, 4),
        Move("Arañazo", 35, MoveType::Normal, "", 2, 3),
        Move("Rugido", 0, MoveType::Normal, "", 3, 2)
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

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                if (currentTurn == TurnState::PLAYER_TURN && waitingForPlayer && !animationPlaying)
                {
                    sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                    int moveIndex = battleUI.handleMouseClick(mousePos);
                    if (moveIndex >= 0)
                    {
                        player.attack(enemy, moveIndex);
                        animationPlaying = true;
                        waitingForPlayer = false;
                        currentTurn = TurnState::ENEMY_TURN;
                        enemyAttackScheduled = true;
                        turnClock.restart();
                    }
                }
            }
        }

        update();
        render();
    }
}

void Game::handleEvents()
{
    // El manejo de eventos se realiza directamente en run().
}

void Game::update()
{
    // BLOQUE 1: Actualizar animaciones siempre
    player.updateAnimation();
    enemy.updateAnimation();

    if (animationPlaying && !player.isAnimating() && !enemy.isAnimating())
    {
        animationPlaying = false;
    }

    // BLOQUE 2: Turno del enemigo
    if (currentTurn == TurnState::ENEMY_TURN && enemyAttackScheduled && !animationPlaying
        && turnClock.getElapsedTime().asMilliseconds() >= 800)
    {
        int moveIndex = std::rand() % static_cast<int>(enemy.getMoves().size());
        enemy.attack(player, moveIndex);
        animationPlaying = true;
        enemyAttackScheduled = false;
        currentTurn = TurnState::PLAYER_TURN;
        waitingForPlayer = true;
    }

    // BLOQUE 3: Verificar fin de batalla
    if (player.isFainted() || enemy.isFainted())
    {
        currentTurn = TurnState::BATTLE_OVER;
        isRunning = false;
    }

    // BLOQUE 4: Actualizar UI
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
