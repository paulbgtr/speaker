#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Audio.h"

const char* ssid     = "";
const char* password = "";

Audio audio;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* stations[] = {
    "http://ice3.somafm.com/fluid-128-mp3",
    "http://ice1.somafm.com/groovesalad-128-mp3",
    "http://ice1.somafm.com/dronezone-128-mp3",
};
const char* stationNames[] = {
    "Fluid",
    "Groove Salad",
    "Drone Zone",
};
int currentStation = 0;
int totalStations = 3;

String currentTrack = "";

void showDisplay() {
    display.clearDisplay();

    struct tm t;
    if (getLocalTime(&t)) {
        char timeBuf[6];
        strftime(timeBuf, sizeof(timeBuf), "%H:%M", &t);
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(timeBuf);

        const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        display.setTextSize(1);
        display.setCursor(90, 6);
        display.println(days[t.tm_wday]);
    }

    display.drawLine(0, 18, 128, 18, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 22);
    display.println(stationNames[currentStation]);

    display.setCursor(0, 36);
    display.println(currentTrack.substring(0, 21));

    display.display();
}

void audio_showstreamtitle(const char* info) {
    currentTrack = String(info);
    showDisplay();
}

void audio_info(const char* info) {
    Serial.println(info);
}

void setup() {
    Serial.begin(115200);

    Wire.begin(1, 2);  // SDA=GPIO1, SCL=GPIO2
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Connecting...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi OK");

    configTime(3 * 3600, 0, "pool.ntp.org");

    audio.setPinout(5, 4, 6);
    audio.setVolume(15);
    audio.setConnectionTimeout(500, 2700);
    audio.connecttohost(stations[currentStation]);

    showDisplay();
}

void loop() {
    audio.loop();

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 60000) {
        lastUpdate = millis();
        showDisplay();
    }
}
