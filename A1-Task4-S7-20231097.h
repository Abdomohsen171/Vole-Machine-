#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <iomanip>

class Memory {
public:
    static const int size = 256;
    std::string memory[size] = {""};

    std::string getCell(int address);
    void setCell(int address, const std::string& val);
};

class Register {
public:
    static const int size = 16;
    int memory[size] = {0};

    int getCell(int address);
    void setCell(int address, int val);
};

class Alu {
public:
    int hexToDecimal(const std::string &hex);
    int hex_dec(char character);
    std::string decimalToBinary(int decimal);
    int addIntegers(int a, int b);
    float addFloatingPoints(float a, float b);
    float FloatFUN(std::string hexString);

private:
    float computeBinaryWithPadding(int hexValue, int paddingLength);
    float computeAdjustedBinary(int hexValue, int shiftCount, bool isNegative = false);
};

class CU;

class CPU {
public:
    int programCounter = 0;
    std::string instructionRegister;
    Register reg;
    Alu alu;
    CU* cu;

    void runNextStep(Memory& memory);
};

class CU {
public:
    void decodeAndExecute(CPU& cpu, Memory& memory);
};

class Machine {
private:
    Memory memory;
    CPU cpu;

public:
    void loadProgramFile(const std::string& filename);
    void executeProgram();
    void displayFormattedOutput();
    void printScreenOutputAndHalt(int outputValue);
};

class MainUI {
private:
    Machine machine;

public:
    void DisplayMenu();
};

#endif