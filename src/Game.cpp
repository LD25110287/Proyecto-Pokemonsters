#include "../include/Game.h"
#include "../include/Move.h"
#include "../include/AudioManager.h"
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
            {"Frenesi Viral",    6,  MoveType::Normal, 0, 14, 0},
            {"Cola de Hierro",   15, MoveType::Normal, 1, 14, 2},
            {"Explosion Fuego",  25, MoveType::Fire,   2, 14, 3},
            {"Mar de Llamas",    40, MoveType::Fire,   3, 14, 5}
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

static const Attribute ATTR_TABLE[6] = {
    Attribute::Vacuna,  // 0
    Attribute::Virus,   // 1
    Attribute::Vacuna,  // 2
    Attribute::Data,    // 3
    Attribute::Vacuna,  // 4
    Attribute::Vacuna,  // 5
};

static const char* PORTRAIT_PATHS[6] = {
    "assets/images/champ_select_1.png",
    "assets/images/champ_select_2.png",
    "assets/images/champ_select_3.png",
    "assets/images/champ_select_4.png",
    "assets/images/champ_select_5.png",
    "assets/images/champ_select_6.png",
};

struct ScaleInfo { float scalePlayer; float scaleEnemy; };
static const ScaleInfo SCALE_TABLE[6] = {
    {3.5f, 2.5f},
    {2.0f, 1.5f},
    {2.0f, 1.5f},
    {1.5f, 1.5f},
    {2.5f, 1.8f},
    {2.0f, 1.5f},
};

struct PosInfo { float px, py, ex, ey; };
static const PosInfo POS_TABLE[6] = {
    {  50.f, 370.f,   670.f,  120.f },
    {  50.f, 360.f,   630.f,  110.f },
    {  50.f, 360.f,   630.f,  110.f },
    {  60.f, 410.f,   630.f,  110.f },
    {  30.f, 370.f,   700.f,  130.f },
    {  50.f, 360.f,   630.f,  110.f },
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
    p.setAttribute(ATTR_TABLE[index]);

    const ScaleInfo& s   = SCALE_TABLE[index];
    const PosInfo&   pos = POS_TABLE[index];

    if (isPlayer)
    {
        p.setScale(s.scalePlayer, s.scalePlayer);
        p.setPosition(pos.px, pos.py);
    }
    else
    {
        p.setScale(-s.scaleEnemy, s.scaleEnemy);
        p.setPosition(pos.ex, pos.ey);
    }

    return p;
}

// ── initBattleUI ─────────────────────────────────────────────────────────────
void Game::initBattleUI(int p1Index, int p2Index)
{
    battleUI.setPlayerPokemon(&player);
    battleUI.setEnemyPokemon(&enemy);
    battleUI.loadAttributeIcons();
    battleUI.loadSwitchIcons();

    // Pasar equipos completos a la UI
    battleUI.setP1Team(p1Team_, p1ActiveSlot_);
    battleUI.setP2Team(p2Team_, p2ActiveSlot_);

    int safe1 = (p1Index < 0 || p1Index > 5) ? 0 : p1Index;
    int safe2 = (p2Index < 0 || p2Index > 5) ? 0 : p2Index;
    battleUI.setPlayerAttribute(ATTR_TABLE[safe1]);
    battleUI.setEnemyAttribute (ATTR_TABLE[safe2]);

    battleUI.updateMoveButtons(&player, true);
    battleUI.updateMoveButtons(&enemy,  false);
    battleUI.setActivePlayer(true);
    battleUI.update();
}

// ── loadBg ────────────────────────────────────────────────────────────────────
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

// ── loadFont helper ───────────────────────────────────────────────────────────
static void loadFont(sf::Font& font)
{
    std::vector<std::string> paths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (auto& p : paths)
        if (font.loadFromFile(p)) return;
}

