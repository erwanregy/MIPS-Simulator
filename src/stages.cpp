#include "stages.hpp"
#include "structures.hpp"
#include "buffers.hpp"

void WBStage::Initialise()
{
    WriteData = 0;
}

void IDStage::Initialise()
{
    Branch = Jump = 0;
    BranchAddr = JumpAddr = 0;
}

void stall(IDEXBuff::Reg IDEX, EXMEMBuff::Reg EXMEM, HDUnit& HDU)
{
    // if (ID.Branch == 1) HDU.IFFlush = 1;
    // else HDU.IFFlush = 0;
    if ((IDEX.MemRead == 1) && ((IDEX.TargReg == IDEX.SourceReg) || (EXMEM.WriteReg == IDEX.TargReg)))
    {
        HDU.PCWrite = HDU.IFIDWrite = 0;
        HDU.Control = 1;
    }
    else
    {
        HDU.PCWrite = HDU.IFIDWrite = 1;
        HDU.Control = 0;
    }
}

void forward(IDEXBuff::Reg IDEX, EXMEMBuff::Reg EXMEM, MEMWBBuff::Reg MEMWB, FUnit& FU)
{
    // ID Stage
    // ForwardA
    if (EXMEM.WriteReg != 0 && IDEX.SourceReg == EXMEM.WriteReg) FU.ID.ForwardA = 2; // EX/MEM hazard
    else if (MEMWB.WriteReg != 0 && IDEX.SourceReg == MEMWB.WriteReg) FU.ID.ForwardA = 1; // MEM/WB hazard
    else FU.ID.ForwardA = 0;
    // ForwardB
    if (EXMEM.WriteReg != 0 && IDEX.TargReg == EXMEM.WriteReg) FU.ID.ForwardB = 2; // EX/MEM hazard
    else if (MEMWB.WriteReg != 0 && IDEX.TargReg == MEMWB.WriteReg) FU.ID.ForwardB = 1; // MEM/WB hazard
    else FU.ID.ForwardB = 0;

    // EX Stage
    // ForwardA
    if (EXMEM.RegWrite == 1 && EXMEM.WriteReg != 0 && IDEX.SourceReg == EXMEM.WriteReg) FU.EX.ForwardA = 2; // EX/MEM hazard
    else if (MEMWB.RegWrite == 1 && MEMWB.WriteReg != 0 && IDEX.SourceReg == MEMWB.WriteReg) FU.EX.ForwardA = 1; // MEM/WB hazard
    else FU.EX.ForwardA = 0;
    // ForwardB
    if (EXMEM.RegWrite == 1 && EXMEM.WriteReg != 0 && IDEX.TargReg == EXMEM.WriteReg) FU.EX.ForwardB = 2; // E/ EX/MEM hazard
    else if (MEMWB.RegWrite == 1 && MEMWB.WriteReg != 0 && IDEX.TargReg == MEMWB.WriteReg) FU.EX.ForwardB = 1; // MEM/WB hazard
    else FU.EX.ForwardB = 0;
}

void fetch(const HDUnit HDU, const IDStage ID, word& PC, InsMem& IMem, IFIDBuff::Reg& IFID)
{
    if (HDU.IFFlush == 0) IMem.Read(PC, IFID.Instruction); // Fetch Instruction
    else IFID.Instruction = 0;

    IFID.PCPlus4 = PC.to_ulong() + 4; // Incremented PC

    // Update PC:
    if (HDU.PCWrite == 1)
    {
        PC = (ID.Branch == 1) ? ID.BranchAddr : IFID.PCPlus4;
        PC = (ID.Jump == 1) ? ID.JumpAddr : PC;
    }
}

void decode(const IFIDBuff::Reg IFID, const HDUnit HDU, const MEMWBBuff::Reg MEMWB, const WBStage WB, const EXMEMBuff::Reg EX, const EXMEMBuff::Reg MEM, const FUnit::Forward FU, RegFile& Reg, IDStage& ID, IDEXBuff::Reg& IDEX)
{
    bitset<6> Opcode = (IFID.Instruction.to_ulong() & 0b11111100000000000000000000000000) >> 26;

    bit BEQ, BNE;
    // Calculate control signals
    if (HDU.Control == 0 && IFID.Instruction != 0) Control(Opcode, IDEX.RegWrite, IDEX.MemToReg, IDEX.MemWrite, IDEX.MemRead, BEQ, BNE, ID.Jump, IDEX.RegDst, IDEX.ALUOp, IDEX.ALUSrc);
    else
    {
        IDEX.RegWrite = IDEX.MemToReg = IDEX.MemWrite = IDEX.MemRead = BEQ = BNE = ID.Jump = IDEX.RegDst = IDEX.ALUSrc = 0;
        IDEX.ALUOp = 0;
    }

    // Extract immediate
    bitset<16> Imm16 = IFID.Instruction.to_ulong();
    // Sign extend
    if (Imm16.test(15)) IDEX.Imm = 0b11111111111111110000000000000000 | Imm16.to_ulong();
    else IDEX.Imm = Imm16.to_ulong();

    // Calculate branch address
    ID.BranchAddr = IFID.PCPlus4.to_ulong() + (IDEX.Imm << 2).to_ulong();

    // Extract address
    bitset<26> address = IFID.Instruction.to_ulong() & 0b00000011111111111111111111111111;
    // Calculate jump address
    ID.JumpAddr = (address << 2).to_ulong() | (IFID.PCPlus4.to_ulong() & 0b11111100000000000000000000000000);
    
    // Extract rs, rd, rt
    IDEX.SourceReg = (IFID.Instruction.to_ulong() >> 21);
    IDEX.TargReg = (IFID.Instruction.to_ulong() >> 16);
    IDEX.DestReg = (IFID.Instruction.to_ulong() >> 11);
    
    // Read/Write
    Reg.Read(IDEX.SourceReg, IDEX.TargReg, IDEX.ReadData1, IDEX.ReadData2); // Read from register
    if (MEMWB.RegWrite == 1) Reg.Write(MEMWB.WriteReg, WB.WriteData); // Write to register if RegWrite is on

    word CompareA, CompareB;

    // ForwardA MUX
    if (FU.ForwardA == 0) CompareA = IDEX.ReadData1;
    else if (FU.ForwardA == 1) CompareA = EX.ALUResult;
    else /* FU.ForwardA == 2 */ CompareA = MEM.ALUResult;
    
    // ForwardB MUX
    if (FU.ForwardB == 0) CompareB = IDEX.ReadData2;
    else if (FU.ForwardB == 1) CompareB = EX.ALUResult;
    else /* FU.ForwardB == 2 */ CompareB = MEM.ALUResult;

    // Evaluate Branch
    bit Equal = (CompareA.to_ulong() - CompareB.to_ulong()) == 0;
    ID.Branch = (BEQ & Equal) | (BNE & Equal.flip());
}

