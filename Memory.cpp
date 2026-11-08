#include "Memory.h"

Memory::Memory() {
    clear();
}

unsigned char Memory::read(int address) const {
    if( address >= 0 && address < 256) {
        return cells[address];
    }
    return 0;
}
void Memory::write(int address, unsigned char value) {
    if (address >= 0 && address < 256){ 
        cells[address] = value;
    }
}
void Memory::clear(){
    for (int i = 0; i < 256; ++i){
        cells[i] = 0;
    } 
}
int Memory::getSize(){
    return 256;
}