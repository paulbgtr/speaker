#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display {
    public:
        Display();
        void init(int sda, int scl);
        void clear();
        void drawClock();
        void drawStationInfo(const char* station);
        void drawHorizontalLine();
        void flush();

    private:
        Adafruit_SSD1306 display_;
};
