#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include "stages.hpp"

using std::cerr, std::endl, std::bitset, std::string, std::vector;

#define bit bitset<1>
#define word bitset<32>

class RegFile
{
public:
    word Register[32];
    void Initialise();
    void Read(bitset<5> ReadReg1, bitset<5> ReadReg2, word &ReadData1, word &ReadData2);
    void Write(bitset<5> WriteReg, word WriteData);
};

class Mem
{
public:
    word *Memory;
    int Size;
    void Initialise(int MemSize);
    void Read(word Address, word &Data);
};

class InsMem : public Mem
{
public:
    vector<string> basic;
    void Initialise(string assembly, int MemSize);
};

class DataMem : public Mem
{
public:
    void Write(word Address, word Data);
};

void Control(bitset<6> Opcode, bit &RegWrite, bit &MemToReg, bit &MemWrite, bit &MemRead, bit &BEQ, bit &BNE, bit &Jump, bit &RegDst, bitset<3> &ALUOp, bit &ALUSrc);

void ALUControl(bitset<3> ALUOp, bitset<6> funct, bitset<6> &ALUFunct);

void ALU(bitset<6> Funct, word InputA, word InputB, bitset<5> shamt, word &Result);

class FUnit
{
public:
    bitset<2> ForwardA, ForwardB;
    void Initialise();
    void forward(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM, MEMWBBuff::Out MEMWB);
};

class IDFUnit : public FUnit
{
    virtual void Forward(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM, MEMWBBuff::Out MEMWB);
};

class EXFUnit : public FUnit
{
    virtual void Forward(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM, MEMWBBuff::Out MEMWB);
};

class HDUnit
{
public:
    bit PCWrite, IFFlush, IFIDWrite, Control;
    void Initialise();
    void HazardDetect(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM);
};