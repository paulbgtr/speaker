#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(int bclkPin, int lrcPin, int dinPin, int volume) {
  isPlaying_ = false;
  audio_.setPinout(bclkPin, lrcPin, dinPin);
  audio_.setVolume(volume);
}

void AudioPlayer::play(const char *url) {
  currentUrl_ = url;
  audio_.setConnectionTimeout(500, 2700);
  audio_.connecttohost(currentUrl_.c_str());
  isPlaying_ = true;
}

void AudioPlayer::toggleAudio() {
  if (isPlaying_) {
    audio_.stopSong();
    isPlaying_ = false;
  } else {
    audio_.setConnectionTimeout(500, 2700);
    audio_.connecttohost(currentUrl_.c_str());
    isPlaying_ = true;
  }
}

bool AudioPlayer::isAudioPlaying() { return isPlaying_; }

void AudioPlayer::loop() {
    audio_.loop();
}
