#ifndef _SIMULATOR_HH_
#define _SIMULATOR_HH_

#include <cstdint>
#include <cstddef>

// forward declaration
namespace brown {class App;}
#include "tile.hh"
#include "app.hh"

namespace brown {

class Simulator {
public:
    /**
     * @note Student implementation.
     */
    virtual void hardwareLoop() = 0;

    /**
     * @note Student implementation.
     */
    virtual std::int64_t getTick();

    const int BMPXRES;
    const int BMPYRES;
    const std::int64_t CLKFRQ;
    const int PULPERPIX;
    const int MAXACC;
    const int MAXVEL;
    const int MINVEL;
    const int MINPOS;
    const int MAXPOS;
    const int MAXT;
    const int MINT;
    const std::size_t FIFMAX;

    Simulator();
    ~Simulator();

    void linkApp(App* app) {this->app = app;}
    void renderLoop(Tile& texture);

protected:
    /**
     * @brief Generate a pulse at a specific clock cycle.
     * @param clk Simulator clock cycle.
     * @param pulx Step x axis.
     * @param puly Step y axis.
     * @param dirx Positive x direction.
     * @param diry Positive y direction.
     * @param down Pen down.
     * @note This is the only interface to interact with the emulated hardware.
     */
    void setpin(std::int64_t clk, bool pulx, bool puly,
                bool dirx, bool diry, bool down);

private:
    Tile drawing;
    App* app;

    std::int64_t pclk = -1;
    std::int64_t oclk[2]{-1, -1};
    std::int64_t ooclk[2]{-2, -2};

    int odir[2]{0, 0};
    bool odown = false;

    bool killax[2]{false, false};

    int cur[2]{0, 0};
    int acur[2]{0, 0};

    int fault = 0;
    std::int64_t faultclk = 0;

    std::int64_t* fifclk;
    int* fifcurx;
    int* fifcury;
    char* fifbit;
    std::size_t fifn = 0;
    std::size_t pulcount[2]{0,0};

    std::int64_t xoff = 0;
    std::int64_t zoom = 1000000;
}; // class Simulator

} // namespace brown

#endif // _SIMULATOR_HH_
