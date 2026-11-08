#ifndef FLOATCODEC_H
#define FLOATCODEC_H

#include <cstdint>

class FloatCodec {
public:
    static double decode(uint8_t val);
    static uint8_t encode(double val);
};

#endif