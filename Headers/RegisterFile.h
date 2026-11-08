#ifndef REGISTERFILE_H
#define REGISTERFILE_H

class RegisterFile {
    private:
    unsigned char registers[16];
    public:
    RegisterFile();
    unsigned char readRegister(int registerIndex) const;
    void writeRegister(int registerIndex, unsigned char value);
    void clear();
};

#endif