#include "audio.h"
#include "../config.h"
#include <Audio.h>

static Audio audio;
static int currentStation = 0;

static const char *urls[] = {
    "http://ice3.somafm.com/fluid-128-mp3",
    "http://ice1.somafm.com/groovesalad-128-mp3",
    "http://ice1.somafm.com/dronezone-128-mp3",
};
static const char *names[] = {
    "Fluid",
    "Groove Salad",
    "Drone Zone",
};
static const int totalStations = 3;

void audioInit(int stationIndex) {
  currentStation = stationIndex;
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DIN);
  audio.setVolume(15);
  audio.setConnectionTimeout(500, 2700);
  audio.connecttohost(urls[currentStation]);
}

void audioStop() {
    audio.stopSong();
}

void audioResume() {
    audio.setConnectionTimeout(500, 2700);
    audio.connecttohost(urls[currentStation]);
}

void audioLoop() { audio.loop(); }

void audioNextStation() {
  currentStation = (currentStation + 1) % totalStations;
  audio.connecttohost(urls[currentStation]);
}

void audioPrevStation() {
  currentStation = (currentStation - 1 + totalStations) % totalStations;
  audio.connecttohost(urls[currentStation]);
}

String audioGetStationName() { return String(names[currentStation]); }
