#ifndef SPINNER_H
#define SPINNER_H

#include <iostream>

class Spinner
{
public:
    static void show()
    {
        static int pos{0};
        const char cursor[4]{'|', '/', '-', '\\'};
        std::cout << "\r" << cursor[pos] << std::flush;
        pos = (pos + 1) % 4;
    }
};

#endif // SPINNER_H
