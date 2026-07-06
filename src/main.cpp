#include "HWCDC.h"
#include "config.h"
#include "display/display.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AudioPlayer.h>
#include <LittleFS.h>
#include <StationManager.h>
#include <WiFiManager.h>

StationManager stationManager = StationManager();
AudioPlayer audioPlayer = AudioPlayer(I2S_BCLK, I2S_LRC, I2S_DIN);

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

  stationManager.loadFromFile("/stations.json");
  audioPlayer.play(stationManager.current().url.c_str());

  displayShow(stationManager.current().name.c_str());
}

void loop() {
  audioPlayer.loop();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    lastUpdate = millis();
    displayShow(stationManager.current().name.c_str());
  }

  if (buttonPressed(CONTROLS_PLAY))
    audioPlayer.toggleAudio();

  if (buttonPressed(CONTROLS_NEXT)) {
      stationManager.next();
      audioPlayer.play(stationManager.current().url.c_str());
      displayShow(stationManager.current().name.c_str());
  }
}
