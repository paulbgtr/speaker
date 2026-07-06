#include "audio/audio.h"
#include "config.h"
#include "display/display.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <StationManager.h>
#include <AudioPlayer.h>

static String currentTrack = "";

bool buttonPressed(int pin) {
  static unsigned long lastDebounce[40] = {0};
  static bool lastState[40] = {HIGH};

  bool state = digitalRead(pin);
  if (state != lastState[pin] && millis() - lastDebounce[pin] > 50) {
    lastDebounce[pin] = millis();
    lastState[pin] = state;
    if (state == LOW)
      return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("=== BOOT ===");

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS MOUNT FAILED!");
    return;
  }

  StationManager stationManager = StationManager();
  stationManager.loadFromFile("/stations.json");

  pinMode(CONTROLS_PLAY, INPUT_PULLUP);
  pinMode(CONTROLS_NEXT, INPUT_PULLUP);
  displayInit();

  WiFiManager wifiManager;

  wifiManager.setConfigPortalTimeout(180);

  bool connected = wifiManager.autoConnect("LoFi-Speaker-Setup");

  if (!connected) {
    Serial.println("Failed to connect, restarting...");
    ESP.restart();
  }

  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());

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

  if (buttonPressed(CONTROLS_PLAY))
    audioToggle();

  if (buttonPressed(CONTROLS_NEXT)) {
    audioNextStation();
    displayShow(audioGetStationName().c_str(), currentTrack);
  }
}
