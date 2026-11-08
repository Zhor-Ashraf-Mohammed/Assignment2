#include "../Headers/FloatCodec.h"

double FloatCodec::decode(uint8_t val) {
    if (val == 0) return 0.0;

    int sign = 1;
    if ((val & 0x80) != 0) {
        sign = -1;
    }

    int exp = ((val >> 4) & 0x07) - 4;
    double mantissa = (val & 0x0F) / 16.0;

    double p = 1.0;
    if (exp > 0) {
        for (int i = 0; i < exp; i++) {
            p *= 2.0;
        }
    } else if (exp < 0) {
        for (int i = 0; i < -exp; i++) {
            p /= 2.0;
        }
    }

    return sign * mantissa * p;
}

uint8_t FloatCodec::encode(double val) {
    if (val == 0.0) return 0;

    uint8_t res = 0;

    if (val < 0) {
        res |= 0x80;
        val = -val;
    }

    int exp = 0;
    while (val >= 1.0 && exp < 3) {
        val /= 2.0;
        exp++;
    }
    while (val < 0.5 && exp > -4) {
        val *= 2.0;
        exp--;
    }

    int expBits = exp + 4;
    int mantissaBits = (int)(val * 16.0);

    res |= ((expBits & 0x07) << 4);
    res |= (mantissaBits & 0x0F);

    return res;
}