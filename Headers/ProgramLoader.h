#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include <vector>
#include <string>

class ProgramLoader {
    std::string filename;
public:
    ProgramLoader(std::string f);
    std::vector<std::string> loadFromFile();
    bool validateInstruction(std::string inst);
    void loadIntoMemory(std::vector<std::string>& mem, std::vector<std::string>& insts);
};

#endif

