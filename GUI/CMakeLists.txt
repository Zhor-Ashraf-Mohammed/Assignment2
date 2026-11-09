cmake_minimum_required(VERSION 3.19)

project(VoleSimulatorGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Folder layout (inside the zip):
#   GUI/      <- this CMakeLists.txt, main.cpp, mainwindow.h, mainwindow.cpp
#   Headers/  <- simulator headers
#   Source/   <- simulator sources
set(SIM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../GitHub/Assignment2)

qt_add_executable(VoleSimulatorGUI
    WIN32

    main.cpp
    mainwindow.h
    mainwindow.cpp

    ${SIM_DIR}/Headers/Screen.h
    ${SIM_DIR}/Headers/RegisterFile.h
    ${SIM_DIR}/Headers/Instruction.h
    ${SIM_DIR}/Headers/Opcode.h
    ${SIM_DIR}/Headers/CPU.h
    ${SIM_DIR}/Headers/CpuStatus.h
    ${SIM_DIR}/Headers/Program.h
    ${SIM_DIR}/Headers/ProgramLoader.h
    ${SIM_DIR}/Headers/FloatCodec.h
    ${SIM_DIR}/Headers/Memory.h
    ${SIM_DIR}/Headers/VoleMachine.h

    ${SIM_DIR}/Source/Screen.cpp
    ${SIM_DIR}/Source/RegisterFile.cpp
    ${SIM_DIR}/Source/Instruction.cpp
    ${SIM_DIR}/Source/CPU.cpp
    ${SIM_DIR}/Source/Program.cpp
    ${SIM_DIR}/Source/ProgramLoader.cpp
    ${SIM_DIR}/Source/FloatCodec.cpp
    ${SIM_DIR}/Source/Memory.cpp
    ${SIM_DIR}/Source/VoleMachine.cpp
)

target_include_directories(VoleSimulatorGUI PRIVATE
    ${SIM_DIR}/Headers
)

target_link_libraries(VoleSimulatorGUI PRIVATE
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(VoleSimulatorGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)