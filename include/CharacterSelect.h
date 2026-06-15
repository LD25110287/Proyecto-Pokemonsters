#ifndef CHARACTER_SELECT_H
#define CHARACTER_SELECT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>
#include "Pokemonster.h"   // para Attribute

// ── Atributos por personaje (mismo orden que CHAR_TABLE en Game.cpp) ──────────
static const Attribute CHAR_ATTRIBUTES[6] = {
    Attribute::Vacuna,  // 0: Exdarktyranomon
    Attribute::Virus,   // 1: BeelStarmon
    Attribute::Vacuna,  // 2: Bioquetzalmon
    Attribute::Data,    // 3: Jesmon
    Attribute::Vacuna,  // 4: Sleipmon
    Attribute::Vacuna,  // 5: Magnamon
};

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

    const std::array<int,3>& getPlayer1Team() const { return p1Team; }
    const std::array<int,3>& getPlayer2Team() const { return p2Team; }

    int getPlayer1Choice() const { return p1Team[0]; }
    int getPlayer2Choice() const { return p2Team[0]; }

private:
    enum class Phase { PICKING, ORDER, CONFIRM, STAGE_SELECT };

    void loadAssets();
    void handleEvents();
    void render();
    void update();

    void drawPickingScreen();
    void drawOrderScreen(bool isP1, int targetRound);
    void drawConfirmScreen();
    void drawStageSelect();

    int  getCardAt(sf::Vector2i mousePos) const;
    void drawTextShadow(const std::string& str, unsigned int size,
                        sf::Color color, float x, float y,
                        sf::Text::Style style = sf::Text::Regular);
    void drawPortrait(int charIndex, float x, float y, float size,
                      sf::Color borderColor = sf::Color::Transparent,
                      float borderThick = 0.f);

    // Dibuja el ícono de atributo escalado a 'iconSize' px en (x, y) top-left
    void drawAttributeIcon(Attribute attr, float x, float y, float iconSize);

    sf::RenderWindow window;
    sf::Font         font;
    Phase            phase;

    std::vector<CharacterData> characters;
    std::vector<sf::Texture>   portraits;
    std::vector<sf::Sprite>    portraitSprites;

    // Íconos de atributo
    sf::Texture attrTextures[3];   // 0=Vacuna(Va), 1=Virus(Vi), 2=Data(Da)
    bool        attrLoaded[3];

    static const int COLS    = 3;
    static const int ROWS    = 2;
    static const int CARD_W  = 230;
    static const int CARD_H  = 217;
    static const int GRID_X  = 40;
    static const int GRID_Y  = 140;
    static const int PADDING = 15;

    int hoveredCard;

    int pickTurn;
    std::vector<int> p1Picks;
    std::vector<int> p2Picks;

    std::array<int,3> p1Team;
    std::array<int,3> p2Team;
    std::vector<int>  orderSelected;
    int               orderSlot;
    int               orderTurn;

    sf::Texture vsBgTexture;
    sf::Sprite  vsBgSprite;

    // Fondo compartido: galería, selección de personaje y orden
    sf::Texture menuBgTexture_;
    sf::Sprite  menuBgSprite_;

    static const int NUM_STAGES = 3;
    sf::Texture  stageBgTextures[NUM_STAGES];
    sf::Sprite   stageBgSprites[NUM_STAGES];
    int          selectedStage;
    sf::Clock    stageAnimClock;
    bool         stageAnimDone;

    bool launchBattle;
};

#endif // CHARACTER_SELECT_H
