#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <string>
#include "Opcode.h"

using namespace std;

class Instruction {
private:
    uint16_t rawWord;
    Opcode opcode;
    uint8_t r;
    uint8_t s;
    uint8_t t;
    uint8_t address;

public:
    Instruction(uint16_t word = 0);

    Opcode getOpcode() const;
    uint8_t getR() const;
    uint8_t getS() const;
    uint8_t getT() const;
    uint8_t getAddress() const;
    uint16_t getRaw() const;

    bool isValid() const;
    string toString() const;
};

#endif