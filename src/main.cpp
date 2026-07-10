#include "HWCDC.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <AudioPlayer.h>
#include <Display.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <StationManager.h>
#include <StationServer.h>
#include <WiFiManager.h>

StationManager stationManager = StationManager();
AudioPlayer audioPlayer = AudioPlayer(I2S_BCLK, I2S_LRC, I2S_DIN);
Display display = Display();
AsyncWebServer server(80);
StationServer stationServer(server, stationManager);

void refreshDisplay() {
  display.clear();
  display.drawClock();
  display.drawHorizontalLine();
  display.drawStationInfo(stationManager.current().name.c_str());
  display.flush();
}

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
  display.init(OLED_SDA, OLED_SCL);

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

  stationServer.begin();

  stationManager.loadFromFile("/stations.json");
  audioPlayer.play(stationManager.current().url.c_str());

  refreshDisplay();
}

void loop() {
  audioPlayer.loop();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    lastUpdate = millis();

    refreshDisplay();
  }

  if (buttonPressed(CONTROLS_PLAY))
    audioPlayer.toggleAudio();

  if (buttonPressed(CONTROLS_NEXT)) {
    stationManager.next();
    audioPlayer.play(stationManager.current().url.c_str());

    refreshDisplay();
  }
}
