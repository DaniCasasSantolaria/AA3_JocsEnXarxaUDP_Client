#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <cassert>
#include <memory>
#include <iostream>

#define AUDIO AudioManager::Instance()

class AudioManager {
private:
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> _clipBuffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>> _clips;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> _songs;
    bool _muted = false;
    bool _mutedSounds = false;
    bool _mutedMusic = false;

    AudioManager() = default;
    AudioManager(AudioManager&) = delete;
    AudioManager& operator =(const AudioManager&) = delete;
    ~AudioManager() = default;

public:
    inline static AudioManager* Instance() {
        static AudioManager instance;
        return &instance;
    }

    inline void LoadClip(const std::string& name) {
        if (_clips.find(name) != _clips.end()) return;

        std::unique_ptr<sf::SoundBuffer> buffer(new sf::SoundBuffer());
        if (!buffer->loadFromFile("resources/audio/sfx/" + name + ".wav")) {
            std::cout << "Clip not found: " << name << std::endl;
            return;
        }

        std::unique_ptr<sf::Sound> sound(new sf::Sound(*buffer));
        _clipBuffers.emplace(name, std::move(buffer));
        _clips.emplace(name, std::move(sound));
    }

    inline void LoadSong(const std::string& name) {
        if (_songs.find(name) != _songs.end()) return;

        std::unique_ptr<sf::Music> music(new sf::Music());
        if (!music->openFromFile("resources/audio/music/" + name + ".mp3")) {
            std::cout << "Song not found: " << name << std::endl;
            return;
        }

        _songs.emplace(name, std::move(music));
    }

    inline void PlayClip(const std::string& name, int repetitions, int volume) {
        if (_mutedSounds) return;

        auto it = _clips.find(name);
        if (it == _clips.end() || !it->second) {
            std::cout << "Clip not found: " << name << std::endl;
            return;
        }

        sf::Sound* sound = it->second.get();
        sound->setVolume(static_cast<float>(volume) * 100.f / 128.f);
        sound->setLooping(repetitions == -1);
        sound->play();
    }

    inline void PlaySong(const std::string& name, int volume) {
        if (_mutedMusic) return;

        auto it = _songs.find(name);
        if (it == _songs.end() || !it->second) {
            std::cout << "Song not found: " << name << std::endl;
            return;
        }

        sf::Music* music = it->second.get();
        music->setVolume(static_cast<float>(volume) * 100.f / 128.f);
        music->setLooping(true);
        music->play();
    }

    inline void MuteMusic() {
        _mutedMusic = true;
        for (std::unordered_map<std::string, std::unique_ptr<sf::Music>>::iterator it = _songs.begin(); it != _songs.end(); ++it) {
            it->second->pause();
        }
    }

    inline void MuteSounds() {
        _mutedSounds = true;
        for (std::unordered_map<std::string, std::unique_ptr<sf::Sound>>::iterator it = _clips.begin(); it != _clips.end(); ++it) {
            it->second->pause();
        }
    }

    inline void Mute() {
        _mutedMusic = true;
        _mutedSounds = true;
        _muted = true;
        for (std::unordered_map<std::string, std::unique_ptr<sf::Sound>>::iterator it = _clips.begin(); it != _clips.end(); ++it) {
            it->second->pause();
        }
        for (std::unordered_map<std::string, std::unique_ptr<sf::Music>>::iterator it = _songs.begin(); it != _songs.end(); ++it) {
            it->second->pause();
        }
    }

    inline void Unmute() {
        _mutedMusic = false;
        _mutedSounds = false;
        _muted = false;
        for (std::unordered_map<std::string, std::unique_ptr<sf::Sound>>::iterator it = _clips.begin(); it != _clips.end(); ++it) {
            it->second->play();
        }
        for (std::unordered_map<std::string, std::unique_ptr<sf::Music>>::iterator it = _songs.begin(); it != _songs.end(); ++it) {
            it->second->play();
        }
    }

    inline bool GetMuted() const { return _muted; }

    inline void StopAudio() {
        for (std::unordered_map<std::string, std::unique_ptr<sf::Music>>::iterator it = _songs.begin(); it != _songs.end(); ++it) {
            it->second->stop();
        }
        for (std::unordered_map<std::string, std::unique_ptr<sf::Sound>>::iterator it = _clips.begin(); it != _clips.end(); ++it) {
            it->second->stop();
        }
    }
};
