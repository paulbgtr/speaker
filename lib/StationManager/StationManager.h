#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
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

private:
  std::vector<Station> stations_;
  int currentIndex_;
};
