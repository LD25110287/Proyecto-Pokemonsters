#include "../include/Game.h"
#include "../include/Move.h"
#include <iostream>

// ── Tabla de los 6 personajes ─────────────────────────────────────────────────
// El "power" ahora es un PORCENTAJE del HP máximo del objetivo:
//   Habilidad 1 (0E): power=6  → ~6%  del HP máximo (rango 5-8%)
//   Habilidad 2 (2E): power=15 → ~15% del HP máximo
//   Habilidad 3 (3E): power=25 → ~25% del HP máximo
//   Habilidad 4 (5E): power=40 → ~40% del HP máximo
// La fórmula en Pokemonster::attack() convierte estos % a daño real.
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
            {"Frenesi Viral",    6,  MoveType::Normal, 0, 9, 0},  // gratis, daño bajo
            {"Cola de Hierro",   15, MoveType::Normal, 1, 9, 2},
            {"Explosion Fuego",  25, MoveType::Fire,   2, 9, 3},
            {"Mar de Llamas",    40, MoveType::Fire,   3, 9, 5}
        }
    },
    {   // 1: BeelStarmon
        "BeelStarmon", "assets/images/2.png", 100, 100, 475, 20, 6,
        {
            {"Gatillo Rapido",    6,  MoveType::Normal, 0, 9, 0},
            {"Garra Doble",       15, MoveType::Normal, 1, 9, 2},
            {"Huracan de Plomo",  25, MoveType::Normal, 2, 9, 3},
            {"Bala Mosca",        40, MoveType::Normal, 3, 9, 5}
        }
    },
    {   // 2: Bioquetzalmon
        "Bioquetzalmon", "assets/images/3.png", 100, 100, 450, 18, 10,
        {
            {"Blindaje Biorganico",  6,  MoveType::Normal,   0, 8, 0},
            {"Onda Congelante",      15, MoveType::Water,    1, 8, 2},
            {"Rayo Fosil",           25, MoveType::Electric, 2, 8, 3},
            {"Vuelo Serpiente",      40, MoveType::Normal,   3, 8, 4}
        }
    },
    {   // 3: Jesmon
        "Jesmon", "assets/images/4.png", 100, 100, 525, 25, 7,
        {
            {"OS Generics",          6,  MoveType::Normal, 0, 9, 0},
            {"Juicio de la Espada",  15, MoveType::Normal, 1, 9, 2},
            {"Weltgeist",            25, MoveType::Normal, 2, 9, 3},
            {"Uno Para Todos",       40, MoveType::Normal, 3, 9, 5}
        }
    },
    {   // 4: Sleipmon
        "Sleipmon", "assets/images/5.png", 150, 80, 550, 19, 12,
        {
            {"Armadura Roja",    6,  MoveType::Normal,   0, 9, 0},
            {"Aliento de Odin",  15, MoveType::Normal,   1, 9, 2},
            {"Disparo Tactico",  25, MoveType::Normal,   2, 9, 3},
            {"Bifrost",          40, MoveType::Electric, 3, 9, 5}
        }
    },
    {   // 5: Magnamon
        "Magnamon", "assets/images/6.png", 100, 100, 490, 23, 9,
        {
            {"Escudo Milagros",       6,  MoveType::Normal,   0, 14, 0},
            {"Patada Magnum",         15, MoveType::Normal,   1, 14, 2},
            {"Disparo de Plasma",     25, MoveType::Electric, 2, 14, 3},
            {"Tormenta Solar Dorada", 40, MoveType::Normal,   3, 14, 5}
        }
    }
};

// ── Escala visual por personaje ───────────────────────────────────────────────
struct ScaleInfo { float scalePlayer; float scaleEnemy; };
static const ScaleInfo SCALE_TABLE[6] = {
    {3.5f, 2.5f},
    {2.0f, 1.5f},
    {2.0f, 1.5f},
    {1.5f, 1.5f},
    {2.5f, 1.8f},
    {2.0f, 1.5f},
};

