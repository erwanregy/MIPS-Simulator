#pragma once

#include <bitset>
#include "structures.hpp"
#include "buffers.hpp"

using std::bitset;

#define bit bitset<1>
#define word bitset<32>

struct IDStage
{
    bit Branch, Jump;
    word BranchAddr, JumpAddr;
    void Initialise();
};

struct WBStage
{
    word WriteData;
    void Initialise();
};

void Control(const bitset<6> Opcode, bit &RegWrite, bit &MemToReg, bit &MemWrite, bit &MemRead, bit &BEQ, bit &BNE, bit &Jump, bit &RegDst, bitset<3> &ALUOp, bit &ALUSrc);

void ALUControl(const bitset<3> ALUOp, const bitset<6> funct, bitset<6> &ALUFunct);

void ALU(const bitset<6> Funct, const word InputA, word InputB, const bitset<5> shamt, word &Result);

void stall(IDEXBuff::Reg IDEX, EXMEMBuff::Reg EXMEM, HDUnit& HDU);

void forward(IDEXBuff::Reg IDEX, EXMEMBuff::Reg EXMEM, MEMWBBuff::Reg MEMWB, FUnit& FU);

void writeback(const MEMWBBuff::Reg MEMWB, WBStage& WB);

void memaccess(const EXMEMBuff::Reg EXMEM, DataMem &DMem, MEMWBBuff::Reg &MEMWB);

void execute(const IDEXBuff::Reg IDEX, const EXMEMBuff::Reg MEM, const WBStage WB, const FUnit::Forward FU, EXMEMBuff::Reg& EXMEM);

void decode(const IFIDBuff::Reg IFID, const HDUnit HDU, const MEMWBBuff::Reg MEMWB, const WBStage WB, const EXMEMBuff::Reg EX, const EXMEMBuff::Reg MEM, const FUnit::Forward FU, RegFile& Reg, IDStage& ID, IDEXBuff::Reg& IDEX);

void fetch(const HDUnit HDU, const IDStage ID, word& PC, InsMem& IMem, IFIDBuff::Reg& IFID);
