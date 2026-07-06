#pragma once
#include <Audio.h>

class AudioPlayer {
public:
  AudioPlayer(int bclkPin, int lrcPin, int dinPin, int volume = 15);

  void toggleAudio();
  bool isAudioPlaying();
  void play(const char *url);
  void loop();

private:
  Audio audio_;
  bool isPlaying_;
  String currentUrl_;
};
