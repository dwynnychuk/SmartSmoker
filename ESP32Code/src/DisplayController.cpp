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
        _drawHeader("IDLE", _setTemp);
        _drawBigTemp(0.0);
        _drawFooter("Push to Ignite", "");
        break;

        case GrillState::IGNITION:
        _drawHeader("IGNITING", _setTemp);
        _drawBigTemp(0.0);
        _drawFooter("Warming Up", "");
        break;

        case GrillState::TEMP_HOLD:
        _drawHeader("COOKING", _setTemp);
        _drawBigTemp(0.0);
        _drawFooter("Auger: ", "100%");
        break;

        case GrillState::LID_OPEN:
        _drawHeader("LID OPEN", _setTemp);
        _drawBigTemp(0.0);
        _drawFooter("Close Lid Soon", "");
        break;

        case GrillState::COOLDOWN:
        _drawHeader("SHUTDOWN", _setTemp);
        _drawBigTemp(0.0);
        _drawFooter("Cooling Down", "");
        break;

        case GrillState::ERROR:
        _drawAlarm(_error);
        break;
    }

    _display.display();

    return;
}

void DisplayController::_drawHeader(const char* label, float setTemp) {
    _display.setTextSize(1);
    _display.setCursor(0,0);
    _display.print(label);

    _display.setCursor(72,0);
    _display.print("Set:");
    _display.print((int)setTemp);
    _display.print((char)247);  // Degrees
}

void DisplayController::_drawBigTemp(float temp) {
    _display.setTextSize(3);
    _display.setCursor(24, 20);
    _display.print((int)temp);
    _display.print((char)247);  // degrees
}

void DisplayController::_drawFooter(const char* left, const char* right) {
    _display.setTextSize(1);
    _display.setCursor(0, 56);
    _display.print(left);
    _display.setCursor(80, 56);
    _display.print(right);
}

void DisplayController::_drawAlarm(ErrorCode error) {
    
}
