#include "buffers.hpp"

void Buffer::Initialise()
{
    In = Out = { 0 };
}

void IFIDBuff::Clock()
{
    Out =
    {
        In.PCPlus4, In.Instruction
    };
}

void IDEXBuff::Clock()
{
    Out =
    {
        In.RegWrite, In.MemToReg, In.MemWrite, In.MemRead, In.RegDst, In.ALUSrc,
        In.ALUOp,
        In.SourceReg, In.TargReg, In.DestReg,
        In.ReadData1, In.ReadData2, In.Imm
    };
}

void EXMEMBuff::Clock()
{
    Out =
    {
        In.RegWrite, In.MemToReg, In.MemWrite, In.MemRead,
        In.WriteReg,
        In.ALUResult, In.WriteData
    };
}

void MEMWBBuff::Clock()
{
    Out =
    {
        In.RegWrite, In.MemToReg,
        In.WriteReg,
        In.ALUResult, In.ReadData
    };
}