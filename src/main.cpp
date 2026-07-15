#include "HWCDC.h"
#include "config.h"
#include "esp32-hal-gpio.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <AudioPlayer.h>
#include <Display.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <StationManager.h>
#include <StationServer.h>
#include <WiFiManager.h>
#include <sys/_types.h>

StationManager stationManager = StationManager();
AudioPlayer audioPlayer = AudioPlayer(I2S_BCLK, I2S_LRC, I2S_DIN);
Display display = Display();
AsyncWebServer server(80);
StationServer stationServer(server, stationManager);
WiFiManager wifiManager;

unsigned long pressTime = 0;
const int longPressThreshold = 1000;
const int debounceDelay = 50;

void refreshDisplay() {
  display.clear();
  display.drawClock();
  display.drawHorizontalLine();
  display.drawStationInfo(stationManager.current().name.c_str());
  display.flush();
}

template <typename ShortF, typename LongF>
void handleButtonPress(int buttonPin, bool &isPressed, unsigned long &pressTime,
                       unsigned long &lastDebounceTime, ShortF shortPress,
                       LongF longPress) {
  bool currentState = digitalRead(buttonPin) == LOW;

  if (millis() - lastDebounceTime > debounceDelay) {
    if (currentState && !isPressed) {
      lastDebounceTime = millis();
      pressTime = millis();
      isPressed = true;
    } else if (!currentState && isPressed) {
      lastDebounceTime = millis();
      isPressed = false;
      unsigned long duration = millis() - pressTime;

      if (duration >= longPressThreshold) {
        longPress();
      } else {
        shortPress();
      }
    }
  }
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

  wifiManager.setConfigPortalTimeout(180);

  bool connected = wifiManager.autoConnect("LoFi-Speaker-Setup");

  if (!connected) {
    Serial.println("Failed to connect, restarting...");
    ESP.restart();
  }

  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("lofi-speaker")) {
    Serial.println("mDNS failed to start");
  }

  MDNS.addService("http", "tcp", 80);

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

  static bool playPressed = false;
  static unsigned long playPressTime = 0;
  static unsigned long lastPlayDebounce = 0;
  static bool nextPressed = false;
  static unsigned long nextPressTime = 0;
  static unsigned long lastNextDebounce = 0;

  handleButtonPress(
      CONTROLS_PLAY, playPressed, playPressTime, lastPlayDebounce,
      []() { audioPlayer.toggleAudio(); },
      []() { wifiManager.resetSettings(); });

  handleButtonPress(
      CONTROLS_NEXT, nextPressed, nextPressTime, lastNextDebounce,
      []() {
        stationManager.next();
        audioPlayer.play(stationManager.current().url.c_str());

        refreshDisplay();
      },
      []() {});
}
