#include "audio/audio.h"
#include "config.h"
#include "display/display.h"
#include <Arduino.h>
#include <WiFi.h>

static String currentTrack = "";

void audio_showstreamtitle(const char *info) {
  currentTrack = String(info);
  displayShow(audioGetStationName().c_str(), currentTrack);
}

void audio_info(const char *info) { Serial.println(info); }

void setup() {
  Serial.begin(115200);

  displayInit();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  configTime(TIMEZONE * 3600, 0, "pool.ntp.org");

  audioInit(0);
  displayShow(audioGetStationName().c_str(), currentTrack);
}

void loop() {
  audioLoop();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    lastUpdate = millis();
    displayShow(audioGetStationName().c_str(), currentTrack);
  }
}
