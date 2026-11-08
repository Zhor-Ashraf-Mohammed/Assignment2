#ifndef VOLE_MACHINE_H
#define VOLE_MACHINE_H

#include <vector>
#include <string>

class VoleMachine {
    std::vector<std::string> memory;
    std::vector<int> registers;
    int pc;
    bool halted;

public:
    VoleMachine();
    void loadProgram(std::vector<std::string>& prog);
    void step();
    void run();
    void reset();
    void getStatus();
};



#endif