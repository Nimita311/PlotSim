#include <thread>

#include "app.hh"
#include "mysimulator.hh"

int main(int argc, const char * argv[]) {
    brown::MySimulator sim;
    brown::App app(sim);
    std::thread logicThread(&brown::App::runLogic, &app);
    int retCode = app.runGraphics();
    logicThread.join();
    return retCode;
}
