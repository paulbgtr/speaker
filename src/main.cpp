#include "audio/audio.h"
#include "config.h"
#include "display/display.h"
#include <Arduino.h>
#include <WiFi.h>

static String currentTrack = "";

bool buttonPressed(int pin) {
    static unsigned long lastDebounce[40] = {0};
    static bool lastState[40] = {HIGH};

    bool state = digitalRead(pin);
    if (state != lastState[pin] && millis() - lastDebounce[pin] > 50) {
        lastDebounce[pin] = millis();
        lastState[pin] = state;
        if (state == LOW) return true;
    }
    return false;
}

void audio_showstreamtitle(const char *info) {
  currentTrack = String(info);
  displayShow(audioGetStationName().c_str(), currentTrack);
}

void audio_info(const char *info) { Serial.println(info); }

void setup() {
  Serial.begin(115200);

  pinMode(CONTROLS_PLAY, INPUT_PULLUP);
  pinMode(CONTROLS_NEXT, INPUT_PULLUP);
  displayInit();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  configTime(TIMEZONE * 3600, 0, "pool.ntp.org");

  audioInit(2);
  displayShow(audioGetStationName().c_str(), currentTrack);
}

void loop() {
  audioLoop();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    lastUpdate = millis();
    displayShow(audioGetStationName().c_str(), currentTrack);
  }

  if (buttonPressed(CONTROLS_PLAY)) audioToggle();
}
