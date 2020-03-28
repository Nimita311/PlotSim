#include <cmath>
#include <cstring>
#include <iostream>

#include "simulator.hh"
#include "config.hh"


#if !defined(max)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

namespace brown {

Simulator::Simulator():
    BMPXRES(BMPXRES_), BMPYRES(BMPYRES_),
    CLKFRQ(CLKFRQ_), MAXACC(MAXACC_),
    MAXVEL(MAXVEL_), MINVEL(MINVEL_),
    MINPOS(MINPOS_), MAXPOS(MAXPOS_),
    MAXT(CLKFRQ_/MINVEL_), MINT(CLKFRQ_/MAXVEL_),
    FIFMAX(FIFMAX_),
    drawing(BMPXRES_, BMPYRES_, BACKGROUND_COLOR) {
    fifclk = new std::int64_t[FIFMAX];
    fifcurx = new int[FIFMAX];
    fifcury = new int[FIFMAX];
    fifbit = new char[FIFMAX];
}

Simulator::~Simulator() {
    delete[] fifclk;
    delete[] fifcurx;
    delete[] fifcury;
    delete[] fifbit;
}

std::int64_t Simulator::getTick() {
    return pclk;
}

void Simulator::setpin(std::int64_t clk, bool pulx, bool puly,
                       bool dirx, bool diry, bool down) {
    int x, y;
    bool pul[]{pulx, puly};
    bool dir[]{dirx, diry};

    /* Advance in time */
    if (clk < pclk || (clk==pclk && !pulx && !puly)) {
        app->delay(15);
        return;
    }
    // delay
    static std::uint32_t startTime = 0; // ms
    static std::uint64_t startClk = 0; // tick
    double delayTime = 0; // ms (for sub ms accuracy)
    if ((clk-startClk) > (CLKFRQ/10)) { // handle delay every 100 ms emulated time
        std::uint32_t elapsedTime = app->getTime() - startTime;
        delayTime += static_cast<double>((clk-startClk))*1000.0/TIME_SCALE/CLKFRQ;
        std::uint32_t delayTimeFloor = static_cast<std::uint32_t>(delayTime);
        if (delayTimeFloor > elapsedTime) {
            app->delay(delayTimeFloor-elapsedTime);
            delayTime -= delayTimeFloor;
        } else {
            delayTime = 0;
        }
        startClk = clk;
        startTime = app->getTime();
    }
    pclk = clk;

    /* Log data */
    if (fifn < FIFMAX) {
        fifclk[fifn] = clk;
        fifcurx[fifn] = acur[0];
        fifcury[fifn] = acur[1];
        fifbit[fifn] = pulx + (puly<<1) + (dirx<<2) + (diry<<3) + (down<<4);
        fifn++;
    }

    /* Draw previous pen location */
    x = (acur[0]>>6)+(BMPXRES>>1);
    y = (acur[1]>>6)+(BMPYRES>>1);
    if (((unsigned)x < (unsigned)BMPXRES)
    && ((unsigned)y < (unsigned)BMPYRES)) {
        if (down) {
            drawing.getpix(x, y) = DOWN_COLOR;
        }
        else if (drawing.getpix(x, y) == BACKGROUND_COLOR) {
            drawing.getpix(x, y) = UP_COLOR;
        }
    }

    /* Move pen and apply restriction*/
    for(int i = 0; i < 2; i++) {
        if (!pul[i]) {continue;}
#if ENABLE_RESTRICTION
        pulcount[i]++;
        std::int64_t t0 = clk - oclk[i];
        std::int64_t t1 = oclk[i] - ooclk[i];

        // Unlock axis after slowing down
        if (t0 >= MAXT) {killax[i] = false;}

        // Fault 2/3: zero clk between pulses
        if (t0 <= 0) {
            if (!fault) {fault = i+2; faultclk = clk;}
            killax[i] = 1; continue;
        }

        // Fault 4/5: change direction at high speed
        if (((t0 < MAXT) || (t1 < MAXT)) && (ooclk[i] >= 0) && (dir[i] != odir[i])) {
            if (!fault) {fault = i+4; faultclk = clk; }
            killax[i] = 1;
        }

        // Fault 6/7: speeding
        if ((t0 < MINT) && (oclk[i] >= 0)) {
            if (!fault) {fault = i+6; faultclk = clk;}
        }

        // Fault 8/9: over-acceleration
        if ((t0 <= MAXT) && (t1 <= MAXT) && (ooclk[i] >= 0)) {
            if (fabs((double)(t1-t0))*CLKFRQ*CLKFRQ*2.0 >= (double)(t0+t1)*t0*t1*MAXACC) {
                killax[i] = 1;
                if (!fault) {fault = i+8; faultclk = clk; }
            }
        }

        if (!killax[i]) {
            acur[i] = min(max(acur[i] + (dir[i]>0)*2-1,MINPOS),MAXPOS);
        }
#else
        acur[i] = min(max(acur[i] + (dir[i]>0)*2-1,MINPOS),MAXPOS);
#endif // ENABLE_RESTRICTION

        cur[i] = min(max(cur[i] + (dir[i]>0)*2-1,MINPOS),MAXPOS);

        ooclk[i] = oclk[i];
        oclk[i] = clk;
        odir[i] = dir[i];
    }
    odown = down;
}

void Simulator::renderLoop(Tile& dd) {
    App& app = *this->app;

    /* Copy drawing to texture */
    for(int y = min(dd.gety(),BMPYRES)-1; y >= 0; y--) {
        uint32_t* wptr = reinterpret_cast<uint32_t*>(dd.getp()*y + dd.getf());
        for(int x = min(dd.getx(),BMPXRES)-1; x >= 0; x--) {
            wptr[x] = drawing.getpix(x, y);
        }
    }

    /* Fill in the blank */
    dd.drawrectfill(BMPXRES, 0, dd.getx(), BMPYRES, BACKGROUND_COLOR);
    dd.drawrectfill(0, BMPYRES, dd.getx(), dd.gety(), BACKGROUND_COLOR);

    /* Draw pen location */
    if (!odown) {
        dd.drawcirc((acur[0]>>6)+(BMPXRES>>1), (acur[1]>>6)+(BMPYRES>>1), 20, UP_LOC_COLOR);
    } else {
        dd.drawcirc((acur[0]>>6)+(BMPXRES>>1), (acur[1]>>6)+(BMPYRES>>1), 20, DOWN_LOC_COLOR);
    }

    /* Show timing info */
    dd.print6x8(0,0,0xffffff,BACKGROUND_COLOR,"%.2f sec, fifn=%d",(double)pclk/CLKFRQ,fifn);

#if ENABLE_SCOPE
    /* Scope, timing plot */
    static int64_t rzoom;
    static double tim = 0.0, otim, dtim;
    static int i, dpx, dpy;

    // Get app status
    const char* keystatus = app.getKeyStatus();
    int mousx, mousy, dmousx, dmousy, bstatus;
    app.getMouseStatus(mousx, mousy, dmousx, dmousy, bstatus);

    // Pan left/right with arrow keys
    otim = tim;
    tim = static_cast<double>(app.getTime())*.001;
    dtim = tim-otim;
    xoff += (keystatus[0xcd]-keystatus[0xcb])*zoom*512*dtim;

    // Zoom and pan with mouse
    if (bstatus & 1<<1) { // right mouse botton down
        // pan
        xoff += dmousx * zoom;
        // zoom
        i = min(max(zoom+(zoom*dmousy)/100,100),10000000);
        xoff += (zoom-i)*mousx;
        zoom = i;
    }

    // Error message
    if (fault) {
        int x = (faultclk-xoff)/zoom;
        if ((fault>>1) == 1) dd.print6x8(x,YOFF-10,0xff8080,FONT_B_COLOR,"fault: %c div by 0",(fault&1)+'x');
        if ((fault>>1) == 2) dd.print6x8(x,YOFF-10,0xff8080,FONT_B_COLOR,"fault: %c dir change at hi veloc",(fault&1)+'x');
        if ((fault>>1) == 3) dd.print6x8(x,YOFF-10,0xff8080,FONT_B_COLOR,"fault: %c veloc too high",(fault&1)+'x');
        if ((fault>>1) == 4) dd.print6x8(x,YOFF-10,0xff8080,FONT_B_COLOR,"fault: %c accel too high",(fault&1)+'x');
        dd.drawline(x,YOFF,x,YOFF+200,0xff8080);
    }

    // Scope background
    int ox = (0-xoff)/zoom;
    int x  = (fifclk[fifn-1]-xoff)/zoom;
    for(i=0;i<5;i++) {
         dd.drawrectfill(ox,YOFF+i*40,x,YOFF+30+i*40,0x404040);
    }
    ox = 0; dpx = -1; dpy = -1;
    rzoom = (((1<<30)/zoom)<<2);

    // Scope data
     for(i=0;i<fifn;i++)
     {
         x = (((fifclk[i]-xoff)*rzoom)>>32); if (x < 0) continue;
         if ((x > dpx) && (fifbit[i]&1)) { dpx = x; dd.drawline(x,YOFF   ,x,YOFF+ 30,0xffffff); }
         if ((x > dpy) && (fifbit[i]&2)) { dpy = x; dd.drawline(x,YOFF+80,x,YOFF+110,0xffffff); }
         if (i > 0)
         {
             dd.drawline(ox,((fifbit[i-1]>>2)&1)*-30+YOFF+ 70,x,((fifbit[i-1]>>2)&1)*-30+YOFF+70,0xffffff);
             dd.drawline(x,((fifbit[i-1]>>2)&1)*-30+YOFF+ 70,x,((fifbit[i  ]>>2)&1)*-30+YOFF+70,0xffffff);

             dd.drawline(ox,((fifbit[i-1]>>3)&1)*-30+YOFF+150,x,((fifbit[i-1]>>3)&1)*-30+YOFF+150,0xffffff);
             dd.drawline( x,((fifbit[i-1]>>3)&1)*-30+YOFF+150,x,((fifbit[i  ]>>3)&1)*-30+YOFF+150,0xffffff);

             dd.drawline(ox,((fifbit[i-1]>>4)&1)*-30+YOFF+190,x,((fifbit[i-1]>>4)&1)*-30+YOFF+190,0xffffff);
             dd.drawline( x,((fifbit[i-1]>>4)&1)*-30+YOFF+190,x,((fifbit[i  ]>>4)&1)*-30+YOFF+190,0xffffff);
         }
         ox = x; if (x >= dd.getx()) break;
     }

    // Scope annotation
     for(i=0;i<fifn;i++)
     {
         if (fifclk[i] >= mousx*zoom + xoff)
         {
             int i0, i1;
             dd.drawcirc((fifcurx[i]>>6)+(BMPXRES>>1),(fifcury[i]>>6)+(BMPYRES>>1),5,
                (fifbit[i] & (1<<4))? DOWN_LOC_COLOR : UP_LOC_COLOR);

             for(i0=i-1;i0> 0    ;i0--) if (fifbit[i0]&1) break;
             for(i1=i  ;i1<fifn-1;i1++) if (fifbit[i1]&1) break;
             if (i0 >= 0)
             {
                 dd.drawline((fifclk[i0]-xoff)/zoom,YOFF+13,(fifclk[i1]-xoff)/zoom,YOFF+13,0x808080);
                 dd.print6x8(mousx+2,YOFF+ 5,FONT_F_COLOR,FONT_B_COLOR,"%d clk",fifclk[i1]-fifclk[i0]);
                 dd.print6x8(mousx+2,YOFF+15,FONT_F_COLOR,FONT_B_COLOR,"(%.1f Hz)",CLKFRQ/(double)(fifclk[i1]-fifclk[i0]));
             }

             for(i0=i-1;i0> 0    ;i0--) if (fifbit[i0]&2) break;
             for(i1=i  ;i1<fifn-1;i1++) if (fifbit[i1]&2) break;
             if (i0 >= 0)
             {
                 dd.drawline((fifclk[i0]-xoff)/zoom,YOFF+93,(fifclk[i1]-xoff)/zoom,YOFF+93,0x808080);
                 dd.print6x8(mousx+2,YOFF+85,FONT_F_COLOR,FONT_B_COLOR,"%d clk",fifclk[i1]-fifclk[i0]);
                 dd.print6x8(mousx+2,YOFF+95,FONT_F_COLOR,FONT_B_COLOR,"(%.1f Hz)",CLKFRQ/(double)(fifclk[i1]-fifclk[i0]));
             }

             break;
         }
    }

    // Scope time cursor
    dd.drawline(mousx,YOFF,mousx,YOFF+200,0x406080);
    dd.print6x8(mousx,YOFF - 8,FONT_F_COLOR,FONT_B_COLOR,"%.6f sec",(double)(mousx*zoom + xoff)/CLKFRQ);

    // Scope captions
    dd.print6x8(5, YOFF+ 13, FONT_F_COLOR, FONT_B_COLOR, "X Pulse:");
    dd.print6x8(5, YOFF+ 53, FONT_F_COLOR, FONT_B_COLOR, "X Dir:");
    dd.print6x8(5, YOFF+ 93, FONT_F_COLOR, FONT_B_COLOR, "Y Pulse:");
    dd.print6x8(5, YOFF+133, FONT_F_COLOR, FONT_B_COLOR, "Y Dir:");
    dd.print6x8(5, YOFF+173, FONT_F_COLOR, FONT_B_COLOR, "Pen Down:");
#endif // ENABLE_SCOPE
}

} // namespace brown
