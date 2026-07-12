#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <optional>
#include <vector>

struct Station {
  int id;
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
  std::optional<Station> deleteStation(int id);
  std::optional<Station> updateStation(int id, const char *name,
                                       const char *url);

private:
  int nextId_;
  std::vector<Station> stations_;
  int currentIndex_;
};
