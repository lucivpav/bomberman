#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <chrono>

class Countdown
{
public:
    Countdown(int milliseconds);
    bool expired(int milliseconds = -1);
private:

    std::chrono::milliseconds mPrev;
    int mPeriod;
};

#endif
