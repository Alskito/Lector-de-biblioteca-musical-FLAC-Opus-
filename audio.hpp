#pragma once
#include <string>


class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    bool start();
    bool available() const;

    bool play(const std::string& file);   
    void pause();
    void resume();
    void stop();
    bool finished() const;                

    const std::string& message() const;   

private:
    void* music;                          
    bool ready;
    std::string lastMessage;
};
