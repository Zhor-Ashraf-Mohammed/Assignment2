#include "VoleMachine.h"
#include <iostream>

VoleMachine::VoleMachine() {
    memory.resize(256, "00");
    registers.resize(16, 0);
    pc = 0;
    halted = false;
}

void VoleMachine::loadProgram(std::vector<std::string>& prog) {
    for (int i = 0; i < prog.size(); i++) {
        memory[i] = prog[i];
    }
    pc = 0;
    halted = false;
}

void VoleMachine::step() {
    if (halted) return;
    
    if (pc >= memory.size() || memory[pc] == "00" || memory[pc] == "C000") {
        halted = true;
        return;
    }
    
    pc++;
}

void VoleMachine::run() {
    while (!halted) {
        step();
    }
}

void VoleMachine::reset() {
    pc = 0;
    halted = false;
    for (int i = 0; i < registers.size(); i++) {
        registers[i] = 0;
    }
    for (int i = 0; i < memory.size(); i++) {
        memory[i] = "00";
    }
}

void VoleMachine::getStatus() {
    std::cout << "PC = " << pc << "\n";
}