#ifndef _MYSIMULATOR_HH_
#define _MYSIMULATOR_HH_

#include <cstdint>

#include "timer.hh"
#include "simulator.hh"

namespace brown {

class MySimulator: public Simulator {
public:
    MySimulator(Timer& timx, Timer& timy):
        Simulator(), timx(timx), timy(timy) {};
    void hardwareLoop();
    std::int64_t getTick();

private:
    Timer& timx;
    Timer& timy;
};

} // namespace brown

#endif // _MYSIMULATOR_HH_
