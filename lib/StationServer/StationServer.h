#pragma once
#include <ESPAsyncWebServer.h>
#include <StationManager.h>

class StationServer {
public:
  StationServer(AsyncWebServer &server, StationManager &stationManager);
  void begin();

private:
  AsyncWebServer &server_;
  StationManager &stationManager_;
  void initEndpoints();
};
