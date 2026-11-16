#include "audio.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

AudioPlayer::AudioPlayer() : music(nullptr), ready(false) {}

AudioPlayer::~AudioPlayer() {
    stop();
    if (ready) {
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
    }
}

bool AudioPlayer::start() {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        lastMessage = SDL_GetError();
        return false;
    }


    int loaded = Mix_Init(MIX_INIT_FLAC | MIX_INIT_OPUS);
    if (!(loaded & MIX_INIT_FLAC)) {
        lastMessage += "FLAC files will not play";
    }
    if (!(loaded & MIX_INIT_OPUS)) {
        lastMessage += "Opus files will not play";
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
        lastMessage = Mix_GetError();
        Mix_Quit();
        SDL_Quit();
        return false;
    }

    ready = true;
    return true;
}

bool AudioPlayer::available() const {
    return ready;
}

bool AudioPlayer::play(const std::string& file) {
    if (!ready) {
        lastMessage = "there is no audio device";
        return false;
    }

    stop(); 

    Mix_Music* loaded = Mix_LoadMUS(file.c_str());
    if (loaded == nullptr) {
        lastMessage = Mix_GetError();
        return false;
    }

    if (Mix_PlayMusic(loaded, 1) != 0) { 
        lastMessage = Mix_GetError();
        Mix_FreeMusic(loaded);
        return false;
    }

    music = loaded;
    return true;
}

void AudioPlayer::pause() {
    if (ready) {
        Mix_PauseMusic();
    }
}

void AudioPlayer::resume() {
    if (ready) {
        Mix_ResumeMusic();
    }
}

void AudioPlayer::stop() {
    if (!ready) {
        return;
    }
    Mix_HaltMusic();
    if (music != nullptr) {
        Mix_FreeMusic(static_cast<Mix_Music*>(music));
        music = nullptr;
    }
}

bool AudioPlayer::finished() const {
    return !ready || Mix_PlayingMusic() == 0;
}

const std::string& AudioPlayer::message() const {
    return lastMessage;
}
