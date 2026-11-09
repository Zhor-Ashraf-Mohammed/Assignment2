#include "VoleMachine.h"
#include "ProgramLoader.h"

#include <stdexcept>
#include <vector>

VoleMachine::VoleMachine()
    : memory(),
      screen(),
      cpu(memory, screen),
      programLoaded(false),
      programStartAddress(0) {

    cpu.reset(0);
}

void VoleMachine::loadProgram(const Program& program) {

    const std::vector<uint8_t> bytes = program.getBytes();

    const unsigned int start = program.getStartAddress();

    const unsigned int end =
        start + static_cast<unsigned int>(bytes.size());

    if (bytes.empty()) {
        throw std::runtime_error(
            "Cannot load an empty program."
        );
    }

    if (end > 254) {
        throw std::runtime_error(
            "Program exceeds executable memory."
        );
    }

    memory.clear();

    for (std::size_t i = 0; i < bytes.size(); i++) {

        memory.write(
            static_cast<int>(start + i),
            bytes[i]
        );
    }

    programStartAddress = program.getStartAddress();

    programLoaded = true;

    cpu.reset(programStartAddress);
}

void VoleMachine::loadProgram(
    const std::string& filename,
    uint8_t startAddress) {

    ProgramLoader loader(filename);

    Program validatedProgram =
        loader.loadFromFile(startAddress);

    loadProgram(validatedProgram);
}

void VoleMachine::step() {

    if (programLoaded &&
        cpu.getStatus() == CpuStatus::RUNNING) {

        cpu.step();
    }
}

void VoleMachine::run() {

    while (programLoaded &&
           cpu.getStatus() == CpuStatus::RUNNING) {

        cpu.step();
    }
}

void VoleMachine::reset() {

    memory.clear();
    screen = Screen();
    cpu.reset(programStartAddress);
}

void VoleMachine::getStatus() {

    screen.displayCPUState(cpu);
}

CPU& VoleMachine::getCPU() {
    return cpu;
}

Memory& VoleMachine::getMemory() {
    return memory;
}

Screen& VoleMachine::getScreen() {
    return screen;
}