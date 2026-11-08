#include "ProgramLoader.h"
#include <fstream>

ProgramLoader::ProgramLoader(std::string f) {
    filename = f;
}

std::vector<std::string> ProgramLoader::loadFromFile() {
    std::vector<std::string> v;
    std::ifstream file(filename);
    std::string s;
    while (file >> s) {
        v.push_back(s);
    }
    return v;
}

bool ProgramLoader::validateInstruction(std::string inst) {
    if (inst.empty()) return false;
    return true;
}

void ProgramLoader::loadIntoMemory(std::vector<std::string>& mem, std::vector<std::string>& insts) {
    for (int i = 0; i < insts.size(); i++) {
        if (validateInstruction(insts[i])) {
            mem.push_back(insts[i]);
        }
    }
}