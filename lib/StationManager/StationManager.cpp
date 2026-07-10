#include "StationManager.h"
#include "FS.h"

StationManager::StationManager() { currentIndex_ = 0; }

bool StationManager::loadFromFile(const char *path) {
  File file = LittleFS.open(path, FILE_READ);
  if (!file) {
    Serial.println("no file");
    return false;
  }

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.c_str());
    return false;
  }

  JsonArray stations = doc["stations"];
  for (JsonObject station : stations) {
    const char *name = station["name"];
    const char *url = station["url"];

    stations_.push_back({name, url});
  }

  return true;
}

void StationManager::next() {
  if (stations_.empty())
    return;

  currentIndex_ = (currentIndex_ + 1) % stations_.size();
}

Station StationManager::current() {
  if (stations_.size() <= 0)
    return Station{};

  return stations_[currentIndex_];
}

void StationManager::addStation(const char *name, const char *url) {
  stations_.push_back({name, url});
}

bool StationManager::saveToFile(const char *path) {
  File file = LittleFS.open(path, FILE_WRITE);

  JsonDocument doc;
  JsonArray arr = doc["stations"].to<JsonArray>();

  for (const Station &station : stations_) {
    JsonObject obj = arr.add<JsonObject>();
    obj["name"] = station.name;
    obj["url"] = station.url;
  }

  if (serializeJson(doc, file) == 0) {
    file.close();
    return false;
  }

  file.close();
  return true;
}

std::vector<Station> &StationManager::getStations() { return stations_; }

std::optional<Station> StationManager::deleteStation(size_t index) {
  if (index >= stations_.size()) {
    return std::nullopt;
  }

  Station deleted = stations_[index];

  stations_.erase(stations_.begin() + index);
  return deleted;
}

std::optional<Station>
StationManager::updateStation(size_t index, const char *name, const char *url) {
  if (index >= stations_.size()) {
    return std::nullopt;
  }

  Station &updated = stations_[index];

  updated.name = name;
  updated.url = url;

  return updated;
};
