#ifndef PROGRAM_H
#define PROGRAM_H

#include <cstdint>
#include <vector>

using namespace std;

class Program {
private:
    uint8_t startAddress;
    vector<uint8_t> bytes;

public:
    Program(uint8_t startAddr, vector<uint8_t> programBytes);

    uint8_t getStartAddress() const;
    vector<uint8_t> getBytes() const;
    int size() const;
};

#endif