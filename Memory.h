#ifndef MEMORY_H
#define MEMORY_H

class Memory {
    private: 
    unsigned char cells [256];

    public:
    Memory();
    unsigned char read( int address) const;
    void write( int address, unsigned char value);
    void clear();
    int getSize();

};
#endif