#include <cstdlib>
#include <stack>
#include <limits>

#include "mysimulator.hh"
#include "ri.hh"

namespace brown {

void MySimulator::hardwareLoop() {
    // drawing finished, do nothing
    static constexpr int ptNum = sizeof(state)/sizeof(state[0]);
    if (ptIdx >= ptNum) {
        setpin(clk, false, false, false, false, false);
        return;
    }

    // target location [px]
    int x = state[ptIdx][1]/scale;
    int y = state[ptIdx][2]/scale;
    bool isDown = static_cast<bool>(state[ptIdx][0]);

    // relative linear motion
    int dx = x - locs[0];
    int dy = y - locs[1];
    moveLinear(dx, dy, isDown);

    // update meta
    locs[0] = x; locs[1] = y;
    ptIdx++;
}

void MySimulator::moveLinear(int dx, int dy, bool isDown) {
    if (dx == 0 && dy == 0) {
        return;
    }

    // direction and displacement
    bool dirx = (dx > 0);
    bool diry = (dy > 0);
    dx = std::abs(dx);
    dy = std::abs(dy);

    // determine primary/long and secondary/short axis
    bool isxmain = (dx > dy);
    int d1px = (isxmain ? dx : dy);
    int d2px = (isxmain ? dy : dx);
    bool d2is0 = (d2px == 0);

    // target steps and ratio
    int d1step = d1px * PULPERPIX;
    int d2step = d2px * PULPERPIX;
    float tg = d2is0? -1 : static_cast<float>(d1step)/d2step;

    // step keeping
    int s1 = 0;
    int s2 = 0;

    // time keeping
    std::int64_t startTick = clk;
    std::int64_t t1 = 0;
    std::int64_t t2 = d2is0 ? std::numeric_limits<std::int64_t>::max() : 0;

    // speed history
    std::stack<std::int64_t> vt1s;
    std::stack<std::int64_t> vt2s;

    while ((s1 < d1step) || (s2 < d2step)) {
        // determine driving axis
        bool ismain = (t1 <= t2);
        int& s = ismain ? s1 : s2;
        int dstep = ismain ? d1step : d2step;
        bool isx = (ismain == isxmain);
        std::int64_t& t = ismain ? t1 : t2;
        std::stack<std::int64_t>& vts = ismain ? vt1s : vt2s;

        // step
        clk = startTick + t;
        setpin(clk, isx, !isx, dirx, diry, isDown);
        if (++s >= dstep) {
            t = std::numeric_limits<std::int64_t>::max();
            continue;
        }

        // set next step interval
        std::int64_t vt = 0;
        if (vts.size() >= (dstep - s)) { // decelerate
            vt = vts.top();
            vts.pop();
        } else if (vts.empty() || (vts.top() > MINT)) { // accelerate
            vt = linearSpeedCurve(t);
            if (vt < MINT) { // speeding
                vt = MINT;
            }
            vts.push(vt);
        } else { // hold
            vt = MINT;
        }

        // adjust speed on secondary axis
        if (!ismain) {
            vt *= tg;
        }
        t += vt;
    }

    // delay, a time gap between line drawings
    clk += MAXT;
    setpin(clk, false, false, dirx, diry, isDown);
}

std::int64_t MySimulator::linearSpeedCurve(std::int64_t tt) {
    static std::int64_t r2 = CLKFRQ * CLKFRQ;
    static std::int64_t rv0 = CLKFRQ * MINVEL;
    static std::int64_t a = MAXACC*4/5;
    static std::int64_t lt = MAXT*2; // low speed period

    std::int64_t vt = 0;
    if (tt < lt) {
        vt = MAXT;
    } else {
        vt = r2/(rv0 + a*(tt-lt));
    }
    return vt;
}

} // namespace brown
