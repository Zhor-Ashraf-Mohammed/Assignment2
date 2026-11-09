Vole Machine Simulator - GUI (Qt)
CS213 Assignment 2 - Part 3 (Bonus)

REQUIREMENTS
- Qt 6 (or Qt 5) with Qt Creator
- CMake 3.16 or newer
- A C++17 compiler (MinGW or MSVC, installed with Qt)

HOW TO BUILD
1. Open Qt Creator.
2. File > Open File or Project, then select GUI/CMakeLists.txt
3. Choose a kit (for example Desktop Qt 6.x MinGW 64-bit) and click Configure Project.
4. Press Ctrl+R to build and run.

HOW TO USE
1. Set the Start Address (hex, default 00).
2. Click "Load" and choose a program .txt file.
3. Click "Single Step" to execute one instruction, or "Run" to execute the whole program.
4. Click "Halt" to pause a running program.
5. Click "Clear" to reset memory, registers and output.
6. Click "Help" for instructions and the Vole instruction set.
7. Peach = changed in last step, mint = cell the PC points to.
8. Writing to address FF prints to the Output (screen).
9. Invalid instructions show an error message / FAULTED status.

PROJECT STRUCTURE
- GUI/      Qt interface (main.cpp, mainwindow.cpp, mainwindow.h, CMakeLists.txt)
- Headers/  Simulator header files
- Source/   Simulator source files
- Programs/ Test program text files (.txt)

TEAM
- Zhor Ashraf Mohammed  - ID 20251165
- Olfat Ahmed Waheed    - ID 20251054
- Basma Sabry Abd elhay - ID 20251076
- Shahd Taha Abdelazeem - ID 20260957
  
