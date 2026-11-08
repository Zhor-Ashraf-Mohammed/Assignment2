#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <string>

class Memory;
class RegisterFile;
class Screen {
    private:
    std::string outputBuffer;

    public:
    Screen();
    void displayOutput( unsigned char value);
    void displayMessage(std::string message);
    void displayMemory( const Memory& memory);
    void displayRegisters(const RegisterFile& registers);
    void displayCPUState();
    void clearOutputBuffer();
};

#endif
