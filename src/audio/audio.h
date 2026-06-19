#pragma once
#include <Arduino.h>

void audioInit(int stationIndex);
void audioLoop();
void audioNextStation();
void audioPrevStation();
String audioGetStationName();
void audioToggle();
bool audioIsPlaying();
