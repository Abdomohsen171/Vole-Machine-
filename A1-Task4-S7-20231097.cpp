#include "A1-Task4-S7-20231097.h"


std::string Memory::getCell(int address) {
    if (address >= 0 && address < size)
        return memory[address];
    else {
        std::cerr << "Error: Memory address out of bounds (" << address << ")" << std::endl;
        return "";
    }
}

void Memory::setCell(int address, const std::string& val) {
    if (address >= 0 && address < size) {
        memory[address] = val;
    } else {
        std::cerr << "Error: Memory address out of bounds (" << address << ")" << std::endl;
    }
}


int Register::getCell(int address) {
    if (address >= 0 && address < size)
        return memory[address];
    else {
        std::cerr << "Error: Register address out of bounds (" << address << ")" << std::endl;
        return -1;
    }
}

void Register::setCell(int address, int val) {
    if (address >= 0 && address < size) {
        memory[address] = val;
    } else {
        std::cerr << "Error: Register address out of bounds (" << address << ")" << std::endl;
    }
}


int Alu::hexToDecimal(const std::string &hex) {
    int resultDecimal = 0;
    int hexBase = 1;
    for (int i = hex.size() - 1; i >= 0; i--) {
        char currentChar = toupper(hex[i]);
        if (currentChar >= '0' && currentChar <= '9') {
            resultDecimal += (currentChar - '0') * hexBase;
        } else if (currentChar >= 'A' && currentChar <= 'F') {
            resultDecimal += (currentChar - 'A' + 10) * hexBase;
        }
        hexBase *= 16;
    }
    return resultDecimal;
}

int Alu::hex_dec(char character) {
    if (character >= '0' && character <= '9') {
        return character - '0';
    } else if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    } else if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    }
    return 0;
}

std::string Alu::decimalToBinary(int decimal) {
    if (decimal == 0) return "0";

    std::string binaryString;
    while (decimal > 0) {
        binaryString = (decimal % 2 == 0 ? "0" : "1") + binaryString;
        decimal /= 2;
    }
    return binaryString;
}

int Alu::addIntegers(int a, int b) {
    int computedSum = a + b;
    computedSum &= 0xFF;
    return computedSum;
}

float Alu::addFloatingPoints(float a, float b) {
    return a + b;
}

float Alu::FloatFUN(std::string hexString) {
    int numericValue = hexToDecimal(hexString);
    if (numericValue <= 15) {
        std::string binaryResult = "0.0000" + decimalToBinary(numericValue);
        float computedTotal = 0.0;
        int powerIndex = -5;

        for (int i = 6; i < binaryResult.length(); i++) {
            if (binaryResult[i] == '1') {
                computedTotal += pow(2, powerIndex);
            }
            powerIndex--;
        }
        return computedTotal;
    } else {
        char firstChar = hexString[0];
        char secondChar = hexString[1];
        int primaryNum = hex_dec(firstChar);
        int secondaryNum = hex_dec(secondChar);
        if (primaryNum <= 7) {
            int adjustedPrimary = primaryNum - 4;
            if (adjustedPrimary <= 0) {
                return computeBinaryWithPadding(secondaryNum, -adjustedPrimary);
            } else {
                return computeAdjustedBinary(secondaryNum, adjustedPrimary);
            }
        } else {
            int offsetValue = primaryNum - 8;
            return computeAdjustedBinary(secondaryNum, offsetValue - 4, true);
        }
    }
}

float Alu::computeBinaryWithPadding(int hexValue, int paddingLength) {
    std::string paddingBinary = "0.";
    for (int i = 0; i < paddingLength; i++) {
        paddingBinary += "0";
    }
    paddingBinary += decimalToBinary(hexValue);

    float calculatedSum = 0;
    int exponent = 0;
    for (int i = 0; i < paddingBinary.length(); i++) {
        if (paddingBinary[i] == '1') {
            calculatedSum += pow(2, exponent);
            exponent--;
        } else if (paddingBinary[i] != '.') {
            exponent--;
        }
    }
    return calculatedSum;
}

float Alu::computeAdjustedBinary(int hexValue, int shiftCount, bool isNegative) {
    std::string binaryRepresentation = "0." + decimalToBinary(hexValue);
    float finalValue = 0;
    int exponentPosition = shiftCount;

    for (int i = 0; i < binaryRepresentation.length(); i++) {
        if (binaryRepresentation[i] == '1') {
            finalValue += pow(2, exponentPosition);
            exponentPosition--;
        } else if (binaryRepresentation[i] != '.') {
            exponentPosition--;
        }
    }
    return isNegative ? -finalValue : finalValue;
}


