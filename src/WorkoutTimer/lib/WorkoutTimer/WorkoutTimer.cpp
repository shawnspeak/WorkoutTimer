#include <WorkoutTimer.h>

WorkoutTimer::WorkoutTimer() {
    _mode = TIMER_MODE_UNSET;
    _state = TIMER_STATE_STANDBY;
}

void WorkoutTimer::init(uint64_t now, TimerFrame& frame) {
    _mode = TIMER_MODE_CLOCK;
    _start = 0;
}


void WorkoutTimer::advance(uint64_t now, TimerFrame& frame) {
    _last = now;
}