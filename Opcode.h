#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>

enum class Opcode : uint8_t {
    LOAD_MEM = 1,
    LOAD_IMM = 2,
    STORE = 3,
    MOVE = 4,
    ADD_INT = 5,
    ADD_FLOAT = 6,
    OR = 7,
    AND = 8,
    XOR = 9,
    ROTATE = 10,
    JUMP = 11,
    HALT = 12,
    UNKNOWN = 0
};

#endif