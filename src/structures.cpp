#include "structures.hpp"
#include "assembler.hpp"

word operator+(word input1, word input2) // overload bitset<32> addition function
{
    word output; bool carry = 0;
    for (int i = 0; i < 32; i++)
    {
        output[i] = (input1[i] ^ input2[i]) ^ carry;
        carry = (input1[i] == 1 && input2[i] == 1) || (input1[i] == 1 && carry == 1) || (input2[i] == 1 && carry == 1);
    }
    return output;
}

void RegFile::Initialise()
{
    for (int i = 0; i < sizeof(Register)/sizeof(Register[0]); i++) Register[i] = 0;
}
void RegFile::Write(bitset<5> WriteReg, word WriteData)
{
    if (WriteReg.to_ulong() > 1 && WriteReg.to_ulong() < 26) Register[WriteReg.to_ulong()] = WriteData;
    else
    {
        if (WriteReg == 0) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $zero. Forbidden, reserved for the value 0";
        else if (WriteReg == 1) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $at. Forbidden, reserved for pseudo-instructions";
        else if (WriteReg == 26) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $k0. Unsupported register";
        else if (WriteReg == 27) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $k1. Unsupported register";
        else if (WriteReg == 28) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $gp. Unsupported register";
        else if (WriteReg == 29) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $sp. Unsupported register";
        else if (WriteReg == 30) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $fp. Unsupported register";
        else if (WriteReg == 31) cerr << "Warning: Tried to write the value " << WriteData.to_ulong() << " to $ra. Unsupported register";
    }
}
void RegFile::Read(bitset<5> ReadReg1, bitset<5> ReadReg2, word &ReadData1, word &ReadData2)
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

void Mem::Initialise(int MemSize)
{
    Size = MemSize;
}
void Mem::Read(word Address, word &Data)
{
    if (Address.to_ulong() > Size);
    Data = Memory[(Address >> 2).to_ulong()];
}

void InsMem::Initialise(string assembly, int MemSize)
{
    Memory = new word[MemSize]; // Create memory
    for (int i = 0; i < MemSize; i++) Memory[i] = 0; // Clear memory
    assemble(assembly, basic, Memory, MemSize);
}

void DataMem::Write(word Address, word Data)
{
    if (Address.to_ulong() > Size);
    Memory[(Address >> 2).to_ulong()] = Data;
}

void Control(bitset<6> Opcode, bit &RegWrite, bit &MemToReg, bit &MemWrite, bit &MemRead, bit &BEQ, bit &BNE, bit &Jump, bit &RegDst, bitset<3> &ALUOp, bit &ALUSrc)
{
    RegWrite = !(Opcode == 2 || Opcode == 4 || Opcode == 5 || Opcode == 43); // Not jump, branch, or store type
    MemToReg = (Opcode == 43); // Store type
    MemWrite = (Opcode == 43); // Store type
    MemRead = (Opcode == 35); // Load type
    BEQ = (Opcode == 4); // beq
    BNE = (Opcode == 5); // bne
    Jump = (Opcode == 2); // JType
    RegDst = (Opcode == 0); // RType
    if (Opcode == 0) ALUOp = 2; // RType - ALUfunc = funct
    else if (Opcode == 4 || Opcode == 5) ALUOp = 1; // Branch type - ALUfunc = subu
    else if (Opcode == 9 || Opcode == 35 || Opcode == 43) ALUOp = 0; // addiu or load/store type  - ALUfunc = addu
    else ALUOp = Opcode.to_ulong() - 8; // IType - ALUfunc = Opcode - 8
    ALUSrc = (Opcode.to_ulong() >= 9); // Imm?
}

void ALUControl(bitset<3> ALUOp, bitset<6> funct, bitset<6> &ALUFunct)
{
    if (ALUOp == 2) ALUFunct = funct; // funct (RType)
    else if (ALUOp == 0) ALUFunct = 33; // addu (load/store type or addiu)
    else if (ALUOp == 1) ALUFunct = 35; // subu (branch type)
    else if (ALUOp == 3) ALUFunct = 43; // sltu (sltiu)
    else if (ALUOp == 4 || ALUOp == 5 || ALUOp == 6) ALUFunct = ALUOp.to_ulong() + 32; // and, or, xor (andi, ori, xori)
}

void ALU(bitset<6> Funct, word InputA, word InputB, bitset<5> shamt, word &Result)
{
    if (Funct == 0) Result = InputB.to_ulong() << shamt.to_ulong(); // sll
    else if (Funct == 3) Result = InputB.to_ulong() >> shamt.to_ulong(); // srl
    else if (Funct == 33) Result = InputA + InputB; // addu
    else if (Funct == 35) Result = InputA + InputB.flip() + 1; // subu
    else if (Funct == 36) Result = InputA & InputB; // and
    else if (Funct == 37) Result = InputA | InputB; // or
    else if (Funct == 38) Result = InputA ^ InputB; // xor
    else if (Funct == 39) Result = (InputA | InputB).flip(); // nor
    else if (Funct == 43) Result = InputA.to_ulong() < InputB.to_ulong(); // sltu
}

void FUnit::Initialise()
{
    ForwardA = ForwardB = 0;
}

void IDFUnit::Forward(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM, MEMWBBuff::Out MEMWB)
{
    // ForwardA:
    if (IDEX.SourceReg == EXMEM.WriteReg) ForwardA = 2; // EX/MEM hazard
    else if (IDEX.SourceReg == MEMWB.WriteReg) ForwardA = 1; // MEM/WB hazard
    else ForwardA = 0;
    // ForwardB:
    if (IDEX.TargReg == EXMEM.WriteReg) ForwardB = 2; // EX/MEM hazard
    else if (IDEX.TargReg == MEMWB.WriteReg) ForwardB = 1; // MEM/WB hazard
    else ForwardB = 0;
}

void EXFUnit::Forward(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM, MEMWBBuff::Out MEMWB)
{
    // ForwardA:
    if (EXMEM.RegWrite == 1 && EXMEM.WriteReg != 0 && IDEX.SourceReg == EXMEM.WriteReg) ForwardA = 2; // EX/MEM hazard
    else if (MEMWB.RegWrite == 1 && MEMWB.WriteReg != 0 && IDEX.SourceReg == MEMWB.WriteReg) ForwardA = 1; // MEM/WB hazard
    else ForwardA = 0;
    // ForwardB:
    if (EXMEM.RegWrite == 1 && EXMEM.WriteReg != 0 && IDEX.TargReg == EXMEM.WriteReg) ForwardB = 2; // E/ EX/MEM hazard
    else if (MEMWB.RegWrite == 1 && MEMWB.WriteReg != 0 && IDEX.TargReg == MEMWB.WriteReg) ForwardB = 1; // MEM/WB hazard
    else ForwardB = 0;
}

void HDUnit::HazardDetect(IDEXBuff::Out IDEX, EXMEMBuff::Out EXMEM)
{
    // if (ID_Branch == 1) IFFlush = 1;
    // else IFFlush = 0;
    if ((IDEX.MemRead == 1) && ((IDEX.TargReg == IDEX.SourceReg) || (EXMEM.WriteReg == IDEX.TargReg)))
    {
        PCWrite = IFIDWrite = 0;
        Control = 1;
    }
    else
    {
        PCWrite = IFIDWrite = 1;
        Control = 0;
    }
}