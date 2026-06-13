#include "../include/Game.h"
#include "../include/Move.h"
#include <iostream>

// ── Tabla de los 6 personajes ─────────────────────────────────────────────────
struct CharInfo
{
    const char* name;
    const char* spritePath;
    int frameW, frameH, hp, atk, def;
    struct MoveInfo {
        const char* name; int power; MoveType type; int row; int frames; int cost;
    };
    MoveInfo moves[4];
};

static const CharInfo CHAR_TABLE[6] = {
    {   // 0: Exdarktyranomon
        "Exdarktyranomon", "assets/images/1.png", 50, 50, 500, 22, 8,
        {
            {"Frenesi Viral",    14, MoveType::Normal, 0, 9, 1},
            {"Cola de Hierro",   17, MoveType::Normal, 1, 9, 2},
            {"Explosion Fuego",  21, MoveType::Fire,   2, 9, 3},
            {"Mar de Llamas",    26, MoveType::Fire,   3, 9, 5}
        }
    },
    {   // 1: BeelStarmon
        "BeelStarmon", "assets/images/2.png", 100, 100, 475, 20, 6,
        {
            {"Gatillo Rapido",    12, MoveType::Normal, 0, 9, 1},
            {"Garra Doble",       17, MoveType::Normal, 1, 9, 2},
            {"Huracan de Plomo",  20, MoveType::Normal, 2, 9, 3},
            {"Bala Mosca",        24, MoveType::Normal, 3, 9, 5}
        }
    },
    {   // 2: Bioquetzalmon
        "Bioquetzalmon", "assets/images/3.png", 100, 100, 450, 18, 10,
        {
            {"Blindaje Biorganico",  11, MoveType::Normal,   0, 8, 1},
            {"Onda Congelante",      17, MoveType::Water,    1, 8, 2},
            {"Rayo Fosil",           20, MoveType::Electric, 2, 8, 3},
            {"Vuelo Serpiente",      24, MoveType::Normal,   3, 8, 4}
        }
    },
    {   // 3: Jesmon
        "Jesmon", "assets/images/4.png", 100, 100, 525, 25, 7,
        {
            {"OS Generics",          12, MoveType::Normal, 0, 9, 1},
            {"Juicio de la Espada",  18, MoveType::Normal, 1, 9, 2},
            {"Weltgeist",            23, MoveType::Normal, 2, 9, 4},
            {"Uno Para Todos",       27, MoveType::Normal, 3, 9, 5}
        }
    },
    {   // 4: Sleipmon
        "Sleipmon", "assets/images/5.png", 150, 80, 550, 19, 12,
        {
            {"Armadura Roja",    11, MoveType::Normal,   0, 9, 1},
            {"Aliento de Odin",  17, MoveType::Normal,   1, 9, 2},
            {"Disparo Tactico",  20, MoveType::Normal,   2, 9, 3},
            {"Bifrost",          26, MoveType::Electric, 3, 9, 5}
        }
    },
    {   // 5: Magnamon
        "Magnamon", "assets/images/6.png", 100, 100, 490, 23, 9,
        {
            {"Escudo Milagros",       12, MoveType::Normal,   0, 14, 1},
            {"Patada Magnum",         17, MoveType::Normal,   1, 14, 2},
            {"Disparo de Plasma",     21, MoveType::Electric, 2, 14, 3},
            {"Tormenta Solar Dorada", 26, MoveType::Normal,   3, 14, 5}
        }
    }
};

// ── Escala visual por personaje ───────────────────────────────────────────────
// Sleipmon y Bioquetzalmon como referencia visual correcta.
// Los demás ajustados para verse proporcionales sin tapar las barras de HP.
struct ScaleInfo { float scalePlayer; float scaleEnemy; };

static const ScaleInfo SCALE_TABLE[6] = {
    {3.5f, 2.5f},   // 0: Exdarktyranomon (50x50 → x3.5 = 175px, proporcional)
    {2.0f, 1.5f},   // 1: BeelStarmon     (100x100 → reducido)
    {2.0f, 1.5f},   // 2: Bioquetzalmon   (100x100 → referencia correcta)
    {1.5f, 1.5f},   // 3: Jesmon          (100x100)
    {2.5f, 1.8f},   // 4: Sleipmon        (150x80  → referencia correcta)
    {2.0f, 1.5f},   // 5: Magnamon        (100x100)
};

