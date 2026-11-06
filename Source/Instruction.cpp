#include "../Headers/Instruction.h"
#include <iostream>

Instruction::Instruction(uint16_t word) {
    rawWord = word;

    uint8_t p1 = (word >> 12) & 0x0F;
    uint8_t p2 = (word >> 8) & 0x0F;
    uint8_t p3 = (word >> 4) & 0x0F;
    uint8_t p4 = word & 0x0F;

    opcode = static_cast<Opcode>(p1);
    r = p2;
    s = p3;
    t = p4;

    address = (p3 << 4) | p4;
}

Opcode Instruction::getOpcode() const { return opcode; }
uint8_t Instruction::getR() const { return r; }
uint8_t Instruction::getS() const { return s; }
uint8_t Instruction::getT() const { return t; }
uint8_t Instruction::getAddress() const { return address; }
uint16_t Instruction::getRaw() const { return rawWord; }

bool Instruction::isValid() const {
    int op = static_cast<int>(opcode);
    if (op >= 1 && op <= 12) {
        return true;
    }
    return false;
}

string Instruction::toString() const {
    char str[10];
    sprintf(str, "%04X", rawWord);
    return string(str);
}