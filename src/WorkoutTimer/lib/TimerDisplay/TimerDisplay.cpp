#include <Arduino.h>

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <stdlib.h>
#include "pico/stdlib.h"
#include "okcolor.hpp"
#endif

#include "TimerDisplay.h"

TimerDisplay::TimerDisplay() {
#if defined(ESP8266)
    _lcd = new LiquidCrystal(D1, D2, D4, D5, D6, D7);
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    _graphics = new pimoroni::PicoGraphics_PenRGB888(53, 11, nullptr);
    _galactic_unicorn = new pimoroni::GalacticUnicorn();
#endif
}

void TimerDisplay::init() {
#if defined(ESP8266)
    // set up the LCD's number of columns and rows:
	_lcd->begin(16, 2);

	// Clears the LCD screen
	_lcd->clear();
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    _galactic_unicorn->init();

    _graphics->set_pen(0, 0, 0);
    _graphics->clear();
#endif

}

uint8_t lastMode = 0;
uint8_t lastState = 0;
uint8_t lastInterval = 0;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
std::string message = "Pirate. Monkey. Robot. Ninja.";
float scroll = -53.0f;
#endif

void TimerDisplay::update(TimerFrame& frame) {
#if defined(ESP8266)
    // clear the display if we need to
    if ((lastMode != frame.mode) ||
        (lastState != frame.state) ||
        (lastInterval != frame.interval)) {
            _lcd->clear();
        }

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):

    // display the mode on the first line
    _lcd->setCursor(0, 0);

    switch (frame.mode)
    {
        case TIMER_MODE_DOWN: 
            _lcd->print("FOR TIME");
            break;
        case TIMER_MODE_UP: 
            _lcd->print("AMRAP");
            break;
        case TIMER_MODE_EMOM: 
            _lcd->print("EMOM");
            break;
        default:
            _lcd->print("UNSET");
            break;
    }

    _lcd->setCursor(9, 0);
    switch (frame.state)
    {
        case TIMER_STATE_STANDBY: 
            _lcd->print("STDBY");
            break;
        case TIMER_STATE_RUN: 
            _lcd->print("RUN");
            break;
        case TIMER_STATE_COMPLETE: 
            _lcd->print("DONE");
            break;
        default:
            _lcd->print("UNSET");
            break;
    }

    // set to line 2
    _lcd->setCursor(0, 1);

    // print the frame display
    _lcd->printf("%02u:%02u:%02u", frame.displayTime[TIME_HOURS], frame.displayTime[TIME_MINUTES], frame.displayTime[TIME_SECONDS]);

    _lcd->setCursor(9, 1);
    _lcd->printf("%u/%u", frame.interval, frame.intervals);

#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    int width = _graphics->measure_text(message, 1);
    scroll += 0.25f;

    if(scroll > width) {
      scroll = -53.0f;
    }

    ok_color::HSL hsl{scroll / 100.0f, 1.0f, 0.5f};
    ok_color::RGB rgb = ok_color::okhsl_to_srgb(hsl);
    _graphics->set_pen(rgb.r * 255, rgb.g * 255, rgb.b * 255);
    _graphics->text(message, pimoroni::Point(0 - scroll, 5), -1, 0.55);

    _galactic_unicorn->update(_graphics);
#endif

    lastMode = frame.mode;
    lastState = frame.state;
    lastInterval = frame.interval;
}