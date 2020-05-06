/**
 * @brief    `Timer` class.
 * @author   Haoze Zhang
 *
 * Distributed under MIT License
 * Copyright (c) 2020 Haoze Zhang | Brown Engineering
 */

#ifndef _TIMER_HH_
#define _TIMER_HH_

#include "simulator.hh"

namespace brown {

/**
 * @brief A microncontroller timer data structure.
 */
class Timer {
public:
    Timer() {}
    Timer(void (*cb)(Timer&,Simulator&)) {this->cb = cb;}

    void setCallback(void (*cb)(Timer&,Simulator&)) {this->cb = cb;}
    void setPrescaler(uint32_t prescaler) {this->prescaler = prescaler;}
    void setPeriod(uint32_t period) {this->period = period;}
    uint32_t getPrescaler() const {return prescaler;}
    uint32_t getPeriod() const {return period;}

    void start() {this->isRunning = true;}
    void stop() {this->isRunning = false;}
    bool running() const {return isRunning;}
    void callback(Simulator& sim) {if (cb) {cb(*this, sim);}}
    std::int64_t t() const {return (prescaler+1)*period;}

private:
    void (*cb)(Timer&, Simulator&) = nullptr;
    bool isRunning = false;
    uint32_t prescaler = 0;
    uint32_t period = 0;
}; // class Timer

} // namespace brown

#endif // _TIMER_HH_
