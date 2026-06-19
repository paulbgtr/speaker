#pragma once
#include <Arduino.h>

void audioInit(int stationIndex);
void audioStop();
void audioResume();
void audioLoop();
void audioNextStation();
void audioPrevStation();
String audioGetStationName();