// ── Posiciones estilo Pokémon clásico ─────────────────────────────────────────
// Jugador: abajo-izquierda. Enemigo: arriba-derecha.
// Las posiciones Y se ajustan para que las barras de HP (que están más arriba)
// no tapen el sprite. Barra jugador en y≈370, sprite jugador empieza en y≈380.
struct PosInfo { float px, py, ex, ey; };

static const PosInfo POS_TABLE[6] = {
    //  jugador(x,  y)    enemigo(x,   y)
    {  50.f, 370.f,   470.f,  120.f },  // 0: Exdarktyranomon
    {  50.f, 360.f,   470.f,  110.f },  // 1: BeelStarmon
    {  50.f, 360.f,   470.f,  110.f },  // 2: Bioquetzalmon
    {  50.f, 360.f,   470.f,  110.f },  // 3: Jesmon
    {  30.f, 370.f,   440.f,  130.f },  // 4: Sleipmon (más ancho)
    {  50.f, 360.f,   470.f,  110.f },  // 5: Magnamon
};

// ── buildCharacter ────────────────────────────────────────────────────────────
Pokemonster Game::buildCharacter(int index, bool isPlayer)
{
    if (index < 0 || index > 5) index = 0;
    const CharInfo& c = CHAR_TABLE[index];

    std::vector<Move> moves;
    for (int i = 0; i < 4; ++i)
        moves.emplace_back(c.moves[i].name, c.moves[i].power, c.moves[i].type,
                           "", c.moves[i].row, c.moves[i].frames, c.moves[i].cost);

    Pokemonster p(c.name, c.hp, c.atk, c.def, moves);
    p.loadSpriteSheet(c.spritePath, c.frameW, c.frameH);

    const ScaleInfo& s   = SCALE_TABLE[index];
    const PosInfo&   pos = POS_TABLE[index];

    if (isPlayer)
    {
        // Jugador: primer plano, abajo-izquierda, escala mayor
        p.setScale(s.scalePlayer, s.scalePlayer);
        p.setPosition(pos.px, pos.py);
    }
    else
    {
        // Enemigo: al fondo, arriba-derecha, escala menor
        p.setScale(s.scaleEnemy, s.scaleEnemy);
        p.setPosition(pos.ex, pos.ey);
    }

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

// ── run ───────────────────────────────────────────────────────────────────────
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
                if (currentTurn == TurnState::PLAYER_TURN &&
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

// ── update ────────────────────────────────────────────────────────────────────
void Game::update()
{
    player.updateAnimation();
    enemy.updateAnimation();

    if (animationPlaying && !player.isAnimating() && !enemy.isAnimating())
        animationPlaying = false;

    // Turno del enemigo: espera 800ms después del ataque del jugador
    if (currentTurn          == TurnState::ENEMY_TURN &&
        enemyAttackScheduled && !animationPlaying      &&
        turnClock.getElapsedTime().asMilliseconds() >= 800)
    {
        player.addEnergy(1);
        enemy.addEnergy(1);

        std::vector<int> usable;
        for (int i = 0; i < (int)enemy.getMoves().size(); ++i)
            if (enemy.canUseMove(i)) usable.push_back(i);

        if (!usable.empty())
        {
            int moveIndex = usable[std::rand() % usable.size()];
            enemy.attack(player, moveIndex);
            animationPlaying = true;
        }

        enemyAttackScheduled = false;
        currentTurn          = TurnState::PLAYER_TURN;
        waitingForPlayer     = true;
    }

    if (player.isFainted() || enemy.isFainted())
    { currentTurn = TurnState::BATTLE_OVER; isRunning = false; }

    battleUI.update();
}

// ── render ────────────────────────────────────────────────────────────────────
void Game::render()
{
    window.clear(sf::Color(30, 30, 50));

    // Enemigo primero (al fondo), jugador encima (primer plano)
    window.draw(enemy);
    window.draw(player);

    battleUI.draw(window);
    window.display();
}
