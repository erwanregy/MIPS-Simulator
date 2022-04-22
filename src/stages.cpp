#include "stages.hpp"
#include "structures.hpp"

void IFStage::fetch(HDUnit HDU, IDStage ID, word PC, InsMem IMem, IFIDBuff::In &IFID)
{
    if (HDU.IFFlush == 0) 
    {
        IMem.Read(PC, IFID.Instruction); // Fetch Instruction
        if (IFID.Instruction == 0) nop++;
        else nop = 0;
    }
    else IFID.Instruction = 0;

    IFID.PCPlus4 = PC.to_ulong() + 4; // Increment PC

    // Update PC:
    if (HDU.PCWrite == 1)
    {
        PC = (ID.Branch == 1) ? ID.BranchAddr : IFID.PCPlus4;
        PC = (ID.Jump == 1) ? ID.JumpAddr : PC;
    }
}

void IDStage::decode(IFIDBuff::Out IFID, WBStage WB, EXMEMBuff::In EX, EXMEMBuff::Out MEM, IDFUnit IDFU, RegFile Reg, IDEXBuff::In &IDEX)
{
    bitset<6> Opcode = (IFID.Instruction.to_ulong() & 0b11111100000000000000000000000000) >> 26;

    bit BEQ, BNE, Jump;
    // Calculate control signals:
    if (Control == 0 && IFID.Instruction != 0) Control(Opcode, IDEX.RegWrite, IDEX.MemToReg, IDEX.MemWrite, IDEX.MemRead, BEQ, BNE, Jump, IDEX.RegDst, IDEX.ALUOp, IDEX.ALUSrc);
    else
    {
        IDEX.RegWrite = IDEX.MemToReg = IDEX.MemWrite = IDEX.MemRead = BEQ = BNE = Jump = IDEX.RegDst = IDEX.ALUSrc = 0;
        IDEX.ALUOp = 0;
    }

    // Extract immediate
    bitset<16> Imm16 = IFID.Instruction.to_ulong();
    // Sign extend
    if (Imm16.test(15)) IDEX.Imm = 0b11111111111111110000000000000000 | Imm16.to_ulong();
    else IDEX.Imm = Imm16.to_ulong();

    // Calculate branch address:
    BranchAddr = IFID.PCPlus4.to_ulong() + (IDEX.Imm << 2).to_ulong();

    // Extract and calculate jump address:
    JumpAddr = ((IFID.Instruction.to_ulong() & 0b00000011111111111111111111111111) << 2) | (IFID.PCPlus4.to_ulong() & 0b11111100000000000000000000000000);
    
    // Extract rs, rd, rt:
    IDEX.SourceReg = (IFID.Instruction.to_ulong() >> 21);
    IDEX.TargReg = (IFID.Instruction.to_ulong() >> 16);
    IDEX.DestReg = (IFID.Instruction.to_ulong() >> 11);

    // Read/Write:
    Reg.Read(IDEX.SourceReg, IDEX.TargReg, IDEX.ReadData1, IDEX.ReadData2); // Read from register
    if (WB.RegWrite == 1) Reg.Write(EX.WriteReg, WB.WriteData); // Write to register if RegWrite is on

    word CompareA, CompareB;

    // ForwardA MUX:
    if (IDFU.ForwardA == 0) CompareA = IDEX.ReadData1;
    else if (IDFU.ForwardA == 1) CompareA = EX.ALUResult;
    else if (IDFU.ForwardA == 2) CompareA = MEM.ALUResult;
    else
    {
        cerr << "Error: EX.ForwardA = 0b" << IDFU.ForwardA << " = " << IDFU.ForwardA.to_ulong() << ". Should only equal 0, 1, or 2";
        exit(1);
    }
    
    // ForwardB MUX:
    if (IDFU.ForwardB == 0) CompareB = IDEX.ReadData2;
    else if (IDFU.ForwardB == 1) CompareB = EX.ALUResult;
    else if (IDFU.ForwardB == 2) CompareB = MEM.ALUResult;
    else
    {
        cerr << "Error: EX.ForwardB = 0b" << IDFU.ForwardB << " = " << IDFU.ForwardB.to_ulong() << ". Should only equal 0, 1, or 2";
        exit(1);
    }

    // Evaluate Branch:
    bit Equal = ((CompareA.to_ulong() - CompareB.to_ulong()) == 0);
    Branch = (BEQ & Equal) | (BNE & Equal.flip());    
}

void EXStage::execute(IDEXBuff::Out IDEX, EXMEMBuff::Out MEM, WBStage WB, EXFUnit EXFU, EXMEMBuff::In &EXMEM)
{
    // ForwardA MUX
    word InputA;
    if (EXFU.ForwardA == 0) InputA = IDEX.ReadData1;
    else if (EXFU.ForwardA == 1) InputA = WB.WriteData;
    else if (EXFU.ForwardA == 2) InputA = MEM.ALUResult;
    else
    {
        cerr << "Error: EX.ForwardA = 0b" << EXFU.ForwardA << " = " << EXFU.ForwardA.to_ulong() << ". Should only equal 0, 1, or 2";
        exit(1);
    }

    // ForwardB MUX
    word WriteData;
    if (EXFU.ForwardB == 0) WriteData = IDEX.ReadData2;
    else if (EXFU.ForwardB == 1) WriteData = WB.WriteData;
    else if (EXFU.ForwardB == 2) WriteData = MEM.ALUResult;
    else
    {
        cerr << "Error: EX.ForwardB = 0b" << EXFU.ForwardB << " = " << EXFU.ForwardB.to_ulong() << ". Should only equal 0, 1, or 2";
        exit(1);
    }
    
    // ALUSrc MUX
    word InputB;
    InputB = (IDEX.ALUSrc == 1) ? IDEX.Imm : WriteData;

    // Extract funct and shamt
    bitset<6> funct = IDEX.Imm.to_ulong() & 0b00000000000000000000000000111111;
    bitset<5> shamt = (IDEX.Imm.to_ulong() & 0b00000000000000000000011111000000) >> 6;

    // ALU Control
    bitset<6> ALUFunct;
    ALUControl(IDEX.ALUOp, funct, ALUFunct);

    // ALU
    word ALUResult;
    ALU(ALUFunct, InputA, InputB, shamt, ALUResult);

    // RegDst MUX
    EXMEM.WriteReg = (IDEX.RegDst == 1) ? IDEX.DestReg : IDEX.TargReg;
}

void MEMStage::memaccess(EXMEMBuff::Out EXMEM, DataMem DMem, MEMWBBuff::In &MEMWB)
{
    // Read
    if (EXMEM.MemRead == 1) DMem.Read(EXMEM.ALUResult, MEMWB.ReadData);
    // Write
    if (EXMEM.MemWrite == 1) DMem.Write(EXMEM.ALUResult, EXMEM.WriteData);
}

void WBStage::writeback(MEMWBBuff::Out MEMWB)
{
    // MemToReg MUX
    WriteData = (MEMWB.MemToReg == 1) ? MEMWB.ReadData : MEMWB.ALUResult;
}