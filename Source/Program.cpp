#include "../Headers/Program.h"

Program::Program(uint8_t startAddr, vector<uint8_t> programBytes) {
    startAddress = startAddr;
    bytes = programBytes;
}

uint8_t Program::getStartAddress() const {
    return startAddress;
}

vector<uint8_t> Program::getBytes() const {
    return bytes;
}

int Program::size() const {
    return bytes.size();
}
