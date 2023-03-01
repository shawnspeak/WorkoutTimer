#ifndef TimerDisplay_h
#define TimerDisplay_h

#include <WorkoutTimer.h>
#include <LiquidCrystal.h>

class TimerDisplay {
    public:
        TimerDisplay();

        void init();

        void update(TimerFrame& frame);

    private:
       LiquidCrystal* _lcd;
};

#endif