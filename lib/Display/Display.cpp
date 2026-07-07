#include "Display.h"

Display::Display() : display_(128, 64, &Wire, -1) {}

void Display::init(int sda, int scl) {
  Wire.begin(sda, scl);
  display_.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_.clearDisplay();
  display_.display();
}

void Display::clear() { display_.clearDisplay(); }

void Display::drawClock() {
  struct tm t;

  if (!getLocalTime(&t))
    return;

  char timeBuf[6];
  strftime(timeBuf, sizeof(timeBuf), "%H:%M", &t);
  display_.setTextSize(2);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);
  display_.println(timeBuf);

  const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  display_.setTextSize(1);
  display_.setCursor(90, 6);
  display_.println(days[t.tm_wday]);
}

void Display::drawHorizontalLine() {
  display_.drawLine(0, 18, 128, 18, SSD1306_WHITE);
}

void Display::drawStationInfo(const char *station) {
  display_.setTextSize(1);
  display_.setCursor(0, 22);
  display_.println(station);
  display_.setCursor(0, 36);
}

void Display::flush() { display_.display(); }
