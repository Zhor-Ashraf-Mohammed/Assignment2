#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>

#include "Memory.h"
#include "RegisterFile.h"
#include "Screen.h"
#include "Instruction.h"
#include "CpuStatus.h"

class CPU {
private:
    uint16_t programCounter;
    uint16_t instructionRegister;

    CpuStatus status;
    std::string faultMessage;

    RegisterFile registers;

    Memory& memory;
    Screen& screen;

    bool fetch();
    Instruction decode();
    void execute(const Instruction& inst);

    void fault(const std::string& message);

    void executeLoadFromMemory(const Instruction& inst);
    void executeLoadImmediate(const Instruction& inst);
    void executeStore(const Instruction& inst);
    void executeMove(const Instruction& inst);
    void executeAddInteger(const Instruction& inst);
    void executeAddFloat(const Instruction& inst);
    void executeBitwise(const Instruction& inst);
    void executeRotate(const Instruction& inst);
    void executeJump(const Instruction& inst);
    void executeHalt();

public:
    CPU(Memory& memory, Screen& screen);

    void reset(uint8_t startAddress);
    void step();

    CpuStatus getStatus() const;
    std::string getFaultMessage() const;

    uint16_t getPC() const;
    uint16_t getIR() const;

    const RegisterFile& getRegisters() const;
};

#endif