#include "Hands.h"

#include <iostream>

void Hands::MoveMouseSmoothly(int x1, int y1)
{
    //const auto from = m_input.GetMousePos();
    //auto x0 = from.x;
    //auto y0 = from.y;

    //bool steep = Math.Abs(y1 - y0) > Math.Abs(x1 - x0);
    //if (steep) { Swap<int>(ref x0, ref y0); Swap<int>(ref x1, ref y1); }
    //if (x0 > x1) { Swap<int>(ref x0, ref x1); Swap<int>(ref y0, ref y1); }
    //int dX = (x1 - x0), dY = Math.Abs(y1 - y0), err = (dX / 2), ystep = (y0 < y1 ? 1 : -1), y = y0;

    //for (int x = x0; x <= x1; ++x)
    //{
    //    if (!(steep ? plot(y, x) : plot(x, y))) return;
    //    err = err - dY;
    //    if (err < 0) { y += ystep;  err += dX; }
    //}

    //const auto dx = std::abs(x1 - x0);
    //const auto dy = std::abs(y1 - y0);
    //const auto steep = 

    ////const auto dx = std::abs(x1 - x0);
    ////const auto dy = std::abs(y1 - y0);
    ////auto error = dy - dx / 2;

    ////for (; x0 != x1; ++x0) {
    ////    m_input.MouseMove(x0, y0, 1);
    ////    std::cout << "x = " << x0 << ", y = " << y0 << std::endl;

    ////    if (error < 0) {
    ////        error += dy;
    ////    }
    ////    else {
    ////        y0 += 1;
    ////        error += dy - dx;
    ////    }
    ////}

    ////std::cout << "end" << std::endl;

    //m_input.Send(true);
}
