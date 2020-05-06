/**
 * @brief    `App` class.
 * @author   Haoze Zhang, Ken Silverman
 *
 * Distributed under MIT License
 * Copyright (c) 2020 Haoze Zhang | Brown Engineering
 */

#ifndef _APP_HH_
#define _APP_HH_

#include <SDL.h>

// forward declaration
namespace brown {class Simulator;}
#include "tile.hh"
#include "simulator.hh"

namespace brown {

/**
 * @brief `App` class handles interactions with the application.
 */
class App {
public:
    App(Simulator& sim);
    App(Simulator& sim, void (*userInit)(Simulator&), void (*userLoop)(Simulator&));
    ~App();

    void setUserInit(void (*userInit)(Simulator&)) {
        this->userInit = userInit;
    }
    void setUserLoop(void (*userLoop)(Simulator&)) {
        this->userLoop = userLoop;
    }

    // Utilities
    void delay(std::uint32_t ms);
    void getMouseStatus(int& mousx, int& mousy, int& dmousx, int& dmousy, int& bstatus) const;
    const char* getKeyStatus() const;
    std::uint32_t getTime();

    // Simulations
    int runLogic();
    int runGraphics();

private:
    Simulator& sim;
    Tile dd;

    void (*userInit)(Simulator&) = nullptr;
    void (*userLoop)(Simulator&) = nullptr;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    // Window resolution
    int xres;
    int yres;

    // Break loop flag
    bool isBreak = false;

    // Mouse status
    int mousx = 0;
    int mousy = 0;
    int dmousx = 0;
    int dmousy = 0;
    int bstatus = 0;

    // Key status
    char* keystatus;

    // App flow
    int appInit();
    void appExit();
    void appEventHandle();

    // Draw control
    int beginDraw();
    void endDraw();

}; // class App

} // namespace brown

#endif // _APP_HH_