void CPU::runNextStep(Memory& memory) {
    instructionRegister = memory.getCell(programCounter);


    if (instructionRegister.empty() || instructionRegister == "0000" || instructionRegister[0] == 'C') {
        programCounter = -1; 
        return;
    }

    cu->decodeAndExecute(*this, memory);


    if (programCounter != -1 && instructionRegister[0] != 'C') {
        programCounter++;
    }
}

void CU::decodeAndExecute(CPU& cpu, Memory& memory) {
    std::string instruction = cpu.instructionRegister;

    if (instruction == "0000") {
        return;  
    }

    char op1 = instruction[0];
    int op2 = cpu.alu.hexToDecimal(instruction.substr(1, 1));
    int op3 = cpu.alu.hexToDecimal(instruction.substr(2, 1));
    int op4 = cpu.alu.hexToDecimal(instruction.substr(3, 1));

    switch (op1) {
        case '1': {
            int regIndex = op2;
            int memAddress = (op3 << 4) | op4;
            cpu.reg.setCell(regIndex, cpu.alu.hexToDecimal(memory.getCell(memAddress)));
            break;
        }
        case '2': {
            int regIndex = op2;
            int value = (op3 << 4) | op4;
            cpu.reg.setCell(regIndex, value);
            break;
        }
        case '3': {
            int regIndex = op2;
            int memAddress = (op3 << 4) | op4;
            memory.setCell(memAddress, std::to_string(cpu.reg.getCell(regIndex)));
            break;
        }
        case '4': {
            int srcReg = op3;
            int destReg = op4;
            int value = cpu.reg.getCell(srcReg);
            if (value != -1) {
                cpu.reg.setCell(destReg, value);
            }
            break;
        }
        case '5': {
            int regDest = op2;
            int regSrc1 = op3;
            int regSrc2 = op4;
            int sum = cpu.alu.addIntegers(cpu.reg.getCell(regSrc1), cpu.reg.getCell(regSrc2));
            cpu.reg.setCell(regDest, sum);
            break;
        }
        case '6': {
            int reg1 = op2;
            int reg2 = op3;
            int regDest = op4;
            float sum = cpu.alu.addFloatingPoints(static_cast<float>(cpu.reg.getCell(reg2)), static_cast<float>(cpu.reg.getCell(regDest)));
            cpu.reg.setCell(reg1, static_cast<int>(sum));
            break;
        }
        case 'B': {
            int memAddress = (op3 << 4) | op4;
            if (cpu.reg.getCell(0) == 0) {
                cpu.programCounter = memAddress - 1;  
            }
            break;
        }
        case 'C': {
            cpu.programCounter = -1;  
            break;
        }
        default:
            std::cerr << "Unknown opcode: " << op1 << " at PC = " << cpu.programCounter << std::endl;
            cpu.programCounter = -1;  
            break;
    }
}

void Machine::loadProgramFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    int addr = 0;
    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        for (size_t i = 0; i < line.length(); i += 4) {
            if (i + 3 < line.length()) {
                std::string instruction = line.substr(i, 4);
                memory.setCell(addr++, instruction);
                if (addr >= Memory::size) {
                    std::cerr << "Warning: Memory overflow while loading instructions" << std::endl;
                    break;
                }
            }
        }
    }
    file.close();
}

void Machine::executeProgram() {
    cpu.cu = new CU();
    while (cpu.programCounter < Memory::size && cpu.programCounter != -1) {
        cpu.runNextStep(memory);
    }
    delete cpu.cu;
}

void Machine::displayFormattedOutput() {
    std::cout << "Memory Content:" << std::endl;
    for (int i = 0; i < Memory::size; ++i) {
        std::cout << "Memory[" << std::setw(3) << std::setfill('0') << i << "] = "
                  << (memory.getCell(i).empty() ? "00" : memory.getCell(i)) << std::endl;
    }
    std::cout << "\nRegister Content:" << std::endl;
    for (int i = 0; i < Register::size; i++) {
        std::cout << "R" << std::dec << i << ": "
                  << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << cpu.reg.getCell(i) << " ";
    }
    std::cout << std::endl;
}

void Machine::printScreenOutputAndHalt(int outputValue) {
    std::cout << "Screen Output: " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << outputValue << std::endl;
    std::cout << "Halting program" << std::endl;
}

void MainUI::DisplayMenu() {
    std::cout << "Enter filename containing instructions: ";
    std::string filename;
    std::cin >> filename;
    machine.loadProgramFile(filename);
    machine.executeProgram();
    machine.displayFormattedOutput();
}

