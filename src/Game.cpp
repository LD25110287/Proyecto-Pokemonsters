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

// ── Atributos por personaje: 1=Va,2=Vi,3=Va,4=Da,5=Va,6=Va ──────────────────
static const Attribute ATTR_TABLE[6] = {
    Attribute::Vacuna,  // 0: Exdarktyranomon
    Attribute::Virus,   // 1: BeelStarmon
    Attribute::Vacuna,  // 2: Bioquetzalmon
    Attribute::Data,    // 3: Jesmon
    Attribute::Vacuna,  // 4: Sleipmon
    Attribute::Vacuna,  // 5: Magnamon
};

// ── Paths de portraits ────────────────────────────────────────────────────────
static const char* PORTRAIT_PATHS[6] = {
    "assets/images/champ_select_1.png",
    "assets/images/champ_select_2.png",
    "assets/images/champ_select_3.png",
    "assets/images/champ_select_4.png",
    "assets/images/champ_select_5.png",
    "assets/images/champ_select_6.png",
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
    p.setAttribute(ATTR_TABLE[index]);   // ← asignar atributo

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

// ── Helper: inicializar BattleUI ──────────────────────────────────────────────
void Game::initBattleUI(int p1Index, int p2Index)
{
    battleUI.setPlayerPokemon(&player);
    battleUI.setEnemyPokemon(&enemy);
    battleUI.loadAttributeIcons();

    int safe1 = (p1Index < 0 || p1Index > 5) ? 0 : p1Index;
    int safe2 = (p2Index < 0 || p2Index > 5) ? 0 : p2Index;
    battleUI.setPlayerAttribute(ATTR_TABLE[safe1]);
    battleUI.setEnemyAttribute (ATTR_TABLE[safe2]);

    battleUI.updateMoveButtons(&player, true);
    battleUI.updateMoveButtons(&enemy,  false);
    battleUI.setActivePlayer(true);
    battleUI.update();
}

// ── Helper: cargar fondo ──────────────────────────────────────────────────────
static void loadBg(sf::Texture& tex, sf::Sprite& spr, int bgIndex)
{
    const std::string BG_PATHS[3] = {
        "assets/images/fondo_batalla_campo.png",
        "assets/images/fondo_batalla_red_crystal.png",
        "assets/images/fondo_batalla_arena.png"
    };
    if (bgIndex < 0 || bgIndex >= 3)
        bgIndex = std::rand() % 3;
    if (tex.loadFromFile(BG_PATHS[bgIndex]))
    {
        spr.setTexture(tex);
        sf::Vector2u sz = tex.getSize();
        spr.setScale(800.f / sz.x, 600.f / sz.y);
    }
}

// ── Constructor normal ────────────────────────────────────────────────────────
Game::Game(int p1Index, int p2Index, int bgIndex)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Batalla")
    , isRunning(true)
    , winner(0)
    , phase(GamePhase::ANNOUNCING)
    , roundNumber(1)
    , currentTurn(TurnState::PLAYER1_TURN)
    , waitingForInput(false)
    , animationPlaying(false)
    , delayScheduled(false)
    , player(buildCharacter(p1Index, true))
    , enemy(buildCharacter(p2Index, false))
    , battleUI(window.getSize())
    , bgIndex_(bgIndex)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (auto& fp : fontPaths)
        if (font_.loadFromFile(fp)) break;

    loadBg(battleBgTexture, battleBg, bgIndex);
    initBattleUI(p1Index, p2Index);
    announceClock.restart();
}

// ── Constructor KoF ───────────────────────────────────────────────────────────
Game::Game(int p1Index, int p2Index, int bgIndex,
           Pokemonster* survivorP1, Pokemonster* survivorP2,
           int roundNum)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Batalla")
    , isRunning(true)
    , winner(0)
    , phase(GamePhase::ANNOUNCING)
    , roundNumber(roundNum)
    , currentTurn(TurnState::PLAYER1_TURN)
    , waitingForInput(false)
    , animationPlaying(false)
    , delayScheduled(false)
    , player(survivorP1 ? *survivorP1 : buildCharacter(p1Index, true))
    , enemy (survivorP2 ? *survivorP2 : buildCharacter(p2Index, false))
    , battleUI(window.getSize())
    , bgIndex_(bgIndex)
{
    window.setFramerateLimit(60);

    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (auto& fp : fontPaths)
        if (font_.loadFromFile(fp)) break;

    // Si viene sobreviviente, reposicionarlo para este escenario
    if (survivorP1)
    {
        int idx = (p1Index < 0 || p1Index > 5) ? 0 : p1Index;
        player.setScale(SCALE_TABLE[idx].scalePlayer, SCALE_TABLE[idx].scalePlayer);
        player.setPosition(POS_TABLE[idx].px, POS_TABLE[idx].py);
    }
    if (survivorP2)
    {
        int idx = (p2Index < 0 || p2Index > 5) ? 0 : p2Index;
        enemy.setScale(-SCALE_TABLE[idx].scaleEnemy, SCALE_TABLE[idx].scaleEnemy);
        enemy.setPosition(POS_TABLE[idx].ex, POS_TABLE[idx].ey);
    }

    loadBg(battleBgTexture, battleBg, bgIndex);
    initBattleUI(p1Index, p2Index);
    announceClock.restart();
}

Game::~Game() { window.close(); }

// ── Pantalla de anuncio de ronda ──────────────────────────────────────────────
void Game::drawRoundAnnouncement()
{
    window.clear(sf::Color(10, 10, 20));
    window.draw(battleBg);

    // Overlay oscuro semi-transparente
    sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    // Texto "Ronda X"
    std::string roundStr = "Ronda " + std::to_string(roundNumber);
    sf::Text txt(roundStr, font_, 72);
    txt.setStyle(sf::Text::Bold);
    txt.setFillColor(sf::Color(255, 215, 0));
    txt.setOutlineColor(sf::Color::Black);
    txt.setOutlineThickness(4.f);
    float tx = (800.f - txt.getLocalBounds().width)  / 2.f;
    float ty = (600.f - txt.getLocalBounds().height) / 2.f - 40.f;
    txt.setPosition(tx, ty);
    window.draw(txt);

    // Subtexto "¡Pelea!"
    sf::Text sub("Pelea!", font_, 36);
    sub.setFillColor(sf::Color::White);
    sub.setOutlineColor(sf::Color::Black);
    sub.setOutlineThickness(2.f);
    float sx = (800.f - sub.getLocalBounds().width) / 2.f;
    sub.setPosition(sx, ty + 90.f);
    window.draw(sub);

    window.display();
}

// ── run ───────────────────────────────────────────────────────────────────────
void Game::run()
{
    while (window.isOpen() && isRunning && currentTurn != TurnState::BATTLE_OVER)
    {
        // ── Fase ANNOUNCING: mostrar "Ronda X" por 2 segundos ─────────────────
        if (phase == GamePhase::ANNOUNCING)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                { window.close(); isRunning = false; return; }
            }

            drawRoundAnnouncement();

            if (announceClock.getElapsedTime().asSeconds() >= 2.f)
            {
                phase           = GamePhase::BATTLE;
                waitingForInput = true;
            }
            continue;
        }

        // ── Fase BATTLE ───────────────────────────────────────────────────────
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