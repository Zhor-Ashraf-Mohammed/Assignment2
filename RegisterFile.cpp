#include "RegisterFile.h"
RegisterFile::RegisterFile(){
    clear();
}
unsigned char RegisterFile::readRegister(int registerIndex) const {
    if(registerIndex >= 0 && registerIndex < 16) {
        return registers[registerIndex];
    }
    return 0;
}
void RegisterFile::writeRegister(int registerIndex, unsigned char value) {
    if( registerIndex >= 0 && registerIndex < 16) {
        registers[registerIndex] = value;
    }
}
void RegisterFile::clear() {
    for (int i = 0; i < 16; ++i){
        registers[i] = 0;
    }
}