#ifndef CHARACTER_SELECT_H
#define CHARACTER_SELECT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>

struct CharacterData
{
    std::string name;
    std::string spritePath;
    std::string portraitPath;
    int frameWidth;
    int frameHeight;
    int hp;
    int attack;
    int defense;
};

class CharacterSelect
{
public:
    CharacterSelect();
    void run();

    bool shouldLaunchBattle()    const { return launchBattle; }
    int  getSelectedStage()      const { return selectedStage; }

    // Devuelve el equipo de cada jugador en orden de batalla elegido
    const std::array<int,3>& getPlayer1Team() const { return p1Team; }
    const std::array<int,3>& getPlayer2Team() const { return p2Team; }

    // Para compatibilidad con main.cpp (primer personaje del equipo)
    int getPlayer1Choice() const { return p1Team[0]; }
    int getPlayer2Choice() const { return p2Team[0]; }

private:
    // ── Fases ─────────────────────────────────────────────────────────────────
    // PICKING:      alternando J1→J2→J1→J2→J1→J2 (6 picks en total)
    // ORDER_P1:     J1 elige el orden de su equipo
    // ORDER_P2:     J2 elige el orden de su equipo
    // CONFIRM:      pantalla final con fondo VS y equipos ordenados
    // STAGE_SELECT: ruleta de escenario
    enum class Phase { PICKING, ORDER_P1, ORDER_P2, CONFIRM, STAGE_SELECT };

    void loadAssets();
    void handleEvents();
    void render();
    void update();

    // Dibujo por fase
    void drawPickingScreen();
    void drawOrderScreen(bool isP1);
    void drawConfirmScreen();
    void drawStageSelect();

    // Helpers
    int  getCardAt(sf::Vector2i mousePos) const;
    void drawTextShadow(const std::string& str, unsigned int size,
                        sf::Color color, float x, float y,
                        sf::Text::Style style = sf::Text::Regular);
    void drawPortrait(int charIndex, float x, float y, float size,
                      sf::Color borderColor = sf::Color::Transparent,
                      float borderThick = 0.f);

    // ── Ventana y fuente ──────────────────────────────────────────────────────
    sf::RenderWindow window;
    sf::Font         font;
    Phase            phase;

    // ── Datos de personajes ───────────────────────────────────────────────────
    std::vector<CharacterData> characters;   // 6 personajes
    std::vector<sf::Texture>   portraits;
    std::vector<sf::Sprite>    portraitSprites;

    // ── Grid de selección ─────────────────────────────────────────────────────
    static const int COLS    = 3;
    static const int ROWS    = 2;
    static const int CARD_W  = 200;
    static const int CARD_H  = 200;
    static const int GRID_X  = 100;
    static const int GRID_Y  = 150;
    static const int PADDING = 20;

    int hoveredCard;   // carta bajo el cursor (-1 = ninguna)

    // ── Sistema de picks (alternando J1/J2, 3 picks cada uno) ────────────────
    // pickOrder[i] = 0 → J1 elige en el turno i, 1 → J2
    // Orden: J1, J2, J1, J2, J1, J2
    int pickTurn;           // 0..5 (cuántos picks se han hecho)
    std::vector<int> p1Picks;   // índices elegidos por J1 (0..5)
    std::vector<int> p2Picks;   // índices elegidos por J2 (0..5)

    // ── Orden de batalla ──────────────────────────────────────────────────────
    // Después de los picks, cada jugador ordena su equipo.
    // orderSelected[i] = índice dentro de p1Picks/p2Picks ya colocado
    std::array<int,3> p1Team;   // equipo J1 en orden de batalla
    std::array<int,3> p2Team;   // equipo J2 en orden de batalla
    std::vector<int>  orderSelected;  // cuáles ya se colocaron al ordenar
    int               orderSlot;      // slot actual a llenar (0,1,2)

    // ── Fondo VS y escenario ──────────────────────────────────────────────────
    sf::Texture vsBgTexture;
    sf::Sprite  vsBgSprite;

    static const int NUM_STAGES = 3;
    sf::Texture  stageBgTextures[NUM_STAGES];
    sf::Sprite   stageBgSprites[NUM_STAGES];
    int          selectedStage;
    sf::Clock    stageAnimClock;
    bool         stageAnimDone;

    bool launchBattle;
};

#endif // CHARACTER_SELECT_H
