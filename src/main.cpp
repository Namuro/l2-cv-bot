#include <iostream>

#include "Runloop.h"
#include "Intercept.h"

int main(int argc, char* argv[])
{
    try {
        ::Runloop runloop{argc, argv};
        runloop.Run();
    } catch (::Intercept::InterceptionDriverNotFoundError &error) {
        std::cout << error.what() << std::endl;
    }

    return 0;
}
