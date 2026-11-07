#include "CPU.h"

#include "FloatCodec.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// Constructor
CPU::CPU(Memory& memory, Screen& screen)
    : programCounter(0),
      instructionRegister(0),
      status(CpuStatus::HALTED),
      faultMessage(""),
      registers(),
      memory(memory),
      screen(screen)
{
}

// Reset CPU
void CPU::reset(uint8_t startAddress)
{
    programCounter = startAddress;
    instructionRegister = 0;

    status = CpuStatus::RUNNING;
    faultMessage = "";

    registers.clear();
}

// Fetch instruction from memory
bool CPU::fetch()
{
    if (programCounter >= 255 || programCounter + 1 >= 255)
    {
        fault("Program Counter is out of memory range.");
        return false;
    }

    unsigned char highByte = memory.read(programCounter);
    unsigned char lowByte = memory.read(programCounter + 1);

    instructionRegister =
        (static_cast<uint16_t>(highByte) << 8) | lowByte;

    programCounter += 2;

    return true;
}

// Decode instruction
Instruction CPU::decode()
{
    return Instruction(instructionRegister);
}

// Execute instruction
void CPU::execute(const Instruction& inst)
{
    switch (inst.getOpcode())
    {
    case Opcode::LOAD_MEM:
        executeLoadFromMemory(inst);
        break;

    case Opcode::LOAD_IMM:
        executeLoadImmediate(inst);
        break;

    case Opcode::STORE:
        executeStore(inst);
        break;

    case Opcode::MOVE:
        executeMove(inst);
        break;

    case Opcode::ADD_INT:
        executeAddInteger(inst);
        break;

    case Opcode::ADD_FLOAT:
        executeAddFloat(inst);
        break;

    case Opcode::OR:
    case Opcode::AND:
    case Opcode::XOR:
        executeBitwise(inst);
        break;

    case Opcode::ROTATE:
        executeRotate(inst);
        break;

    case Opcode::JUMP:
        executeJump(inst);
        break;

    case Opcode::HALT:
        executeHalt();
        break;

    default:
        fault("Unknown instruction opcode.");
        break;
    }
}

// Execute one instruction
void CPU::step()
{
    if (status != CpuStatus::RUNNING)
        return;

    if (!fetch())
        return;

    Instruction inst = decode();

    if (!inst.isValid())
    {
        fault("Invalid instruction.");
        return;
    }

    execute(inst);
}

// Load from memory
void CPU::executeLoadFromMemory(const Instruction& inst)
{
    int r = inst.getR();
    int address = inst.getAddress();

    unsigned char value = memory.read(address);

    registers.writeRegister(r, value);
}

// Load immediate value
void CPU::executeLoadImmediate(const Instruction& inst)
{
    int r = inst.getR();

    unsigned char value = inst.getAddress();

    registers.writeRegister(r, value);
}

// Store register value in memory
void CPU::executeStore(const Instruction& inst)
{
    int r = inst.getR();
    int address = inst.getAddress();

    unsigned char value = registers.readRegister(r);

    if (address == 255)
    {
        screen.displayOutput(value);
    }
    else
    {
        memory.write(address, value);
    }
}

// Move register R to register S
void CPU::executeMove(const Instruction& inst)
{
    int r = inst.getS();
    int s = inst.getT();

    unsigned char value = registers.readRegister(r);

    registers.writeRegister(s, value);
}

// Add as two's complement integers
void CPU::executeAddInteger(const Instruction& inst)
{
    int r = inst.getR();
    int s = inst.getS();
    int t = inst.getT();

    unsigned char value1 = registers.readRegister(s);
    unsigned char value2 = registers.readRegister(t);

    unsigned char result = value1 + value2;

    registers.writeRegister(r, result);
}

// Add floating-point values
void CPU::executeAddFloat(const Instruction& inst)
{
    int r = inst.getR();
    int s = inst.getS();
    int t = inst.getT();

    unsigned char value1 = registers.readRegister(s);
    unsigned char value2 = registers.readRegister(t);

    double number1 = FloatCodec::decode(value1);
    double number2 = FloatCodec::decode(value2);

    unsigned char result = FloatCodec::encode(number1 + number2);

    registers.writeRegister(r, result);
}

// Bitwise operations
void CPU::executeBitwise(const Instruction& inst)
{
    int r = inst.getR();
    int s = inst.getS();
    int t = inst.getT();

    unsigned char value1 = registers.readRegister(s);
    unsigned char value2 = registers.readRegister(t);

    unsigned char result = 0;

    switch (inst.getOpcode())
    {
    case Opcode::OR:
        result = value1 | value2;
        break;

    case Opcode::AND:
        result = value1 & value2;
        break;

    case Opcode::XOR:
        result = value1 ^ value2;
        break;

    default:
        fault("Invalid bitwise operation.");
        return;
    }

    registers.writeRegister(r, result);
}

// Rotate register right
void CPU::executeRotate(const Instruction& inst)
{
    int r = inst.getR();
    int rotations = inst.getAddress();

    unsigned char value = registers.readRegister(r);

    rotations %= 8;

    for (int i = 0; i < rotations; i++)
    {
        unsigned char lastBit = value & 1;

        value >>= 1;

        if (lastBit)
            value |= 128;
    }

    registers.writeRegister(r, value);
}

// Jump if register R equals register 0
void CPU::executeJump(const Instruction& inst)
{
    int r = inst.getR();
    int address = inst.getAddress();

    if (registers.readRegister(r) ==
        registers.readRegister(0))
    {
        programCounter = address;
    }
}

// Halt CPU
void CPU::executeHalt()
{
    status = CpuStatus::HALTED;
}

// Handle errors
void CPU::fault(const string& message)
{
    status = CpuStatus::FAULTED;
    faultMessage = message;

    screen.displayMessage(message);
}

// Getters
CpuStatus CPU::getStatus() const
{
    return status;
}

string CPU::getFaultMessage() const
{
    return faultMessage;
}

uint16_t CPU::getPC() const
{
    return programCounter;
}

uint16_t CPU::getIR() const
{
    return instructionRegister;
}

const RegisterFile& CPU::getRegisters() const
{
    return registers;
}