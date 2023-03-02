#include <WorkoutTimer.h>
#include <Arduino.h>

WorkoutTimer::WorkoutTimer() {
    reset();
}

void WorkoutTimer::init(uint64_t now, TimerFrame& frame) {
    reset();
}

void WorkoutTimer::setCountdown(uint8_t seconds) {
    if ((seconds >= 0) && (seconds <= MAX_COUNTDOWN_SECONDS)) {
        _countdown = seconds;
    }
}

void WorkoutTimer::start(uint64_t now) {
    switch (_state)
    {
        case TIMER_STATE_STANDBY:
        case TIMER_STATE_COMPLETE:
            // in standby, transition to run
            _start = now;
            _pause = 0;
            _state = TIMER_STATE_RUN;
            break;
        
        case TIMER_STATE_PAUSE:
            // in paused state, transition to run
            _start = now - (_pause - _start);
            _pause = 0;
            _state = TIMER_STATE_RUN;

        default:
            // do nothing for now
            break;
    }
}

void WorkoutTimer::pause(uint64_t now) {
    if (_state == TIMER_STATE_RUN) {
        _pause = now;
        _state = TIMER_STATE_PAUSE;
    } else {
        // nothing to do
    }
}

void WorkoutTimer::reset() {
    _state = TIMER_STATE_STANDBY;
    _mode = TIMER_MODE_UNSET;
    _start = 0;
    _pause = 0;
    _intervals = 0;
    _totalSeconds = 0;

    // countdown can remain
}

void WorkoutTimer::setEmom(uint8_t intervals, uint32_t intervalSeconds) {
    // whenever mode is changed, state is reset to standby
    reset();
    
    _mode = TIMER_MODE_EMOM;
    _intervals = intervals;
    _totalSeconds = intervalSeconds;
}

void WorkoutTimer::setUp(uint32_t totalSeconds) {
    // whenever mode is changed, state is reset to standby
    reset();

    _mode = TIMER_MODE_UP;
    _totalSeconds = totalSeconds;
    _intervals = 1;
}

void WorkoutTimer::setDown(uint32_t totalSeconds) {
    // whenever mode is changed, state is reset to standby
    reset();
    
    _mode = TIMER_MODE_DOWN;
    _totalSeconds = totalSeconds;
    _intervals = 1;
}

void secondsToTime(uint32_t seconds, uint8_t time[]) {
    if (seconds == 0) {
        time[TIME_HOURS] = 0;
        time[TIME_MINUTES] = 0;
        time[TIME_SECONDS] = 0;
        return;
    }

    time[TIME_HOURS] = seconds / 3600;
    time[TIME_MINUTES] = (seconds % 3600) / 60;
    time[TIME_SECONDS] = (seconds % 3600) % 60;
}

void millisecondsToTime(uint64_t milliseconds, uint8_t time[]) {
    if (milliseconds == 0) {
        time[TIME_HOURS] = 0;
        time[TIME_MINUTES] = 0;
        time[TIME_SECONDS] = 0;
        return;
    }
    time[TIME_HOURS] = milliseconds / 3600000;
    time[TIME_MINUTES] = (milliseconds % 3600000) / 60000;
    time[TIME_SECONDS] = ((milliseconds % 3600000) % 60000) / 1000;
}

uint8_t WorkoutTimer::determineInterval(uint64_t milliseconds) {
    // determine consumed time
    uint64_t consumed = milliseconds - _start;
    return (consumed / 1000) / _totalSeconds;
}

bool WorkoutTimer::isRunComplete(uint64_t at) {
    // what milliseconds would "complete" be?
    return (at > (_start + (_intervals * _totalSeconds * 1000)));
}

void WorkoutTimer::determineDisplay(uint64_t milliseconds, uint8_t time[]) {
    if (_start >= milliseconds) {
        secondsToTime(0, time); // zero it out
        return;
    }

    // determine consumed time
    uint64_t consumed = milliseconds - _start;
    uint64_t intervalMilliseconds = (_totalSeconds * 1000);
    uint8_t currentInterval = determineInterval(milliseconds);

    // now decide out to display it
    switch (_mode)
    {
        case TIMER_MODE_UP:
            // consumed time IS the display time
            millisecondsToTime(consumed, time);
            break;

        case TIMER_MODE_DOWN:
            // display time is total milliseconds - consumed
            millisecondsToTime((_totalSeconds * 1000) - consumed, time);
            break;

        case TIMER_MODE_EMOM:
            // display time is remainder of interval time
            millisecondsToTime(
                intervalMilliseconds - (consumed - (currentInterval * intervalMilliseconds)),
                time);
        default:
            break;
    }
}

void WorkoutTimer::advance(uint64_t now, TimerFrame& frame) {
    // this is where all the brains are

    // take the timer state and "now" to determine where we should be
    //   and what the frame should contain

    // frame always contains the current state and mode
    frame.state = _state;
    frame.mode = _mode;

    // additional pass the settings we would like to display
    frame.intervals = _intervals;
    frame.countdown = _countdown;
    secondsToTime(_totalSeconds, frame.intervalTime);

    // now fill out the rest based on state
    switch (_state)
    {
        case TIMER_STATE_STANDBY:
            // in standby, we display what will occur after start
            frame.interval = 1; 
            secondsToTime(_totalSeconds, frame.displayTime);
            break;

        case TIMER_STATE_PAUSE:
            // in paused state, we treat _pause as now and freeze
            frame.interval = determineInterval(_pause);
            determineDisplay(_pause, frame.displayTime);
            break;

        case TIMER_STATE_RUN:
            // check if we are complete, 
            if (!isRunComplete(now)) {
                // in run, we display is ocurring
                frame.interval = determineInterval(now) + 1;
                determineDisplay(now, frame.displayTime);
                break;
            } else {
                _state = TIMER_STATE_COMPLETE;
                Serial.println("Timer run is complete");
            }

            // else fall through to complete

        case TIMER_STATE_COMPLETE:
            // in complete, we display what has occurred
            frame.interval = _intervals; // we are on the last interval

            if (_mode == TIMER_MODE_UP) {
                secondsToTime(_totalSeconds, frame.displayTime); // display the total time
            } else {
                millisecondsToTime(0, frame.displayTime); // otherwise we display zero
            }
            break;
    }
}