// ── Posiciones estilo Pokémon clásico ─────────────────────────────────────────
// ¡MODIFICADO!: Recalculamos 'ex' (coordenada X del Jugador 2)
// Al espejar (-X scale), el personaje se dibuja HACIA LA IZQUIERDA del origen.
// Por lo tanto, movemos el origen de los enemigos (coordenada ex) a la derecha
// del escenario para que al espejarse queden centrados.
struct PosInfo { float px, py, ex, ey; };
static const PosInfo POS_TABLE[6] = {
    {  50.f, 370.f,   670.f,  120.f },  // J2 (Exdarktyranomon) movido ex 470->670
    {  50.f, 360.f,   630.f,  110.f },  // J2 (BeelStarmon)     movido ex 470->630
    {  50.f, 360.f,   630.f,  110.f },  // Bioquetzalmon
    {  60.f, 410.f,   630.f,  110.f },  // Jesmon
    {  30.f, 370.f,   700.f,  130.f },  // Sleipmon
    {  50.f, 360.f,   630.f,  110.f },  // Magnamon
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
        // Jugador 1 (abajo-izquierda) mira hacia la derecha (escala normal)
        p.setScale(s.scalePlayer, s.scalePlayer);
        p.setPosition(pos.px, pos.py);
    }
    else
    {
        p.setScale(s.scaleEnemy, s.scaleEnemy);
        // ¡MEJORA!: Aplicamos Efecto ESPEJO (scaleX negativa) al Jugador 2
        // Al poner la escala en X como negativa (-s.scaleEnemy), SFML
        // invierte el sprite horizontalmente, haciendo que apunte hacia la izquierda.
        p.setScale(-s.scaleEnemy, s.scaleEnemy);
        // Usamos la nueva posición ex corregida para compensar el giro
        p.setPosition(pos.ex, pos.ey);
    }

    return p;
}

// ── Constructor ───────────────────────────────────────────────────────────────
Game::Game(int p1Index, int p2Index, int bgIndex)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Batalla")
    , isRunning(true)
    , winner(0)
    , currentTurn(TurnState::PLAYER1_TURN)
    , waitingForInput(true)
    , animationPlaying(false)
    , delayScheduled(false)
    , player(buildCharacter(p1Index, true))
    , enemy(buildCharacter(p2Index, false))
    , battleUI(window.getSize())
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Fondo aleatorio de batalla
    const std::string BG_PATHS[3] = {
        "assets/images/fondo_batalla_campo.png",
        "assets/images/fondo_batalla_red_crystal.png",
        "assets/images/fondo_batalla_arena.png"
    };
    if (bgIndex < 0 || bgIndex >= 3)
        bgIndex = std::rand() % 3;
    if (battleBgTexture.loadFromFile(BG_PATHS[bgIndex]))
    {
        battleBg.setTexture(battleBgTexture);
        sf::Vector2u texSize = battleBgTexture.getSize();
        battleBg.setScale(800.f / static_cast<float>(texSize.x),
                          600.f / static_cast<float>(texSize.y));
    }
    else
        std::cerr << "[Game] No se pudo cargar fondo: " << BG_PATHS[bgIndex] << "\n";

    battleUI.setPlayerPokemon(&player);
    battleUI.setEnemyPokemon(&enemy);
    battleUI.updateMoveButtons(&player, true);
    battleUI.updateMoveButtons(&enemy,  false);
    battleUI.setActivePlayer(true);
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
                if (!waitingForInput || animationPlaying || delayScheduled)
                    continue;

                sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);

                if (currentTurn == TurnState::PLAYER1_TURN)
                {
                    int moveIndex = battleUI.handleMouseClick(mp, true);
                    if (moveIndex >= 0)
                    {
                        player.attack(enemy, moveIndex);
                        animationPlaying = true;
                        waitingForInput  = false;
                        delayScheduled   = true;
                        animDelayClock.restart();
                    }
                }
                else if (currentTurn == TurnState::PLAYER2_TURN)
                {
                    int moveIndex = battleUI.handleMouseClick(mp, false);
                    if (moveIndex >= 0)
                    {
                        enemy.attack(player, moveIndex);
                        animationPlaying = true;
                        waitingForInput  = false;
                        delayScheduled   = true;
                        animDelayClock.restart();
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

    if (delayScheduled && !animationPlaying &&
        animDelayClock.getElapsedTime().asMilliseconds() >= 600)
    {
        delayScheduled = false;

        // +1 energía a ambos al final de cada turno (habilidad 1 es gratis
        // así que la energía sube naturalmente para las habilidades caras)
        player.addEnergy(1);
        enemy.addEnergy(1);

        if (player.isFainted() || enemy.isFainted())
        {
            // Determinar ganador: si el jugador (J1) está ko, ganó J2 y viceversa
            if (player.isFainted() && !enemy.isFainted())
                winner = 2;
            else if (enemy.isFainted() && !player.isFainted())
                winner = 1;
            else
                winner = 0;  // empate (ambos ko al mismo tiempo)

            currentTurn = TurnState::BATTLE_OVER;
            isRunning   = false;
            return;
        }

        if (currentTurn == TurnState::PLAYER1_TURN)
        {
            currentTurn = TurnState::PLAYER2_TURN;
            battleUI.setActivePlayer(false);
        }
        else
        {
            currentTurn = TurnState::PLAYER1_TURN;
            battleUI.setActivePlayer(true);
        }

        waitingForInput = true;
    }

    battleUI.update();
}

// ── render ────────────────────────────────────────────────────────────────────
void Game::render()
{
    window.clear(sf::Color(30, 30, 50));
    window.draw(battleBg);
    window.draw(enemy);
    window.draw(player);
    battleUI.draw(window);
    window.display();
}