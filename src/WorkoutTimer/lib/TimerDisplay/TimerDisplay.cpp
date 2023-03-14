#include <Arduino.h>

#include <LiquidCrystal.h>
#include "TimerDisplay.h"

TimerDisplay::TimerDisplay() {
    _lcd = new LiquidCrystal(D1, D2, D4, D5, D6, D7);
}

void TimerDisplay::init() {
    // set up the LCD's number of columns and rows:
	_lcd->begin(16, 2);

	// Clears the LCD screen
	_lcd->clear();
}

uint8_t lastMode = 0;
uint8_t lastState = 0;
uint8_t lastInterval = 0;

void TimerDisplay::update(TimerFrame& frame) {

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

    lastMode = frame.mode;
    lastState = frame.state;
    lastInterval = frame.interval;
}