#include "DisplayController.h"

DisplayController::DisplayController()
    : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

bool DisplayController::begin() {
    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        return false;
    }
    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);
    _display.display();
    return true;
}

void DisplayController::render(GrillState _state, const Telemetry& data, float _setTemp, ErrorCode _error) {
    _display.clearDisplay();

    switch(_state) {
        case GrillState::IDLE:
        //do something
        break;

        case GrillState::IGNITION:
        //do something
        break;

        case GrillState::TEMP_HOLD:
        //do something
        break;

        case GrillState::LID_OPEN:
        //do something
        break;

        case GrillState::COOLDOWN:
        //do something
        break;

        case GrillState::ERROR:
        //do something
        break;
    }

    _display.display();
}