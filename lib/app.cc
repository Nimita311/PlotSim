#include <iostream>

#include "app.hh"
#include "config.hh"

namespace brown {

App::App(Simulator& sim): sim(sim), dd(), xres(XRES_), yres(YRES_) {
    sim.linkApp(this);
    keystatus = new char[256];
}

App::App(Simulator& sim, void (*userInit)(Simulator&), void (*userLoop)(Simulator&)):
    App(sim) {
    this->userInit = userInit;
    this->userLoop = userLoop;
}

App::~App() {
    delete[] keystatus;
}

int App::appInit() {
    /* Init SDL */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        return -1;
    }

    /* Create window and renderer */
    window = SDL_CreateWindow(WINDOW_NAME_, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xres, yres, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        SDL_Quit();
        return -1;
    }
    renderer = SDL_CreateRenderer(window,-1,0);
    if (renderer == nullptr) {
        SDL_Quit();
        SDL_DestroyWindow(window);
        return -1;
    }

    /* Create VRAM texture */
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, xres, yres);

    return 0;
}

void App::appExit() {
    if (renderer) {SDL_DestroyRenderer(renderer);}
    if (window) {SDL_DestroyWindow(window);}
    SDL_Quit();
}

void App::appEventHandle() {
    SDL_Event event;
    dmousx = 0; dmousy = 0;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {isBreak = 1;}
                if (event.key.keysym.sym == SDLK_LEFT ) {keystatus[0xcb] = 1;}
                if (event.key.keysym.sym == SDLK_RIGHT) {keystatus[0xcd] = 1;}
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {isBreak = 1;}
                if (event.key.keysym.sym == SDLK_LEFT ) {keystatus[0xcb] = 0;}
                if (event.key.keysym.sym == SDLK_RIGHT) {keystatus[0xcd] = 0;}
                break;
            case SDL_MOUSEMOTION:
                mousx = event.motion.x; dmousx += event.motion.xrel;
                mousy = event.motion.y; dmousy += event.motion.yrel;
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:   bstatus |= 1<<0; break;
                    case SDL_BUTTON_RIGHT:  bstatus |= 1<<1; break;
                    case SDL_BUTTON_MIDDLE: bstatus |= 1<<2; break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:   bstatus &= ~(1<<0); break;
                    case SDL_BUTTON_RIGHT:  bstatus &= ~(1<<1); break;
                    case SDL_BUTTON_MIDDLE: bstatus &= ~(1<<2); break;
                }
                break;
            case SDL_QUIT:
                isBreak = true; break;
        }
    }
}

int App::beginDraw() {
    void* fptr = nullptr;
    int pitch = 0;
    int retCode = SDL_LockTexture(texture, nullptr, &fptr, &pitch);
    dd.set(reinterpret_cast<std::uintptr_t>(fptr), pitch, xres, yres);
    return retCode;
}

void App::endDraw() {
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void App::delay(std::uint32_t ms) {
    SDL_Delay(ms);
}

std::uint32_t App::getTime() {
    return SDL_GetTicks();
}

void App::getMouseStatus(int& mousx, int& mousy, int& dmousx, int& dmousy, int& bstatus) const {
    mousx = this->mousx;
    mousy = this->mousy;
    dmousx = this->dmousx;
    dmousy = this->dmousy;
    bstatus = this->bstatus;
}

const char* App::getKeyStatus() const {
    return keystatus;
}

int App::runGraphics() {
    if (appInit() != 0) {return -1;}
    while (!isBreak) {
        appEventHandle();
        if (beginDraw() == 0) {
            sim.renderLoop(dd);
        }
        endDraw();
        SDL_Delay(30);
    }
    appExit();
    return 0;
}

int App::runLogic() {
    if (userInit) {userInit(sim);}
    while (!isBreak) {
        sim.hardwareLoop();
        if (userLoop) {userLoop(sim);}
    }
    return 0;
}

} // namespace brown
