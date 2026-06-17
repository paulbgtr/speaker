#include "display.h"
#include "../config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

static Adafruit_SSD1306 display(128, 64, &Wire, -1);

void displayInit() {
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void displayShow(const char *station, const String &track) {
  display.clearDisplay();

  struct tm t;
  if (getLocalTime(&t)) {
    char timeBuf[6];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M", &t);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(timeBuf);

    const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    display.setTextSize(1);
    display.setCursor(90, 6);
    display.println(days[t.tm_wday]);
  }

  display.drawLine(0, 18, 128, 18, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 22);
  display.println(station);
  display.setCursor(0, 36);
  display.println(track.substring(0, 21));

  display.display();
}
