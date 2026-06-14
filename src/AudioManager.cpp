#include "../include/AudioManager.h"
#include <iostream>

// ── Definición de miembros estáticos ─────────────────────────────────────────
sf::Music           AudioManager::music;
AudioManager::Track AudioManager::currentTrack  = AudioManager::Track::NONE;
float               AudioManager::currentVolume = 50.f;   // 50% por defecto

// ── Intro (3 seg, una sola vez al iniciar la app) ────────────────────────────
void AudioManager::playIntro()
{
    sf::Music intro;
    if (!intro.openFromFile("assets/sounds/introduction_game.mp3"))
    {
        std::cerr << "[Audio] No se pudo cargar 'introduction_game.mp3'\n";
        return;
    }

    intro.setLoop(false);
    intro.setVolume(currentVolume);
    intro.play();

    // Bloqueante: aún no hay ventana abierta, así que no afecta el render.
    sf::sleep(sf::seconds(3.f));

    intro.stop();
}

// ── Música de menú (en bucle) ────────────────────────────────────────────────
void AudioManager::playMenuMusic()
{
    if (currentTrack == Track::MENU)
        return;   // ya está sonando, no reiniciar

    music.stop();
    if (music.openFromFile("assets/sounds/musica_menu.mp3"))
    {
        music.setLoop(true);
        music.setVolume(currentVolume);
        music.play();
        currentTrack = Track::MENU;
    }
    else
    {
        std::cerr << "[Audio] No se pudo cargar 'musica_menu.mp3'\n";
        currentTrack = Track::NONE;
    }
}

// ── Música de combate (en bucle) ─────────────────────────────────────────────
void AudioManager::playBattleMusic()
{
    if (currentTrack == Track::BATTLE)
        return;   // ya está sonando, no reiniciar

    music.stop();
    if (music.openFromFile("assets/sounds/musica_fondo_combate.mp3"))
    {
        music.setLoop(true);
        music.setVolume(currentVolume);
        music.play();
        currentTrack = Track::BATTLE;
    }
    else
    {
        std::cerr << "[Audio] No se pudo cargar 'musica_fondo_combate.mp3'\n";
        currentTrack = Track::NONE;
    }
}

// ── Volumen ───────────────────────────────────────────────────────────────────
void AudioManager::setVolume(float volume)
{
    if (volume < 0.f)   volume = 0.f;
    if (volume > 100.f) volume = 100.f;

    currentVolume = volume;
    music.setVolume(currentVolume);
}

float AudioManager::getVolume()
{
    return currentVolume;
}

// ── Detener música ────────────────────────────────────────────────────────────
void AudioManager::stopMusic()
{
    music.stop();
    currentTrack = Track::NONE;
}
