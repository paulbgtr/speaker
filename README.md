# LoFi Speaker

A WiFi internet radio speaker built on the Seeed XIAO ESP32S3.

## Features

- Streams internet radio over I2S
- OLED display showing the clock, current station, and track
- Two physical buttons: play/pause and next station
- WiFi setup via captive portal on first boot
- Reachable at `http://lofi-speaker.local` (mDNS)
- Web UI for managing stations (add/edit/delete), backed by a small REST API
- Stations persisted as JSON on the device's LittleFS filesystem

## Hardware

- Seeed XIAO ESP32S3
- I2S DAC (e.g. MAX98357A) for audio out
- SSD1306 OLED display (I2C)
- Two push buttons

Pins are defined in `src/config.h`.

## Project structure

```
lib/
  StationManager/   station list: load/save/add/update/delete, current selection
  AudioPlayer/       wraps the I2S audio stream
  Display/           OLED rendering
  StationServer/     HTTP API for managing stations
src/
  main.cpp           wiring/coordination, button handling
data/                web UI + stations.json, flashed to LittleFS
```

## Building

```
pio run                      # build
pio run -t upload            # flash firmware
pio run -t uploadfs          # flash data/ to LittleFS
```

On first boot, connect to the `LoFi-Speaker-Setup` WiFi network to configure your home WiFi. Once connected, manage stations at `http://lofi-speaker.local`.

## TODO

**UI/polish**

- [x] "Cancel edit" button (currently the only way out of edit mode without submitting is refreshing)
- [ ] Confirmation before deleting a station
- [ ] Error handling on `fetch()` calls in `script.js` (currently fails silently if the device is unreachable)

**Real features**

- [ ] Persist last-played station and volume across reboot
- [ ] Reconnect/retry when a stream drops mid-playback
- [ ] Volume control (currently fixed at construction time)

**Bigger**

- [ ] WiFi reset (long-press a button to call `wifiManager.resetSettings()`, for reconfiguring without reflashing)
- [ ] OTA firmware updates (avoid needing a USB connection for every change)
