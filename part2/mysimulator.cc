#include <cstdlib>
#include <algorithm>

#include "mysimulator.hh"

namespace brown {

void MySimulator::hardwareLoop() {
    bool xr = timx.running();
    bool yr = timy.running();
    if (!xr && !yr) {return;}
    std::int64_t tx = static_cast<std::int64_t>(timx.t()) - cx;
    std::int64_t ty = static_cast<std::int64_t>(timy.t()) - cy;
    std::int64_t dt = (xr && yr) ? std::min(tx, ty) : (xr ? tx : ty);
    if (xr) {cx += dt;}
    if (yr) {cy += dt;}
    clk += dt;

    if (xr && (cx >= timx.t())) {
        setpin(clk, true, false, dirx, diry, isDown);
        timx.callback(*this);
        cx = 0;
    }
    if (yr && (cy >= timy.t())) {
        setpin(clk, false, true, dirx, diry, isDown);
        timy.callback(*this);
        cy = 0;
    }
}

std::int64_t MySimulator::getTick() {
    return clk;
}

void MySimulator::delay(std::int64_t tt) {
    clk += tt;
    setpin(clk, false, false, dirx, diry, isDown);
}

} // namespace brown