// ── Constructor normal (sin equipos) ─────────────────────────────────────────
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
    , p1Team_({p1Index, p1Index, p1Index})
    , p2Team_({p2Index, p2Index, p2Index})
    , p1ActiveSlot_(0)
    , p2ActiveSlot_(0)
    , player(buildCharacter(p1Index, true))
    , enemy(buildCharacter(p2Index, false))
    , battleUI(window.getSize())
    , bgIndex_(bgIndex)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    loadFont(font_);
    loadBg(battleBgTexture, battleBg, bgIndex);
    initBattleUI(p1Index, p2Index);
    announceClock.restart();
    AudioManager::playRoundAnnounce(roundNumber);
}

// ── Constructor con equipos completos ─────────────────────────────────────────
Game::Game(const std::array<int,3>& p1Team,
           const std::array<int,3>& p2Team,
           int bgIndex)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Batalla")
    , isRunning(true)
    , winner(0)
    , phase(GamePhase::ANNOUNCING)
    , roundNumber(1)
    , currentTurn(TurnState::PLAYER1_TURN)
    , waitingForInput(false)
    , animationPlaying(false)
    , delayScheduled(false)
    , p1Team_(p1Team)
    , p2Team_(p2Team)
    , p1ActiveSlot_(0)
    , p2ActiveSlot_(0)
    , player(buildCharacter(p1Team[0], true))
    , enemy(buildCharacter(p2Team[0], false))
    , battleUI(window.getSize())
    , bgIndex_(bgIndex)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    loadFont(font_);
    loadBg(battleBgTexture, battleBg, bgIndex);
    initBattleUI(p1Team[0], p2Team[0]);
    announceClock.restart();
    AudioManager::playRoundAnnounce(roundNumber);
}

// ── Constructor KoF (sin equipos) ────────────────────────────────────────────
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
    , p1Team_({p1Index, p1Index, p1Index})
    , p2Team_({p2Index, p2Index, p2Index})
    , p1ActiveSlot_(0)
    , p2ActiveSlot_(0)
    , player(survivorP1 ? *survivorP1 : buildCharacter(p1Index, true))
    , enemy (survivorP2 ? *survivorP2 : buildCharacter(p2Index, false))
    , battleUI(window.getSize())
    , bgIndex_(bgIndex)
{
    window.setFramerateLimit(60);
    loadFont(font_);

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
    AudioManager::playRoundAnnounce(roundNumber);
}

// ── Constructor KoF con equipos completos ────────────────────────────────────
Game::Game(const std::array<int,3>& p1Team,
           const std::array<int,3>& p2Team,
           int bgIndex,
           Pokemonster* survivorP1,
           Pokemonster* survivorP2,
           int roundNum,
           int p1ActiveSlot,
           int p2ActiveSlot)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Batalla")
    , isRunning(true)
    , winner(0)
    , phase(GamePhase::ANNOUNCING)
    , roundNumber(roundNum)
    , currentTurn(TurnState::PLAYER1_TURN)
    , waitingForInput(false)
    , animationPlaying(false)
    , delayScheduled(false)
    , p1Team_(p1Team)
    , p2Team_(p2Team)
    , p1ActiveSlot_(p1ActiveSlot)
    , p2ActiveSlot_(p2ActiveSlot)
    , player(survivorP1 ? *survivorP1 : buildCharacter(p1Team[p1ActiveSlot], true))
    , enemy (survivorP2 ? *survivorP2 : buildCharacter(p2Team[p2ActiveSlot], false))
    , battleUI(window.getSize())
    , bgIndex_(bgIndex)
{
    window.setFramerateLimit(60);
    loadFont(font_);

    int idx1 = p1Team[p1ActiveSlot];
    int idx2 = p2Team[p2ActiveSlot];

    if (survivorP1)
    {
        player.setScale(SCALE_TABLE[idx1].scalePlayer, SCALE_TABLE[idx1].scalePlayer);
        player.setPosition(POS_TABLE[idx1].px, POS_TABLE[idx1].py);
    }
    if (survivorP2)
    {
        enemy.setScale(-SCALE_TABLE[idx2].scaleEnemy, SCALE_TABLE[idx2].scaleEnemy);
        enemy.setPosition(POS_TABLE[idx2].ex, POS_TABLE[idx2].ey);
    }

    loadBg(battleBgTexture, battleBg, bgIndex);
    initBattleUI(idx1, idx2);
    announceClock.restart();
    AudioManager::playRoundAnnounce(roundNumber);
}

