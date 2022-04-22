#include <bitset>
#include "buffers.hpp"

using std::bitset;

#define bit bitset<1>
#define word bitset<32>

class IFStage
{
public:
    word WriteData;
    int nop;
    void fetch(HDUnit HDU, IDStage ID, word PC, InsMem IMem, IFIDBuff::In &IFID);
};

class IDStage
{
public:
    bit Branch, Jump;
    word BranchAddr, JumpAddr;
    void decode(IFIDBuff::Out IFID, WBStage WB, EXMEMBuff::In EX, EXMEMBuff::Out MEM, IDFUnit FU, RegFile Reg, IDEXBuff::In &IDEX);
};

class EXStage
{
public:
    void execute(IDEXBuff::Out IDEX, EXMEMBuff::Out MEM, WBStage WB, EXFUnit EXFU, EXMEMBuff::In &EXMEM);
};

class MEMStage
{
public:
    void memaccess(EXMEMBuff::Out EXMEM, DataMem DMem, MEMWBBuff::In &MEMWB);
};

class WBStage
{
public:
    bit RegWrite;
    word WriteData;
    void writeback(MEMWBBuff::Out MEMWB);
};