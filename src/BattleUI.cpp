#include "../include/BattleUI.h"
#include "../include/Pokemonster.h"
#include <iostream>

BattleUI::BattleUI(const sf::Vector2u& windowSize)
    : winSize(windowSize), player(nullptr), enemy(nullptr)
{
    // load a font if available; absence is tolerated
    if (!font.loadFromFile("assets/fonts/arial.ttf"))
    {
        // no font, texts will be blank
        std::cerr << "Warning: could not load font assets/fonts/arial.ttf. Move labels may be empty.\n";
    }

    // HP bars setup
    playerHpBack.setSize({250.f, 20.f});
    playerHpBack.setFillColor(sf::Color(100, 100, 100));
    playerHpBack.setPosition(20.f, winSize.y - 80.f);

    playerHpFront.setSize({250.f, 20.f});
    playerHpFront.setFillColor(sf::Color::Green);
    playerHpFront.setPosition(playerHpBack.getPosition());

    enemyHpBack.setSize({250.f, 20.f});
    enemyHpBack.setFillColor(sf::Color(100, 100, 100));
    enemyHpBack.setPosition(winSize.x - 270.f, 20.f);

    enemyHpFront.setSize({250.f, 20.f});
    enemyHpFront.setFillColor(sf::Color::Green);
    enemyHpFront.setPosition(enemyHpBack.getPosition());

    // Move buttons (4) at bottom-right
    moveButtons.resize(4);
    moveTexts.resize(4);
    const float btnW = 220.f;
    const float btnH = 40.f;
    for (int i = 0; i < 4; ++i)
    {
        float x = winSize.x - btnW - 20.f;
        float y = winSize.y - (4 - i) * (btnH + 8.f) - 20.f;
        moveButtons[i].setSize({btnW, btnH});
        moveButtons[i].setFillColor(sf::Color(70, 70, 70));
        moveButtons[i].setPosition(x, y);

        moveTexts[i].setFont(font);
        moveTexts[i].setCharacterSize(16);
        moveTexts[i].setFillColor(sf::Color::White);
        moveTexts[i].setPosition(x + 8.f, y + 6.f);
    }
}

void BattleUI::setPlayerPokemon(Pokemonster* p)
{
    player = p;
}

void BattleUI::setEnemyPokemon(Pokemonster* e)
{
    enemy = e;
}

void BattleUI::update()
{
    if (player)
    {
        float ratio = 1.f;
        if (player->getHPMax() > 0)
            ratio = static_cast<float>(player->getHP()) / static_cast<float>(player->getHPMax());
        playerHpFront.setSize({250.f * ratio, 20.f});
        if (ratio > 0.6f) playerHpFront.setFillColor(sf::Color::Green);
        else if (ratio > 0.3f) playerHpFront.setFillColor(sf::Color::Yellow);
        else playerHpFront.setFillColor(sf::Color::Red);

        // update move texts if possible
        const auto& moves = player->getMoves();
        for (size_t i = 0; i < moveTexts.size() && i < moves.size(); ++i)
        {
            moveTexts[i].setString(moves[i].name);
        }
    }

    if (enemy)
    {
        float ratio = 1.f;
        if (enemy->getHPMax() > 0)
            ratio = static_cast<float>(enemy->getHP()) / static_cast<float>(enemy->getHPMax());
        enemyHpFront.setSize({250.f * ratio, 20.f});
        if (ratio > 0.6f) enemyHpFront.setFillColor(sf::Color::Green);
        else if (ratio > 0.3f) enemyHpFront.setFillColor(sf::Color::Yellow);
        else enemyHpFront.setFillColor(sf::Color::Red);
    }
}

void BattleUI::draw(sf::RenderTarget& target)
{
    // draw HP bars
    target.draw(playerHpBack);
    target.draw(playerHpFront);
    target.draw(enemyHpBack);
    target.draw(enemyHpFront);

    // draw move buttons and texts
    for (size_t i = 0; i < moveButtons.size(); ++i)
    {
        target.draw(moveButtons[i]);
        if (font.getInfo().family.size() > 0)
            target.draw(moveTexts[i]);
    }
}

int BattleUI::handleMouseClick(sf::Vector2i mousePos)
{
    sf::Vector2f m(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    for (size_t i = 0; i < moveButtons.size(); ++i)
    {
        if (moveButtons[i].getGlobalBounds().contains(m))
            return static_cast<int>(i);
    }
    return -1;
}
