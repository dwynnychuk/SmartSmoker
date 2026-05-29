#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "StateMachine.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

class DisplayController {
public:
    DisplayController();
    bool begin();
    void render(GrillState _state, const Telemetry& data, float _setTemp, ErrorCode _error);

private:
    void _drawHeader(const char* stateLabel, float setTemp);
    void _drawBigTemp(float temp);
    void _drawFooter(const char* left, const char* right);
    void _drawAlarm(ErrorCode error);

    Adafruit_SSD1306 _display;
};

#endif