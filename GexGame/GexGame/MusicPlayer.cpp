#include "MusicPlayer.h"
#include <stdexcept>



MusicPlayer::MusicPlayer() {
    m_filenames["gameTheme"] = "../assets/Music/MissionTheme.ogg";
    m_filenames["menuTheme"] = "../assets/Music/MenuTheme.ogg";
    m_filenames["menuTheme2"] = "../assets/Music/MenuTheme2.ogg";


}


MusicPlayer& MusicPlayer::getInstance() {
    static MusicPlayer instance;
    return instance;
}


void MusicPlayer::play(String theme) {
    if (!m_music.openFromFile(m_filenames[theme]))
        throw std::runtime_error("Music could not open file");
    if (m_isPlaying) {
        m_music.setVolume(m_volume);
        m_music.setLoop(true);
        m_music.play();
    }
    else {
        stop();
    }
}


void MusicPlayer::stop() {
    m_music.stop();
}


void MusicPlayer::setPaused(bool paused) {
    if (paused) {
        m_music.pause();
        m_isPlaying = false;
    }
        
    else {
        m_music.play();
        m_isPlaying = true;
    }
        
}


void MusicPlayer::setVolume(float volume) {
    m_volume = volume;
    m_music.setVolume(m_volume);
}