Game::~Game() { window.close(); }

// ── executeSwitch ─────────────────────────────────────────────────────────────
// Sustituye al personaje activo del jugador indicado por el del slot elegido.
// Retorna true si el cambio se realizó, false si fue inválido.
bool Game::executeSwitch(bool isP1, int slot)
{
    if (slot < 0 || slot > 2) return false;

    std::array<int,3>& team      = isP1 ? p1Team_ : p2Team_;
    int&               activeSlot = isP1 ? p1ActiveSlot_ : p2ActiveSlot_;

    if (slot == activeSlot) return false;   // ya está en campo

    int newCharIdx = team[slot];
    if (newCharIdx < 0 || newCharIdx > 5) return false;

    // Construir el nuevo personaje y reemplazar
    if (isP1)
    {
        player = buildCharacter(newCharIdx, true);
        battleUI.setPlayerPokemon(&player);
        battleUI.setPlayerAttribute(ATTR_TABLE[newCharIdx]);
    }
    else
    {
        enemy = buildCharacter(newCharIdx, false);
        battleUI.setEnemyPokemon(&enemy);
        battleUI.setEnemyAttribute(ATTR_TABLE[newCharIdx]);
    }

    activeSlot = slot;

    // Sincronizar el slot activo en la UI (fix bug: panel solo mostraba 1 personaje)
    battleUI.updateActiveSlot(isP1, slot);

    // Actualizar la UI con el nuevo slot activo
    battleUI.setP1Team(p1Team_, p1ActiveSlot_);
    battleUI.setP2Team(p2Team_, p2ActiveSlot_);
    battleUI.updateMoveButtons(isP1 ? &player : &enemy, isP1);

    return true;
}

// ── drawRoundAnnouncement ─────────────────────────────────────────────────────
void Game::drawRoundAnnouncement()
{
    window.clear(sf::Color(10, 10, 20));
    window.draw(battleBg);

    sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

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
        // ── Fase ANNOUNCING ───────────────────────────────────────────────────
        if (phase == GamePhase::ANNOUNCING)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                { window.close(); isRunning = false; return; }
            }

            drawRoundAnnouncement();

            if (announceClock.getElapsedTime().asSeconds() >= 3.f)
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
            {
                // Si el panel de cambio está abierto, Escape lo cierra
                if (battleUI.isSwitchPanelOpen())
                {
                    // Forzar cierre: hacer un click fuera con coordenadas imposibles
                    battleUI.handleSwitchClick({-1, -1});
                }
                else
                {
                    window.close(); isRunning = false;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                if (!waitingForInput || animationPlaying || delayScheduled)
                    continue;

                sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);

                // ── Panel de cambio abierto → procesar selección ──────────────
                if (battleUI.isSwitchPanelOpen())
                {
                    int slot = battleUI.handleSwitchClick(mp);
                    if (slot >= 0)
                    {
                        bool isP1 = (currentTurn == TurnState::PLAYER1_TURN);
                        if (executeSwitch(isP1, slot))
                        {
                            // Cambio exitoso: pierde el turno (igual que atacar)
                            // No hay animación de ataque, pero sí el delay de turno
                            waitingForInput = false;
                            delayScheduled  = true;
                            animDelayClock.restart();
                        }
                    }
                    continue;   // no procesar clicks de movimiento este frame
                }

                // ── Clicks normales de movimiento ─────────────────────────────
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
                    // handleMouseClick ya abrió el panel si se clickeó el ícono
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

        player.addEnergy(1);
        enemy.addEnergy(1);

        if (player.isFainted() || enemy.isFainted())
        {
            if (player.isFainted() && !enemy.isFainted())
                winner = 2;
            else if (enemy.isFainted() && !player.isFainted())
                winner = 1;
            else
                winner = 0;

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
