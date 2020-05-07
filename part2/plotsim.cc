#include <thread>
#include <stack>

#include "app.hh"
#include "timer.hh"
#include "mysimulator.hh"

#include "ri.hh"

static int r1step = 0;
static int r2step = 0;
static bool isxmain = true;
static float tg = 0;
static std::stack<std::int64_t> vt1s;
static std::stack<std::int64_t> vt2s;

std::int64_t linearSpeedCurve(brown::Simulator& sim, std::int64_t tt) {
    static std::int64_t r2 = sim.CLKFRQ * sim.CLKFRQ;
    static std::int64_t rv0 = sim.CLKFRQ * sim.MINVEL;
    static std::int64_t a = sim.MAXACC*4/5;
    static std::int64_t lt = sim.MAXT*2; // low speed period

    std::int64_t vt = 0;
    if (tt < lt) {
        vt = sim.MAXT;
    } else {
        vt = r2/(rv0 + a*(tt-lt));
    }
    return vt;
}

static void moveLinear(brown::MySimulator& sim, int dx, int dy, bool isDown) {
    if (dx == 0 && dy == 0) {
        return;
    }

    sim.setDirx(dx > 0);
    sim.setDiry(dy > 0);
    sim.setDown(isDown);
    dx = std::abs(dx);
    dy = std::abs(dy);

    isxmain = (dx > dy);
    int d1px = (isxmain ? dx : dy);
    int d2px = (isxmain ? dy : dx);
    bool d2is0 = (d2px == 0);

    r1step = d1px * sim.PULPERPIX;
    r2step = d2px * sim.PULPERPIX;
    tg = d2is0? -1 : static_cast<float>(r1step)/r2step;

    // start timers
    auto& tim1 = isxmain ? sim.getTimx() : sim.getTimy();
    auto& tim2 = isxmain ? sim.getTimy() : sim.getTimx();

    tim1.setPeriod(0);
    tim1.start();
    if (!d2is0) {
        tim2.setPeriod(0);
        tim2.start();
    }
}

static void userInit(brown::Simulator& sim) {
    // empty
}

static void userLoop(brown::Simulator& sim) {
    auto* psim = dynamic_cast<brown::MySimulator*>(&sim);
    if (!psim) {return;}
    brown::MySimulator& msim = *psim;

    // return if still drawing
    if (r1step > 0 || r2step > 0) {
        return;
    }

    // drawing finished, do nothing
    static constexpr std::size_t ptNum = sizeof(state)/sizeof(state[0]);
    static std::size_t ptIdx = 0;
    if (ptIdx >= ptNum) {
        msim.delay(0);
        return;
    }

    // delay between lines
    if (ptIdx > 0) {
        msim.delay(sim.MAXT);
    }

    // target location [px]
    static int scale = 8;
    int x = state[ptIdx][1]/scale;
    int y = state[ptIdx][2]/scale;
    bool isDown = static_cast<bool>(state[ptIdx][0]);

    // relative linear motion
    static int locs[2]{0, 0};
    int dx = x - locs[0];
    int dy = y - locs[1];
    moveLinear(msim, dx, dy, isDown);

    // update meta
    locs[0] = x; locs[1] = y;
    ptIdx++;
}

static void timCallback(brown::Timer& tim, brown::Simulator& sim,
                        std::int64_t& t, bool ismain) {
    std::stack<std::int64_t>& vts = ismain ? vt1s : vt2s;
    int& rstep = ismain? r1step : r2step;

    if (--rstep <= 0) {
        tim.stop();
        t = 0; vts.empty();
        return;
    }

    // set next speed interval
    std::int64_t vt = 0;
    if (vts.size() >= rstep) { // decelerate
        vt = vts.top();
        vts.pop();
    } else if (vts.empty() || (vts.top() > sim.MINT)) { // accelerate
        vt = linearSpeedCurve(sim, t);
        if (vt < sim.MINT) { // speeding
            vt = sim.MINT;
        }
        vts.push(vt);
    } else { // hold
        vt = sim.MINT;
    }

    // adjust speed on secondary axis
    if (!ismain) {
        vt *= tg;
    }
    tim.setPeriod(vt);
    t += vt;
}

static void timxCallback(brown::Timer& tim, brown::Simulator& sim) {
    static std::int64_t t = 0;
    timCallback(tim, sim, t, isxmain);
}

static void timyCallback(brown::Timer& tim, brown::Simulator& sim) {
    static std::int64_t t = 0;
    timCallback(tim, sim, t, !isxmain);
}

int main(int argc, const char * argv[]) {
    brown::Timer timx(timxCallback);
    brown::Timer timy(timyCallback);
    brown::MySimulator sim(timx, timy);
    brown::App app(sim, userInit, userLoop);
    std::thread logicThread(&brown::App::runLogic, &app);
    int retCode = app.runGraphics();
    logicThread.join();
    return retCode;
}
