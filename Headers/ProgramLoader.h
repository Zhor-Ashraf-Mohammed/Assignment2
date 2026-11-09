#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include <cstdint>
#include <string>
#include "Program.h"

class ProgramLoader {
private:
    std::string filename;

public:
    explicit ProgramLoader(const std::string& file);

    Program loadFromFile(uint8_t startAddress = 0) const;

    bool validateInstruction(const std::string& instruction) const;
};

#endif