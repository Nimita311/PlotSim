/**
 * @brief    Simulation configurations.
 * @author   Haoze Zhang
 *
 * Distributed under MIT License
 * Copyright (c) 2020 Haoze Zhang | Brown Engineering
 */

#ifndef _CONFIG_HH_
#define _CONFIG_HH_

/* Application settings *****************************************************/
#define WINDOW_NAME_ "PlotSim"
#define XRES_ 1600
#define YRES_ 800

/* Simulation settings ******************************************************/
// Canvas size [pixel]
#define BMPXRES_ 800
#define BMPYRES_ 800
// Simulator clock frequency [tick/second]
#define CLKFRQ_  static_cast<std::int64_t>(1e8)
// Restriction switch
#define ENABLE_RESTRICTION 1
// Maximum speed [step/second]
#define MAXVEL_  10000
// Minimum speed without acceleration restriction [step/second]
#define MINVEL_  1000
// Maximum acceleration [step/second^2]
#define MAXACC_  640000
// Leadscrew ratio [step/pixel]
#define PULPERPIX_ (1<<6)
// Minimum position [step]
#define MINPOS_ (BMPXRES_*-PULPERPIX_/2)
// Maximum position [step]
#define MAXPOS_ (BMPXRES_*+PULPERPIX_/2)
// Time scale, 1.0 real time; >1.0 speed up
#define TIME_SCALE 1.0

/* Scope ********************************************************************/
#define ENABLE_SCOPE 1
#define FIFMAX_ (1L<<24)
#define YOFF 615

/* Colors *******************************************************************/
#define BACKGROUND_COLOR 0x000000
#define DOWN_COLOR 0xd0c0b0
#define UP_COLOR 0x102030
#define DOWN_LOC_COLOR 0x00ff00
#define UP_LOC_COLOR 0xff0000
#define FONT_F_COLOR 0xffffff
#define FONT_B_COLOR BACKGROUND_COLOR

#endif // _CONFIG_HH_
