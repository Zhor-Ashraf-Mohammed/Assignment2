#ifndef VOLE_MACHINE_H
#define VOLE_MACHINE_H

#include <cstdint>
#include <string>

#include "CPU.h"
#include "Memory.h"
#include "Program.h"
#include "Screen.h"

class VoleMachine {

private:
    Memory memory;
    Screen screen;
    CPU cpu;

    bool programLoaded;
    uint8_t programStartAddress;

public:

    VoleMachine();

    void loadProgram(const Program& program);

    void loadProgram(
        const std::string& filename,
        uint8_t startAddress = 0
    );

    void step();

    void run();

    void reset();

    void getStatus();

    CPU& getCPU();

    Memory& getMemory();

    Screen& getScreen();
};

#endif