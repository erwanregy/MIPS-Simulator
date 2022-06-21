#include <iostream>
#include "structures.hpp"
#include "assembler.hpp"

using std::cerr;

void RegFile::Initialise()
{
    for (int i = 0; i < sizeof(Register)/sizeof(Register[0]); i++) Register[i] = 0;
}
void RegFile::Write(const bitset<5> WriteReg, const word WriteData)
{
    if (WriteReg.to_ulong() > 0 && WriteReg.to_ulong() < 26) Register[WriteReg.to_ulong()] = WriteData;
    else
    {
        if (WriteReg == 0) cerr << "Warning: Tried to write to $zero which is reserved for the value 0\n";
        else if (WriteReg == 26) cerr << "Warning: Tried to write to $k0 which is an unsupported register\n";
        else if (WriteReg == 27) cerr << "Warning: Tried to write to $k1 which is an unsupported register\n";
        else if (WriteReg == 28) cerr << "Warning: Tried to write to $gp which is an unsupported register\n";
        else if (WriteReg == 29) cerr << "Warning: Tried to write to $sp which is an unsupported register\n";
        else if (WriteReg == 30) cerr << "Warning: Tried to write to $fp which is an unsupported register\n";
        else if (WriteReg == 31) cerr << "Warning: Tried to write to $ra which is an unsupported register\n";
        else cerr << "Warning tried to write to $" << WriteReg << " which does not exist\n";
    }
}
void RegFile::Read(const bitset<5> ReadReg1, const bitset<5> ReadReg2, word &ReadData1, word &ReadData2)
{
    if (ReadReg1.to_ulong() < 26 && ReadReg2.to_ulong() < 26)
    {
        ReadData1 = Register[ReadReg1.to_ulong()];
        ReadData2 = Register[ReadReg2.to_ulong()];
    }
    else
    {
        if (ReadReg1 == 26 || ReadReg2 == 26) cerr << "Warning: Tried to read from $k0. Unsupported register";
        else if (ReadReg1 == 27 || ReadReg2 == 27) cerr << "Warning: Tried to read from $k1. Unsupported register";
        else if (ReadReg1 == 28 || ReadReg2 == 28) cerr << "Warning: Tried to read from $gp. Unsupported register";
        else if (ReadReg1 == 29 || ReadReg2 == 29) cerr << "Warning: Tried to read from $sp. Unsupported register";
        else if (ReadReg1 == 30 || ReadReg2 == 30) cerr << "Warning: Tried to read from $fp. Unsupported register";
        else if (ReadReg1 == 31 || ReadReg2 == 31) cerr << "Warning: Tried to read from $ra. Unsupported register";
    }
}

void Mem::Initialise(unsigned const int size)
{
    MemSize = size;
    Memory = new word[MemSize]; // Create memory
    for (int i = 0; i < MemSize; i++) Memory[i] = 0; // Clear memory
}
void Mem::Read(const word Address, word &Data)
{
    if ((Address >> 2).to_ulong() > MemSize) cerr << "Warning: Tried to read from address " << Address.to_ulong() << " which does not exist\n";
    else Data = Memory[(Address >> 2).to_ulong()];
}

void InsMem::Initialise(const string assembly, unsigned const int size, const bool print_basic, const bool print_imem, const int base_type)
{
    MemSize = size;
    Memory = new word[MemSize]; // Create memory
    for (int i = 0; i < MemSize; i++) Memory[i] = 0; // Clear memory
    assemble(assembly, basic, Memory, MemSize, print_basic, print_imem, base_type, PCMax);
}

void DataMem::Write(const word Address, const word Data)
{
    if ((Address >> 2).to_ulong() > MemSize) cerr << "Warning: Tried to write to address " << Address.to_ulong() << " which does not exist\n";
    else Memory[(Address >> 2).to_ulong()] = Data;
}

void HDUnit::Initialise()
{
    IFFlush = PCWrite = 0;
    IFIDWrite = Control = 1;
}

void FUnit::Initialise()
{
    ID = EX = { 0 };
};
