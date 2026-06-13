#include "../include/Game.h"
#include "../include/Move.h"
#include <iostream>

// ── Tabla de los 6 personajes ─────────────────────────────────────────────────
// Cada entrada: nombre, spritePath, frameW, frameH, hp, atk, def, moves[4]
// Moves: { nombre, poder, tipo, "", fila, frames }
// Todos los spritesheets tienen 4 filas (fila 0-3) y N columnas según el sprite
struct CharInfo
{
    const char* name;
    const char* spritePath;
    int frameW, frameH;
    int hp, atk, def;
    // 4 movimientos: nombre, poder, fila
    struct MoveInfo { const char* name; int power; MoveType type; int row; int frames; };
    MoveInfo moves[4];
};

static const CharInfo CHAR_TABLE[6] = {
    {   // 0: Exdarktyranomon — 1.png — 9 cols x 4 filas — 50x50
        "Exdarktyranomon", "assets/images/1.png", 50, 50, 100, 22, 8,
        {
            {"Frenesi Viral",    45, MoveType::Normal, 0, 9},
            {"Cola de Hierro",   55, MoveType::Normal, 1, 9},
            {"Explosion Fuego",  70, MoveType::Fire,   2, 9},
            {"Mar de Llamas",    85, MoveType::Fire,   3, 9}
        }
    },
    {   // 1: BeelStarmon — 2.png — 9 cols x 4 filas — 100x100
        "BeelStarmon", "assets/images/2.png", 100, 100, 95, 20, 6,
        {
            {"Gatillo Rapido",   40, MoveType::Normal, 0, 9},
            {"Garra Doble",      55, MoveType::Normal, 1, 9},
            {"Huracan de Plomo", 65, MoveType::Normal, 2, 9},
            {"Bala Mosca",       80, MoveType::Normal, 3, 9}
        }
    },
    {   // 2: Bioquetzalmon — 3.png — 8 cols x 4 filas — 100x100
        "Bioquetzalmon", "assets/images/3.png", 100, 100, 90, 18, 10,
        {
            {"Blindaje Biorganico", 35, MoveType::Normal,  0, 8},
            {"Onda Congelante",     55, MoveType::Water,   1, 8},
            {"Rayo Fosil",          65, MoveType::Electric,2, 8},
            {"Vuelo de la Serpiente",80,MoveType::Normal,  3, 8}
        }
    },
    {   // 3: Jesmon — 4.png — 9 cols x 4 filas — 100x100
        "Jesmon", "assets/images/4.png", 100, 100, 105, 25, 7,
        {
            {"OS Generics",        40, MoveType::Normal, 0, 9},
            {"Juicio de la Espada",60, MoveType::Normal, 1, 9},
            {"Weltgeist",          75, MoveType::Normal, 2, 9},
            {"Uno Para Todos",     90, MoveType::Normal, 3, 9}
        }
    },
    {   // 4: Sleipmon — 5.png — 9 cols x 4 filas — 150x80
        "Sleipmon", "assets/images/5.png", 150, 80, 110, 19, 12,
        {
            {"Armadura Roja",    35, MoveType::Normal, 0, 9},
            {"Aliento de Odin",  55, MoveType::Normal, 1, 9},
            {"Disparo Tactico",  65, MoveType::Normal, 2, 9},
            {"Bifrost",          85, MoveType::Electric,3, 9}
        }
    },
    {   // 5: Magnamon — 6.png — 14 cols x 4 filas — 100x100
        "Magnamon", "assets/images/6.png", 100, 100, 98, 23, 9,
        {
            {"Escudo Milagros",       40, MoveType::Normal,   0, 14},
            {"Patada Magnum",         55, MoveType::Normal,   1, 14},
            {"Disparo de Plasma",     70, MoveType::Electric, 2, 14},
            {"Tormenta Solar Dorada", 88, MoveType::Normal,   3, 14}
        }
    }
};

// ── Función auxiliar ──────────────────────────────────────────────────────────
Pokemonster Game::buildCharacter(int index, bool isPlayer)
{
    if (index < 0 || index > 5) index = 0;
    const CharInfo& c = CHAR_TABLE[index];

    std::vector<Move> moves;
    for (int i = 0; i < 4; ++i)
        moves.emplace_back(c.moves[i].name, c.moves[i].power, c.moves[i].type,
                           "", c.moves[i].row, c.moves[i].frames);

    Pokemonster p(c.name, c.hp, c.atk, c.def, moves);
    p.loadSpriteSheet(c.spritePath, c.frameW, c.frameH);

    // Jugador abajo-izquierda, enemigo arriba-derecha
    if (isPlayer)
        p.setPosition(150.f, 320.f);
    else
        p.setPosition(450.f, 80.f);

    return p;
}

// ── Constructor ───────────────────────────────────────────────────────────────
Game::Game(int p1Index, int p2Index)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Batalla")
    , isRunning(true)
    , currentTurn(TurnState::PLAYER_TURN)
    , waitingForPlayer(true)
    , animationPlaying(false)
    , enemyAttackScheduled(false)
    , player(buildCharacter(p1Index, true))
    , enemy(buildCharacter(p2Index, false))
    , battleUI(window.getSize())
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    battleUI.setPlayerPokemon(&player);
    battleUI.setEnemyPokemon(&enemy);
    battleUI.updateMoveButtons(&player);
    battleUI.update();
}

Game::~Game() { window.close(); }

// ── Game loop ─────────────────────────────────────────────────────────────────
void Game::run()
{
    while (window.isOpen() && isRunning && currentTurn != TurnState::BATTLE_OVER)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            { window.close(); isRunning = false; }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            { window.close(); isRunning = false; }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                if (currentTurn  == TurnState::PLAYER_TURN &&
                    waitingForPlayer && !animationPlaying)
                {
                    sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);
                    int moveIndex = battleUI.handleMouseClick(mp);
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
        enemyAttackScheduled && !animationPlaying      &&
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
    { currentTurn = TurnState::BATTLE_OVER; isRunning = false; }

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
