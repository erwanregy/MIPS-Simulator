#pragma once

#include <bitset>

using std::bitset;

#define bit bitset<1>
#define word bitset<32>

struct Buffer
{
    struct Reg
    {
        bit Data;
    } In, Out;
    void Initialise();
    void Clock();
};

struct IFIDBuff : Buffer
{
    struct Reg
    {
        word PCPlus4, Instruction;
    } In, Out;
    virtual void Clock();
};

struct IDEXBuff : Buffer
{
    struct Reg
    {
        bit RegWrite, MemToReg, MemWrite, MemRead, RegDst, ALUSrc;
        bitset<3> ALUOp;
        bitset <5> SourceReg, TargReg, DestReg;
        word ReadData1, ReadData2, Imm;
    } In, Out;
    virtual void Clock();
};

struct EXMEMBuff : Buffer
{
    struct Reg
    {
        bit RegWrite, MemToReg, MemWrite, MemRead;
        bitset<5> WriteReg;
        word ALUResult, WriteData;
    } In, Out;
    virtual void Clock();
};

struct MEMWBBuff : Buffer
{
    struct Reg
    {
        bit RegWrite, MemToReg;
        bitset<5> WriteReg;
        word ALUResult, ReadData;
    } In, Out;
    virtual void Clock();
};