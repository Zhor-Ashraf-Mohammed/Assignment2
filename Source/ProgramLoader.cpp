#include "ProgramLoader.h"

#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

ProgramLoader::ProgramLoader(const std::string& file)
    : filename(file) {
}

bool ProgramLoader::validateInstruction(const std::string& inst) const {

    if (inst.size() != 4)
        return false;

    for (unsigned char ch : inst) {
        if (!std::isxdigit(ch))
            return false;
    }

    unsigned int word = 0;

    try {
        word = static_cast<unsigned int>(
            std::stoul(inst, nullptr, 16)
        );
    }
    catch (...) {
        return false;
    }

    const unsigned int opcode = (word >> 12) & 0xF;
    const unsigned int r = (word >> 8) & 0xF;
    const unsigned int s = (word >> 4) & 0xF;

    switch (opcode) {

    case 0x1:
    case 0x2:
    case 0x3:
    case 0xB:
        return true;

    case 0x4:
        return r == 0;

    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
        return true;

    case 0xA:
        return s == 0;

    case 0xC:
        return (word & 0x0FFF) == 0;

    default:
        return false;
    }
}

Program ProgramLoader::loadFromFile(uint8_t startAddress) const {

    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Cannot open program file: " + filename
        );
    }

    std::vector<uint8_t> bytes;
    std::string token;
    int tokenNumber = 0;

    while (file >> token) {

        ++tokenNumber;

        if (!validateInstruction(token)) {
            throw std::runtime_error(
                "Invalid instruction at item " +
                std::to_string(tokenNumber) +
                ": " + token
            );
        }

        unsigned int word = static_cast<unsigned int>(
            std::stoul(token, nullptr, 16)
        );

        bytes.push_back(
            static_cast<uint8_t>((word >> 8) & 0xFF)
        );

        bytes.push_back(
            static_cast<uint8_t>(word & 0xFF)
        );
    }

    if (file.bad()) {
        throw std::runtime_error(
            "Error while reading program file."
        );
    }

    if (bytes.empty()) {
        throw std::runtime_error(
            "Program file is empty."
        );
    }

    unsigned int endAddress =
        static_cast<unsigned int>(startAddress) +
        static_cast<unsigned int>(bytes.size());

    if (endAddress > 254) {
        throw std::runtime_error(
            "Program does not fit in memory."
        );
    }

    return Program(startAddress, bytes);
}