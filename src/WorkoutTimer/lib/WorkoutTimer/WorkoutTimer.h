#ifndef WorkoutTimer_h
#define WorkoutTimer_h

#include <inttypes.h>

// modes
#define TIMER_MODE_UNSET 0x00;
#define TIMER_MODE_CLOCK 0x01;
#define TIMER_MODE_EMOM 0x02;
#define TIMER_MODE_AMRAP 0x03;
#define TIMER_MODE_FORTIME 0x04;
#define TIMER_MODE_TABATA 0x05;

// state
#define TIMER_STATE_STANDBY 0x01;
#define TIMER_STATE_RUN 0x02;
#define TIMER_STATE_PAUSE 0x03;

struct TimerFrame {

};

class WorkoutTimer {
    public:
        WorkoutTimer();

        void init(uint64_t now, TimerFrame& frame);

        void advance(uint64_t now, TimerFrame& frame);

    private:
       uint8_t _mode;
       uint8_t _state;
       uint64_t _start;
       uint64_t _last;
};

#endif