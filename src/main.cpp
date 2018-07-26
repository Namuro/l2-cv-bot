#include <iostream>

#include "Screenshot.h"

int main(int argc, char* argv[])
{
    Screenshot screen;
    auto rect = screen.WindowRect("Test");

    if (!rect.has_value()) {
        std::cout << "Window not found" << std::endl;
    }
    else {
        std::cout
        << rect->width
        << " " << rect->height
        << " " << rect->x
        << " " << rect->y
        << " " <<
        std::endl;
    }
}
