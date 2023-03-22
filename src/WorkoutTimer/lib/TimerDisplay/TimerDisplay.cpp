#include <Arduino.h>

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <stdlib.h>
#include "pico/stdlib.h"
#include "okcolor.hpp"
#include <SafeString.h>
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
    _graphics->set_font(&fontTimer);
    _galactic_unicorn->set_brightness(0.5F);

    _galactic_unicorn->set_volume(1.0F);


    // setup beep channel
    pimoroni::AudioChannel beepChannel = _galactic_unicorn->synth_channel(0);
    beepChannel.waveforms   = pimoroni::Waveform::SINE | pimoroni::Waveform::SQUARE;
    beepChannel.attack_ms   = 38;
    beepChannel.decay_ms    = 300;
    beepChannel.sustain     = UINT16_MAX;
    beepChannel.release_ms  = 168;
    beepChannel.volume      = UINT16_MAX;


#endif

}

uint8_t lastMode = 0;
uint8_t lastState = 0;
uint8_t lastInterval = 0;

int32_t y = 0;
float s = 1.0F;
uint8_t f = 0;

unsigned long lastA = 0;
unsigned long lastB = 0;
unsigned long lastC = 0;
unsigned long lastD = 0;

unsigned long toneTill = 0;


#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
createSafeString(displayBuffer, 50);
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
        case TIMER_STATE_PAUSE: 
            _lcd->print("PAUSE");
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
    displayBuffer.clear();
    _graphics->set_pen(0, 0, 0);
    _graphics->clear();

    if (frame.countdown > 0) {
        displayBuffer.printf("%u", frame.countdown);
    } else if (frame.countdown == 0) {

        // if (toneTill == 0) {
        //     toneTill = millis() + 2000;
        //     Serial.println("tone start");

        //     _galactic_unicorn->synth_channel(0).frequency  = 265;
        //     _galactic_unicorn->synth_channel(0).trigger_attack();

        //     // start the tone
        //     _galactic_unicorn->play_synth();
        // }


        displayBuffer = "GO!";
    } else {
        if (frame.displayTime[TIME_HOURS] > 0) {
            //displayBuffer.printf("%02u:%02u:%02u", frame.displayTime[TIME_HOURS], frame.displayTime[TIME_MINUTES], frame.displayTime[TIME_SECONDS]);
            displayBuffer.printf("%u:%u:%u", frame.displayTime[TIME_HOURS], frame.displayTime[TIME_MINUTES], frame.displayTime[TIME_SECONDS]);
        } else {
            //displayBuffer.printf("%02u:%02u", frame.displayTime[TIME_MINUTES], frame.displayTime[TIME_SECONDS]);
            displayBuffer.printf("%02u:%02u", frame.displayTime[TIME_MINUTES], frame.displayTime[TIME_SECONDS]);
        }
    }
    int width = _graphics->measure_text(displayBuffer.c_str(), s);

    // ok_color::HSL hsl{scroll / 100.0f, 1.0f, 0.5f};
    // ok_color::RGB rgb = ok_color::okhsl_to_srgb(hsl);
    //_graphics->set_pen(rgb.r * 255, rgb.g * 255, rgb.b * 255);
    _graphics->set_pen(201, 201, 201);

    if ((frame.mode == TIMER_MODE_EMOM) && (frame.countdown == -1)) {
        // park the time on the right of the display
        _graphics->text(displayBuffer.c_str(), pimoroni::Point(53 - width, y), -1, s);
    } else {
        _graphics->text(displayBuffer.c_str(), pimoroni::Point(26 - (width / 2), y), -1, s);
    }

    _galactic_unicorn->update(_graphics);

    // clear tone
    // if ((toneTill != 0) && (millis() > toneTill)) {
    //     toneTill = 0;
    //     _galactic_unicorn->stop_playing();
    //     Serial.println("tone stop");
    // }

    // if (_galactic_unicorn->is_pressed(pimoroni::GalacticUnicorn::SWITCH_A)) {
    //     if (millis() > lastA + 1000) {
    //         s += 0.05F;
    //         lastA = millis();

    //         Serial.printf("Scale change: %f\n", s);
    //     }
    // }

    // if (_galactic_unicorn->is_pressed(pimoroni::GalacticUnicorn::SWITCH_B)) {
    //     if (millis() > lastB + 1000) {
    //         s -= 0.05F;
    //         lastB = millis();

    //         Serial.printf("Scale change: %f \n", s);
    //     }
    // }

    // if (_galactic_unicorn->is_pressed(pimoroni::GalacticUnicorn::SWITCH_C)) {
    //     if (millis() > lastC + 1000) {
    //         y = (y >= 7)?-4:y+1;
    //         lastC = millis();

    //         Serial.printf("Y change: %u \n", y);
    //     }
    // }

    // if (_galactic_unicorn->is_pressed(pimoroni::GalacticUnicorn::SWITCH_D)) {
    //     if (millis() > lastD + 1000) {
    //         f = (f >= 7)?0:f+1;
    //         lastD = millis();

    //         switch (f)
    //         {
    //             case 0: _graphics->set_font("sans"); break;
    //             case 1: _graphics->set_font("bitmap6"); break;
    //             case 2: _graphics->set_font("bitmap8"); break;
    //             case 3: _graphics->set_font("bitmap14_outline"); break;
    //             case 4: _graphics->set_font("gothic"); break;
    //             case 5: _graphics->set_font("gothic"); break;
    //             case 6: _graphics->set_font("serif_italic"); break;
    //             case 7: _graphics->set_font("serif"); break;
    //             default:
    //                 break;
    //         }

    //         Serial.printf("Font change: %u \n", f);
    //     }
    // }

#endif

    lastMode = frame.mode;
    lastState = frame.state;
    lastInterval = frame.interval;
}