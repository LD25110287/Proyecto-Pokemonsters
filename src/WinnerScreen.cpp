#include "../include/WinnerScreen.h"
#include <iostream>

// ── Portraits por índice de personaje (0-5) ──────────────────────────────────
static const char* PORTRAIT_PATHS[6] = {
    "assets/images/champ_select_1.png",
    "assets/images/champ_select_2.png",
    "assets/images/champ_select_3.png",
    "assets/images/champ_select_4.png",
    "assets/images/champ_select_5.png",
    "assets/images/champ_select_6.png",
};

// ── Constructor ───────────────────────────────────────────────────────────────
WinnerScreen::WinnerScreen(int winnerNumber, const std::array<int,3>& winnerTeam)
    : window(sf::VideoMode(800, 600), "Pokemonsters - Resultado", sf::Style::Close)
    , winnerNumber_(winnerNumber)
    , winnerTeam_(winnerTeam)
{
    window.setFramerateLimit(60);
    portraitLoaded_[0] = portraitLoaded_[1] = portraitLoaded_[2] = false;
    loadAssets();
}

// ── loadAssets ───────────────────────────────────────────────────────────────
void WinnerScreen::loadAssets()
{
    // Fuente
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (auto& p : fontPaths)
        if (font.loadFromFile(p)) break;

    // Fondo GANADOR.png (ya es 800x600, sin necesidad de escalar)
    if (bgTexture.loadFromFile("assets/images/GANADOR.png"))
    {
        bgSprite.setTexture(bgTexture);
    }
    else
        std::cerr << "[WinnerScreen] No se pudo cargar assets/images/GANADOR.png\n";

    // Retratos del equipo ganador
    const float PORTRAIT_SIZE = 100.f;
    const float gap   = 30.f;
    const float totalW = 3 * PORTRAIT_SIZE + 2 * gap;
    const float startX = (800.f - totalW) / 2.f;
    const float y       = 470.f;

    for (int i = 0; i < 3; ++i)
    {
        int charIdx = winnerTeam_[i];
        if (charIdx < 0 || charIdx > 5) charIdx = 0;

        if (portraitTextures_[i].loadFromFile(PORTRAIT_PATHS[charIdx]))
        {
            portraitLoaded_[i] = true;
            portraitSprites_[i].setTexture(portraitTextures_[i]);

            sf::Vector2u sz = portraitTextures_[i].getSize();
            portraitSprites_[i].setScale(PORTRAIT_SIZE / sz.x, PORTRAIT_SIZE / sz.y);
            portraitSprites_[i].setPosition(startX + i * (PORTRAIT_SIZE + gap), y);
        }
        else
        {
            portraitLoaded_[i] = false;
            std::cerr << "[WinnerScreen] No se pudo cargar " << PORTRAIT_PATHS[charIdx] << "\n";
        }
    }
}

// ── render ───────────────────────────────────────────────────────────────────
void WinnerScreen::render()
{
    window.clear(sf::Color(20, 20, 35));

    if (bgTexture.getSize().x > 0)
        window.draw(bgSprite);

    // ── "JUGADOR X" debajo del cartel "YOU WIN!" ─────────────────────────────
    std::string label = "JUGADOR " + std::to_string(winnerNumber_);

    sf::Text shadow(label, font, 46);
    shadow.setStyle(sf::Text::Bold);
    shadow.setFillColor(sf::Color(0, 0, 0, 200));
    float sx = (800.f - shadow.getLocalBounds().width) / 2.f;
    shadow.setPosition(sx + 3.f, 408.f);
    window.draw(shadow);

    sf::Text title(label, font, 46);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color(255, 215, 0));
    float tx = (800.f - title.getLocalBounds().width) / 2.f;
    title.setPosition(tx, 405.f);
    window.draw(title);

    // ── Retratos del equipo ganador ──────────────────────────────────────────
    const float PORTRAIT_SIZE = 100.f;
    for (int i = 0; i < 3; ++i)
    {
        if (!portraitLoaded_[i]) continue;

        // Marco dorado alrededor de cada retrato
        sf::RectangleShape frame(sf::Vector2f(PORTRAIT_SIZE + 6.f, PORTRAIT_SIZE + 6.f));
        frame.setFillColor(sf::Color(20, 20, 20, 150));
        frame.setOutlineColor(sf::Color(255, 215, 0));
        frame.setOutlineThickness(3.f);
        sf::Vector2f pos = portraitSprites_[i].getPosition();
        frame.setPosition(pos.x - 3.f, pos.y - 3.f);
        window.draw(frame);

        window.draw(portraitSprites_[i]);
    }

    // ── Hint para continuar ───────────────────────────────────────────────────
    std::string hint = "Haz click para volver al menu";
    sf::Text hintShadow(hint, font, 18);
    hintShadow.setFillColor(sf::Color(0, 0, 0, 180));
    float hsx = (800.f - hintShadow.getLocalBounds().width) / 2.f;
    hintShadow.setPosition(hsx + 2.f, 582.f);
    window.draw(hintShadow);

    sf::Text hintText(hint, font, 18);
    hintText.setFillColor(sf::Color::White);
    float hx = (800.f - hintText.getLocalBounds().width) / 2.f;
    hintText.setPosition(hx, 580.f);
    window.draw(hintText);

    window.display();
}

// ── run ───────────────────────────────────────────────────────────────────────
void WinnerScreen::run()
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                window.close();   // click en cualquier lugar -> volver al menú
            }

            if (event.type == sf::Event::KeyPressed &&
                (event.key.code == sf::Keyboard::Escape ||
                 event.key.code == sf::Keyboard::Enter))
            {
                window.close();
            }
        }

        if (!window.isOpen()) break;
        render();
    }
}
