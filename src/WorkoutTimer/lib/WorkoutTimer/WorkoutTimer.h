#ifndef WorkoutTimer_h
#define WorkoutTimer_h

#include <inttypes.h>

// modes
#define TIMER_MODE_UNSET 0x00
#define TIMER_MODE_CLOCK 0x01
#define TIMER_MODE_EMOM 0x02
#define TIMER_MODE_DOWN 0x03
#define TIMER_MODE_UP 0x04

// state
#define TIMER_STATE_STANDBY 0x01
#define TIMER_STATE_RUN 0x02
#define TIMER_STATE_PAUSE 0x03
#define TIMER_STATE_COMPLETE 0x04

#define DEFAULT_COUNTDOWN_SECONDS 10
#define MAX_COUNTDOWN_SECONDS 100

#define TIME_HOURS 0
#define TIME_MINUTES 1
#define TIME_SECONDS 2

struct TimerFrame {
    uint8_t mode; // timer mode
    uint8_t state; // timer state

    uint8_t displayTime[3]; // time to display
    uint8_t interval; // current interval

    uint8_t countdown; // countdown
    uint8_t intervals; // number of intervals
    uint8_t intervalTime[3]; // interval time
};

class WorkoutTimer {
    public:
        WorkoutTimer();

        void init(uint64_t now, TimerFrame& frame);

        // start the timer
        void start(uint64_t now);
        void pause(uint64_t now);

        void setCountdown(uint8_t seconds);

        // set timer mode
        void setEmom(uint8_t intervals, uint32_t intervalSeconds);
        void setDown(uint32_t totalSeconds);
        void setUp(uint32_t totalSeconds);

        // determine where the frame should be based on a given time
        void advance(uint64_t now, TimerFrame& frame);

    private:
        void reset();
        bool isRunComplete(uint64_t at);
        void determineDisplay(uint64_t milliseconds, uint8_t time[]);
        uint8_t determineInterval(uint64_t milliseconds);

        // current timer mode
        uint8_t _mode;

        // current timer state
        uint8_t _state;

        // time of start
        uint64_t _start;

        // time of pause
        uint64_t _pause;

        // countdown setting
        uint8_t _countdown;

        // total (interval) time
        uint32_t _totalSeconds;

        // total intervals
        uint8_t _intervals;
};

#endif