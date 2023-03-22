#ifndef TimerDisplay_h
#define TimerDisplay_h

#include <WorkoutTimer.h>

#if defined(ESP8266)
#include <LiquidCrystal.h>
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include "pico_graphics.hpp"
#include "galactic_unicorn.hpp"
#endif


class TimerDisplay {
    public:
        TimerDisplay();

        void init();

        void update(TimerFrame& frame);

    private:
#if defined(ESP8266)
       LiquidCrystal* _lcd;
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    pimoroni::PicoGraphics_PenRGB888* _graphics;
    pimoroni::GalacticUnicorn* _galactic_unicorn;
#endif
};

#endif