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
    // ORDER:        draft de orden — patrón J1,J2,J2,J1,J1,J2 (6 sub-turnos)
    // CONFIRM:      pantalla final con fondo VS y equipos ordenados
    // STAGE_SELECT: ruleta de escenario
    enum class Phase { PICKING, ORDER, CONFIRM, STAGE_SELECT };

    void loadAssets();
    void handleEvents();
    void render();
    void update();

    // Dibujo por fase
    void drawPickingScreen();
    void drawOrderScreen(bool isP1, int targetRound); 
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
    static const int CARD_W  = 230;   
    static const int CARD_H  = 217;   
    static const int GRID_X  = 40;    
    static const int GRID_Y  = 140;   
    static const int PADDING = 15;

    int hoveredCard;   // carta bajo el cursor (-1 = ninguna)

    // ── Sistema de picks ──────────────────────────────────────────────────────
    int pickTurn;           
    std::vector<int> p1Picks;   
    std::vector<int> p2Picks;   

    // ── Orden de batalla ──────────────────────────────────────────────────────
    std::array<int,3> p1Team;   
    std::array<int,3> p2Team;   
    std::vector<int>  orderSelected;  
    int               orderSlot;      
    int               orderTurn;      

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