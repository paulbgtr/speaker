#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <optional>
#include <vector>

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
  bool saveToFile(const char *path);

  std::vector<Station> &getStations();
  void addStation(const char *name, const char *url);
  std::optional<Station> deleteStation(size_t index);
  std::optional<Station> updateStation(size_t index, const char *name,
                                       const char *url);

private:
  std::vector<Station> stations_;
  int currentIndex_;
};
