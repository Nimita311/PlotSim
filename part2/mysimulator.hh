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

    void delay(std::int64_t tt);

    Timer& getTimx() const {return timx;}
    Timer& getTimy() const {return timy;}

    void setDirx(bool dirx) {this->dirx = dirx;}
    void setDiry(bool diry) {this->diry = diry;}
    void setDown(bool isDown) {this->isDown = isDown;}

private:
    Timer& timx;
    Timer& timy;

    std::int64_t clk = 0;

    // timer counter
    std::int64_t cx = 0;
    std::int64_t cy = 0;

    // pen control
    bool dirx = true;
    bool diry = true;
    bool isDown = false;
};

} // namespace brown

#endif // _MYSIMULATOR_HH_
