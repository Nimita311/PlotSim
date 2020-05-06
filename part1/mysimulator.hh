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
private:
    // scale down factor of the given way points
    const int scale = 8;

    // current clock
    std::int64_t clk = 0;
    // current location [px]
    int locs[2]{0, 0};
    // current index of the point being drawn
    std::size_t ptIdx = 0;

    void moveLinear(int dx, int dy, bool isDown);
    std::int64_t linearSpeedCurve(std::int64_t tt);

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
