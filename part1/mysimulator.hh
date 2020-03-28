#ifndef _MYSIMULATOR_HH_
#define _MYSIMULATOR_HH_

#include <cstdint>

#include "simulator.hh"

namespace brown {

/**
 * @brief Simulator subclass with direct hardware control.
 *
 * You have no further restrictions on the implementation of this class beyond
 * inheriting class `brown::Simulator`.
 */
class MySimulator: public Simulator {
public:
    MySimulator(): Simulator() {};

    /**
     * @brief Pulse generation logic for plotting.
     * @note You must override this pure virtual method.
     *
     * This method will be invoked repeatedly and indefinitely by the
     * simulator. You are recommended to handle one point at a time.
     * You can invoke `this->setpin` to generate pulses.
     */
    void hardwareLoop();
};

} // namespace brown

#endif // _MYSIMULATOR_HH_
