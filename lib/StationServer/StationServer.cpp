#include "StationServer.h"

StationServer::StationServer(AsyncWebServer &server,
                             StationManager &stationManager)
    : server_(server), stationManager_(stationManager) {}

void StationServer::initEndpoints() {
  server_.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server_.on("/stations", AsyncWebRequestMethod::HTTP_GET,
             [this](AsyncWebServerRequest *request) {
               std::vector<Station> stations = stationManager_.getStations();

               JsonDocument doc;
               JsonArray arr = doc.to<JsonArray>();

               for (const Station &station : stations) {
                 JsonObject obj = arr.add<JsonObject>();
                 obj["name"] = station.name;
                 obj["url"] = station.url;
               }

               AsyncResponseStream *response =
                   request->beginResponseStream("application/json");
               serializeJson(doc, *response);
               request->send(response);
             });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
      "/stations", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject data = json.as<JsonObject>();
        const char *name = data["name"];
        const char *url = data["url"];

        if (!name || !url) {
          request->send(400, "application/json",
                        "{\"error\":\"missing fields\"}");
          return;
        }

        stationManager_.addStation(name, url);
        stationManager_.saveToFile("/stations.json");

        request->send(200, "application/json", "{\"ok\":true}");
      });

  handler->setMethod(AsyncWebRequestMethod::HTTP_POST);
  server_.addHandler(handler);

  server_.on("/stations", AsyncWebRequestMethod::HTTP_DELETE,
             [this](AsyncWebServerRequest *request) {
               if (!request->hasParam("index")) {
                 request->send(400, "application/json",
                               "{\"error\":\"missing index\"}");
                 return;
               }

               String index = request->getParam("index")->value();

               auto deleted = stationManager_.deleteStation(index.toInt());

               if (!deleted.has_value()) {
                 request->send(404, "application/json",
                               "{\"error\":\"invalid index\"}");
                 return;
               }

               stationManager_.saveToFile("/stations.json");

               request->send(200, "application/json", "{\"ok\":true}");
             });

  AsyncCallbackJsonWebHandler *updateHandler = new AsyncCallbackJsonWebHandler(
      "/stations", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        if (!request->hasParam("index")) {
          request->send(400, "application/json",
                        "{\"error\":\"missing index\"}");
          return;
        }

        String index = request->getParam("index")->value();

        JsonObject data = json.as<JsonObject>();
        const char *name = data["name"];
        const char *url = data["url"];

        if (!name && !url) {
          request->send(400, "application/json",
                        "{\"error\":\"nothing to update\"}");
          return;
        }

        auto updated = stationManager_.updateStation(index.toInt(), name, url);

        if (!updated.has_value()) {
          request->send(404, "application/json",
                        "{\"error\":\"invalid index\"}");
          return;
        }

        stationManager_.saveToFile("/stations.json");
        request->send(200, "application/json", "{\"ok\":true}");
      });

  updateHandler->setMethod(AsyncWebRequestMethod::HTTP_PATCH);
  server_.addHandler(updateHandler);
};

void StationServer::begin() {
  initEndpoints();
  server_.begin();
}
