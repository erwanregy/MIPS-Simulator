#include <bitset>

using std::bitset;

#define bit bitset<1>
#define word bitset<32>

class Buffer
{
public:
    struct Reg {};
    struct In : public Reg {};
    struct Out : public Reg {};
    void Initialise();
    void Clock();
};

class IFIDBuff : public Buffer
{
public:
    word PCPlus4, Instruction;
    virtual void Clock(IFIDIn In, IFIDOut &Out)
    {
        Out.PCPlus4 = In.PCPlus4;
    }
};
class IFIDIn : public IFIDBuff {};
class IFIDOut : public IFIDBuff {};

class IDEXBuff : public Buffer
{
public:
    bit RegWrite, MemToReg, MemWrite, MemRead, RegDst, ALUSrc;
    bitset<3> ALUOp;
    bitset <5> SourceReg, TargReg, DestReg;
    word ReadData1, ReadData2, Imm;
    virtual void Clock(IDEXIn In, IDEXOut &Out)
    {
        Out.PCPlus4 = In.PCPlus4;
    }
};
class IDEXIn : public IFIDBuff {};
class IDEXOut : public IFIDBuff {};

class EXMEMBuff : public Buffer
{
public:
    struct Reg
    {
        bit RegWrite, MemToReg, MemWrite, MemRead;
        bitset<5> WriteReg;
        word ALUResult, WriteData;
    };
    struct In : public Reg {};
    struct Out : public Reg {};
};

class MEMWBBuff : public Buffer
{
public:
    struct Reg
    {
        bit RegWrite, MemToReg;
        bitset<5> WriteReg;
        word ALUResult, ReadData;
    };
    struct In : public Reg {};
    struct Out : public Reg {};
};