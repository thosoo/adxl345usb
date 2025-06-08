#ifndef MBED_H
#define MBED_H
#include <cstdint>
#include <vector>

namespace mbed {
class SPI {
public:
    std::vector<uint8_t> written;
    SPI(int, int, int) {}
    void frequency(int) {}
    void format(int, int) {}
    uint8_t write(uint8_t val) { written.push_back(val); return 0; }
};
}

#endif
