#include <thread>

#include "app.hh"
#include "timer.hh"
#include "mysimulator.hh"

static void userInit(brown::Simulator& sim) {
    // your amazing code
}

static void userLoop(brown::Simulator& sim) {
    // your amazing code
}

static void timxCallback(brown::Timer& tim, brown::Simulator& sim) {
    // your amazing code
}

static void timyCallback(brown::Timer& tim, brown::Simulator& sim) {
    // your amazing code
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
