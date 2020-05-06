/**
 * @brief    `Tile` class.
 * @author   Ken Silverman, Haoze Zhang
 *
 * Distributed under MIT License
 * Copyright (c) 2020 Haoze Zhang | Brown Engineering
 */

#ifndef _TILE_HH_
#define _TILE_HH_

#include <cstdint>

namespace brown {

class Tile {
public:
    Tile();
    Tile(int x, int y);
    Tile(int x, int y, std::uint32_t fill);
    Tile(std::uintptr_t f, int p, int x, int y);
    ~Tile();

    void set(std::uintptr_t f, int p, int x, int y);
    void setf(std::uintptr_t f) {this->f = f;}
    void setp(int p) {unreserve(); this->p = p;}
    void setx(int x) {this->x = x;}
    void sety(int y) {this->y = y;}
    std::uintptr_t getf() const {return f;}
    int getp() const {return p;}
    int getx() const {return x;}
    int gety() const {return y;}
    std::uint32_t& getpix(int x, int y);

    void print6x8     (int ox, int y, std::uint32_t fcol, std::uint32_t bcol, const char *fmt, ...);
    void drawpix      (int x, int y, std::uint32_t col);
    void drawhlin     (int x0, int x1, int y, std::uint32_t col);
    void drawrectfill (int x0, int y0, int x1, int y1, std::uint32_t col);
    void drawline     (float x0, float y0, float x1, float y1, std::uint32_t col);
    void drawcirc     (int xc, int yc, int r, std::uint32_t col);
    void drawcircfill (int xc, int yc, int r, std::uint32_t col);

private:
    std::uintptr_t f;
    int p, x, y;
    bool reserved = false;
    void unreserve();
}; // class Tile

} // namespace brown

#endif // _TILE_HH_
