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

    // ── JUGADOR: Exdarktyranomon (1.png) ─────────────────────────────────
    // Spritesheet: 9 cols x 4 filas, frame 50x50
    // Fila 0: Frenesí Viral  | Fila 1: Cola de Hierro
    // Fila 2: Explosión Fuego | Fila 3: Mar de Llamas
    player = Pokemonster("Exdarktyranomon", 100, 22, 8, {
        Move("Frenesi Viral",    45, MoveType::Normal, "", 0, 9),
        Move("Cola de Hierro",   55, MoveType::Normal, "", 1, 9),
        Move("Explosion Fuego",  70, MoveType::Fire,   "", 2, 9),
        Move("Mar de Llamas",    85, MoveType::Fire,   "", 3, 9)
    });
    player.loadSpriteSheet("assets/images/1.png", 50, 50);
    player.setPosition(150.f, 300.f);

    // ── ENEMIGO: BeelStarmon (2.png) ─────────────────────────────────────
    // Spritesheet: 9 cols x 4 filas, frame 100x100
    // Fila 0: Gatillo Rápido | Fila 1: Garra Doble
    // Fila 2: Huracán Plomo  | Fila 3: Bala Mosca
    enemy = Pokemonster("BeelStarmon", 95, 20, 6, {
        Move("Gatillo Rapido",   40, MoveType::Normal, "", 0, 9),
        Move("Garra Doble",      55, MoveType::Normal, "", 1, 9),
        Move("Huracan de Plomo", 65, MoveType::Normal, "", 2, 9),
        Move("Bala Mosca",       80, MoveType::Normal, "", 3, 9)
    });
    enemy.loadSpriteSheet("assets/images/2.png", 100, 100);
    enemy.setPosition(450.f, 100.f);

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

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            {
                window.close();
                isRunning = false;
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
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

        update();
        render();
    }
}

void Game::handleEvents() {}

void Game::update()
{
    player.updateAnimation();
    enemy.updateAnimation();

    if (animationPlaying && !player.isAnimating() && !enemy.isAnimating())
        animationPlaying = false;

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

    if (player.isFainted() || enemy.isFainted())
    {
        currentTurn = TurnState::BATTLE_OVER;
        isRunning   = false;
    }

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
