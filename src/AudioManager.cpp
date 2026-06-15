#include "../include/AudioManager.h"
#include <iostream>

// ── Definición de miembros estáticos ─────────────────────────────────────────
sf::Music           AudioManager::music;
AudioManager::Track AudioManager::currentTrack  = AudioManager::Track::NONE;
float               AudioManager::currentVolume = 10.f;   // 50% por defecto

sf::SoundBuffer AudioManager::roundBuffers[3];
sf::Sound       AudioManager::roundSound;
bool            AudioManager::roundBuffersLoaded = false;

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

// ── Anuncio de ronda (round_1/2/3.mp3, superpuesto sobre la música) ──────────
void AudioManager::playRoundAnnounce(int roundNumber)
{
    // Cargar los 3 buffers una sola vez (lazy load)
    if (!roundBuffersLoaded)
    {
        const std::string paths[3] = {
            "assets/sounds/round_1.mp3",
            "assets/sounds/round_2.mp3",
            "assets/sounds/round_3.mp3"
        };
        for (int i = 0; i < 3; ++i)
        {
            if (!roundBuffers[i].loadFromFile(paths[i]))
                std::cerr << "[Audio] No se pudo cargar '" << paths[i] << "'\n";
        }
        roundBuffersLoaded = true;
    }

    // Ronda 1->idx0, 2->idx1, 3->idx2. Ronda 4+ reutiliza el sonido de ronda 3.
    int idx = roundNumber - 1;
    if (idx < 0) idx = 0;
    if (idx > 2) idx = 2;

    roundSound.setBuffer(roundBuffers[idx]);
    roundSound.setVolume(currentVolume);
    roundSound.play();
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
