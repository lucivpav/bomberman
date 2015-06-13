#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <chrono>

/**
 * @brief The Countdown class represents a timer that is set to expire
 * in a specific period of time.
 */
class Countdown
{
public:
    /**
     * @brief Initializes the timer to a given expiration period.
     * @param milliseconds The amount of milliseconds before the timer expires.
     */
    Countdown(int milliseconds);

    /**
     * @brief Returns whether the timer has expired. This method can be used
     * to reuse the timer again, setting it to an arbitrary expiry period.
     * @param milliseconds The amount of milliseconds before the timer expires
     * again.
     * @return true if the timer has expired, false otherwise.
     */
    bool expired(int milliseconds = -1);
private:

    std::chrono::milliseconds mPrev;
    int mPeriod;
};

#endif
