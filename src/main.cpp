#include "ArduinoJson/Array/JsonArray.hpp"
#include "ArduinoJson/Object/JsonObject.hpp"
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

  // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // server.on("/stations", AsyncWebRequestMethod::HTTP_GET,
  //           [](AsyncWebServerRequest *request) {
  //             std::vector<Station> stations = stationManager.getStations();

  //             JsonDocument doc;
  //             JsonArray arr = doc.to<JsonArray>();

  //             for (const Station &station : stations) {
  //               JsonObject obj = arr.add<JsonObject>();
  //               obj["name"] = station.name;
  //               obj["url"] = station.url;
  //             }

  //             AsyncResponseStream *response =
  //                 request->beginResponseStream("application/json");
  //             serializeJson(doc, *response);
  //             request->send(response);
  //           });

  // AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
  //     "/stations", [](AsyncWebServerRequest *request, JsonVariant &json) {
  //       JsonObject data = json.as<JsonObject>();
  //       const char *name = data["name"];
  //       const char *url = data["url"];

  //       if (!name || !url) {
  //         request->send(400, "application/json",
  //                       "{\"error\":\"missing fields\"}");
  //         return;
  //       }

  //       stationManager.addStation(name, url);
  //       stationManager.saveToFile("/stations.json");

  //       request->send(200, "application/json", "{\"ok\":true}");
  //     });
  // handler->setMethod(AsyncWebRequestMethod::HTTP_POST);
  // server.addHandler(handler);

  // server.on("/stations", AsyncWebRequestMethod::HTTP_DELETE,
  //           [](AsyncWebServerRequest *request) {
  //             if (!request->hasParam("index")) {
  //               request->send(400, "application/json",
  //                             "{\"error\":\"missing index\"}");
  //               return;
  //             }

  //             String index = request->getParam("index")->value();

  //             auto deleted = stationManager.deleteStation(index.toInt());

  //             if (!deleted.has_value()) {
  //               request->send(404, "application/json",
  //                             "{\"error\":\"invalid index\"}");
  //               return;
  //             }

  //             stationManager.saveToFile("/stations.json");

  //             request->send(200, "application/json", "{\"ok\":true}");
  //           });

  // AsyncCallbackJsonWebHandler *updateHandler = new AsyncCallbackJsonWebHandler(
  //     "/stations", [](AsyncWebServerRequest *request, JsonVariant &json) {
  //       if (!request->hasParam("index")) {
  //         request->send(400, "application/json",
  //                       "{\"error\":\"missing index\"}");
  //         return;
  //       }

  //       String index = request->getParam("index")->value();

  //       JsonObject data = json.as<JsonObject>();
  //       const char *name = data["name"];
  //       const char *url = data["url"];

  //       if (!name && !url) {
  //         request->send(400, "application/json",
  //                       "{\"error\":\"nothing to update\"}");
  //         return;
  //       }

  //       auto updated = stationManager.updateStation(index.toInt(), name, url);

  //       if (!updated.has_value()) {
  //         request->send(404, "application/json",
  //                       "{\"error\":\"invalid index\"}");
  //         return;
  //       }

  //       stationManager.saveToFile("/stations.json");
  //       request->send(200, "application/json", "{\"ok\":true}");
  //     });

  // updateHandler->setMethod(AsyncWebRequestMethod::HTTP_PATCH);
  // server.addHandler(updateHandler);

  // server.begin();

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
