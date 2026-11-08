#include "Screen.h"
#include "Memory.h"
#include "RegisterFile.h"
#include <iostream>
#include <iomanip>

Screen::Screen() {
    clearOutputBuffer();
}
void Screen::displayOutput(unsigned char value) {
    outputBuffer += static_cast<char>(value);
    std::cout << "[Screen Output]: " << static_cast<char>(value) 
              << " (Hex: 0x" << std::hex << std::uppercase << (int)value << std::dec << ")\n";
}
void Screen::displayMessage(std::string message) {
    std::cout << message << std::endl;
}
void Screen::displayMemory(const Memory& memory) {

    std::cout << "\n=================== Main Memory (256 Bytes) ===================\n";
    for (int i = 0; i < 256; ++i) {
        if (i % 16 == 0) {
            std::cout << "\n0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << i << ": ";
        }
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)memory.read(i) << " ";
    }
    std::cout << std::dec << "\n===============================================================\n";
}
void Screen::displayRegisters(const RegisterFile& registers) {

    std::cout << "\n================ CPU Registers (R0 - RF) ================\n";
    for (int i = 0; i < 16; ++i) {
        std::cout << "R" << std::hex << std::uppercase << i << ": 0x" 
                  << std::setw(2) << std::setfill('0') << (int)registers.readRegister(i) << "  ";
        if ((i + 1) % 4 == 0) std::cout << "\n";
    }
    std::cout << std::dec << "=========================================================\n";
}
void Screen::displayCPUState() {
    std::cout << "\n[CPU State]: Displaying execution status...\n";
}
void Screen::clearOutputBuffer() {
    outputBuffer = "";
}