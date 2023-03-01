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

void TimerDisplay::update(TimerFrame& frame) {
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    _lcd->setCursor(0, 1);

    // print the number of seconds since reset:
    _lcd->print(millis() / 1000);
}