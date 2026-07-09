#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>
#include <optional>

struct Station {
  String name;
  String url;
};

class StationManager {
public:
  StationManager();

  bool loadFromFile(const char *path);
  void next();
  Station current();
  void addStation(const char* name, const char* url);
  bool saveToFile(const char* path);

  std::vector<Station> &getStations();
  std::optional<Station> deleteStation(size_t index);

private:
  std::vector<Station> stations_;
  int currentIndex_;
};