void execute(const IDEXBuff::Reg IDEX, const EXMEMBuff::Reg MEM, const WBStage WB, const FUnit::Forward FU, EXMEMBuff::Reg& EXMEM)
{
    // Control signals
    EXMEM.RegWrite = IDEX.RegWrite;
    EXMEM.MemToReg = IDEX.MemToReg;
    EXMEM.MemWrite = IDEX.MemWrite;
    EXMEM.MemRead = IDEX.MemRead;

    // ForwardA MUX
    word InputA;
    if (FU.ForwardA == 0) InputA = IDEX.ReadData1;
    else if (FU.ForwardA == 1) InputA = WB.WriteData;
    else /* FU.ForwardA == 2 */ InputA = MEM.ALUResult;

    // ForwardB MUX
    if (FU.ForwardB == 0) EXMEM.WriteData = IDEX.ReadData2;
    else if (FU.ForwardB == 1) EXMEM.WriteData = WB.WriteData;
    else /* FU.ForwardB == 2 */ EXMEM.WriteData = MEM.ALUResult;
    
    // ALUSrc MUX
    word InputB;
    InputB = (IDEX.ALUSrc == 1) ? IDEX.Imm : EXMEM.WriteData;

    // Extract funct and shamt
    bitset<6> funct = IDEX.Imm.to_ulong() & 0b00000000000000000000000000111111;
    bitset<5> shamt = (IDEX.Imm.to_ulong() & 0b00000000000000000000011111000000) >> 6;

    // ALU Control
    bitset<6> ALUFunct;
    ALUControl(IDEX.ALUOp, funct, ALUFunct);

    // ALU
    ALU(ALUFunct, InputA, InputB, shamt, EXMEM.ALUResult);

    // RegDst MUX
    EXMEM.WriteReg = (IDEX.RegDst == 1) ? IDEX.DestReg : IDEX.TargReg;
}

void memaccess(const EXMEMBuff::Reg EXMEM, DataMem& DMem, MEMWBBuff::Reg& MEMWB)
{   
    // Control signals
    MEMWB.RegWrite = EXMEM.RegWrite;
    MEMWB.MemToReg = EXMEM.MemToReg;

    MEMWB.ALUResult = EXMEM.ALUResult;

    // Read/Write
    if (EXMEM.MemRead == 1) DMem.Read(EXMEM.ALUResult, MEMWB.ReadData);
    if (EXMEM.MemWrite == 1) DMem.Write(EXMEM.ALUResult, EXMEM.WriteData);

    MEMWB.WriteReg = EXMEM.WriteReg;
}

void writeback(const MEMWBBuff::Reg MEMWB, WBStage& WB)
{
    // MemToReg MUX
    WB.WriteData = (MEMWB.MemToReg == 1) ? MEMWB.ReadData : MEMWB.ALUResult;
}

void Control(const bitset<6> Opcode, bit &RegWrite, bit &MemToReg, bit &MemWrite, bit &MemRead, bit &BEQ, bit &BNE, bit &Jump, bit &RegDst, bitset<3> &ALUOp, bit &ALUSrc)
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

void ALUControl(const bitset<3> ALUOp, const bitset<6> funct, bitset<6> &ALUFunct)
{
    if (ALUOp == 2) ALUFunct = funct; // funct (RType)
    else if (ALUOp == 0) ALUFunct = 33; // addu (load/store type or addiu)
    else if (ALUOp == 1) ALUFunct = 35; // subu (branch type)
    else if (ALUOp == 3) ALUFunct = 43; // sltu (sltiu)
    else if (ALUOp == 4 || ALUOp == 5 || ALUOp == 6) ALUFunct = ALUOp.to_ulong() + 32; // and, or, xor (andi, ori, xori)
}

word operator+(const word input1, const word input2) // overload bitset<32> addition function
{
    word output; bool carry = 0;
    for (int i = 0; i < 32; i++)
    {
        output[i] = (input1[i] ^ input2[i]) ^ (int)carry;
        carry = (input1[i] == 1 && input2[i] == 1) || (input1[i] == 1 && carry == 1) || (input2[i] == 1 && carry == 1);
    }
    return output;
}

void ALU(const bitset<6> Funct, const word InputA, word InputB, const bitset<5> shamt, word &Result)
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