#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "cpu.hpp"

using std::filesystem::create_directory, std::stringstream, std::cout, std::ofstream, std::hex, std::cin;

CPU::CPU(const string assembly, unsigned const int IMemSize, unsigned const int DMemSize, const bool step_mode, const int base_type, const bool forwarding, const bool print_basic, const bool print_imem, const int print_pipeline, const int print_regfile, const int print_dmem, const bool testing_mode, const int print_buffers)
    : stepmode(step_mode), basetype(base_type), forwarding(forwarding), printbasic(print_basic), printimem(print_imem), printpipeline(print_pipeline), printregfile(print_regfile), printdmem(print_dmem), testingmode(testing_mode), printbuffers(print_buffers)
{
    create_directory("output");
    ofstream clear;
    clear.open("output/Stages.txt");
    clear.open("output/Register File.txt");
    clear.open("output/Data Memory.txt");
    PC = 0;
    if (!testingmode) IMem.Initialise(assembly, IMemSize, print_basic, print_imem, base_type);
    else IMem.Initialise("tests/li.asm", IMemSize, print_basic, print_imem, base_type);
    Reg.Initialise();
    DMem.Initialise(DMemSize);
    IFID.Initialise();
    ID.Initialise();
    IDEX.Initialise();
    EXMEM.Initialise();
    MEMWB.Initialise();
    WB.Initialise();
    FU.Initialise();
    HDU.Initialise();
    CC = 1;
    running = 1;
    stages.IF = IMem.basic[PC.to_ulong()/4];
    stages.ID = stages.EX = stages.MEM = stages.WB = "";
}

void CPU::clock()
{
    MEMWB.Clock();
    EXMEM.Clock();
    IDEX.Clock();
    if (HDU.IFIDWrite == 1) IFID.Clock();
    CC++;
    stages.WB = stages.MEM;
    stages.MEM = stages.EX;
    stages.EX = stages.ID;
    stages.ID = stages.IF;
    if (PC.to_ulong()/4 < IMem.basic.size()) stages.IF = IMem.basic[PC.to_ulong()/4];
    else stages.IF = "";
}

void CPU::stop()
{
    running = 0;
}

void print(const string input, const int style, const string output)
{
    if (style == 1)
    {
        ofstream outfile("output/"+output);
        outfile << input;
    }
    else if (style > 1)
    {
        cout << "\x1B[2J\x1B[H" << input;
        if (style == 3)
        {
            ofstream outfile("output/"+output);
            outfile << input;
        }
    }
    
    // 0 neither
    // 1 file
    // 2 console
    // 3 console and file
}

void CPU::print_cycle(const int basetype)
{
    if (printpipeline > 0)
    {
        // Pipeline
        stringstream pipe;
        pipe << "Clock Cycle " << CC << '\n'
        << '\n'
        << " Stage  |       Instruction       \n"
        << "--------+-------------------------\n"
        << "   IF   |   "; if (PC.to_ulong() > 0) pipe << stages.IF; pipe << '\n'
        << "--------+-------------------------\n"
        << "   ID   |   "; if (PC.to_ulong() > 4) pipe << stages.ID; pipe << '\n'
        << "--------+-------------------------\n"
        << "   EX   |   "; if (PC.to_ulong() > 8) pipe << stages.EX; pipe << '\n'
        << "--------+-------------------------\n"
        << "  MEM   |   "; if (PC.to_ulong() > 12) pipe << stages.MEM; pipe << '\n'
        << "--------+-------------------------\n"
        << "   WB   |   "; if (PC.to_ulong() > 16) pipe << stages.WB; pipe << '\n';
        string pipdata = pipe.str();
        string pipefile = "Pipeline.txt";
        print(pipdata, printpipeline, pipefile);
    }

    if (printregfile > 0)
    {
        // Register File
        stringstream reg;
        reg << "Clock Cycle " << CC << '\n'
        << '\n'
        << "  Name |   Num   |  Val  " << '\n'
        << "-------+---------+-------" << '\n'
        << " $zero |   $0    |"; if (Reg.Register[0].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[0].to_ulong() << '\n'
        << "  $at  |   $1    |"; if (Reg.Register[1].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[1].to_ulong() << '\n'
        << "  $v0  |   $2    |"; if (Reg.Register[2].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[2].to_ulong() << '\n'
        << "  $v1  |   $3    |"; if (Reg.Register[3].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[3].to_ulong() << '\n'
        << "  $a0  |   $4    |"; if (Reg.Register[4].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[4].to_ulong() << '\n'
        << "  $a1  |   $5    |"; if (Reg.Register[5].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[5].to_ulong() << '\n'
        << "  $a2  |   $6    |"; if (Reg.Register[6].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[6].to_ulong() << '\n'
        << "  $a3  |   $7    |"; if (Reg.Register[7].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[7].to_ulong() << '\n'
        << "  $t0  |   $8    |"; if (Reg.Register[8].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[8].to_ulong() << '\n'
        << "  $t1  |   $9    |"; if (Reg.Register[9].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[9].to_ulong() << '\n'
        << "  $t2  |   $10   |"; if (Reg.Register[10].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[10].to_ulong() << '\n'
        << "  $t3  |   $11   |"; if (Reg.Register[11].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[11].to_ulong() << '\n'
        << "  $t4  |   $12   |"; if (Reg.Register[12].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[12].to_ulong() << '\n'
        << "  $t5  |   $13   |"; if (Reg.Register[13].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[13].to_ulong() << '\n'
        << "  $t6  |   $14   |"; if (Reg.Register[14].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[14].to_ulong() << '\n'
        << "  $t7  |   $15   |"; if (Reg.Register[15].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[15].to_ulong() << '\n'
        << "  $s0  |   $16   |"; if (Reg.Register[16].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[16].to_ulong() << '\n'
        << "  $s1  |   $17   |"; if (Reg.Register[17].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[17].to_ulong() << '\n'
        << "  $s2  |   $18   |"; if (Reg.Register[18].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[18].to_ulong() << '\n'
        << "  $s3  |   $19   |"; if (Reg.Register[19].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[19].to_ulong() << '\n'
        << "  $s4  |   $20   |"; if (Reg.Register[20].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[20].to_ulong() << '\n'
        << "  $s5  |   $21   |"; if (Reg.Register[21].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[21].to_ulong() << '\n'
        << "  $s6  |   $22   |"; if (Reg.Register[22].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[22].to_ulong() << '\n'
        << "  $s7  |   $23   |"; if (Reg.Register[23].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[23].to_ulong() << '\n'
        << "  $t8  |   $24   |"; if (Reg.Register[24].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[24].to_ulong() << '\n'
        << "  $t9  |   $25   |"; if (Reg.Register[25].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[25].to_ulong() << '\n'
        << "  $k0  |   $26   |"; if (Reg.Register[26].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[26].to_ulong() << '\n'
        << "  $k1  |   $27   |"; if (Reg.Register[27].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[27].to_ulong() << '\n'
        << "  $gp  |   $28   |"; if (Reg.Register[28].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[28].to_ulong() << '\n'
        << "  $sp  |   $29   |"; if (Reg.Register[29].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[29].to_ulong() << '\n'
        << "  $fp  |   $30   |"; if (Reg.Register[30].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[30].to_ulong() << '\n'
        << "  $ra  |   $31   |"; if (Reg.Register[31].to_ulong() < 100) reg << ' '; reg << "  " << (int)Reg.Register[31].to_ulong() << '\n'
        << "   pc  |         |"; if (PC.to_ulong() < 100) reg << ' '; reg << "  " << PC.to_ulong() << '\n';
        string regdata = reg.str();
        string regfile = "Register File.txt";
        print(regdata, printregfile, regfile);
    }

    if (printdmem > 0)
    {        
        // Data Memory
        stringstream dmem;
        dmem << "Clock Cycle " << CC << '\n'
        << '\n';
        if (basetype == 1)
        {
            dmem << "  Addr  |  Val  \n"
                    "--------+-------\n";
            for (int i = 0; i < DMem.MemSize; i++) { dmem << " "; if (i < 10) dmem << "   "; else if (i < 100) dmem << "  "; else if (i < 1000) dmem << ' '; dmem << i << "   |   "; dmem << int(DMem.Memory[i].to_ulong()) << '\n'; }
        }
        else if (basetype == 2) for (int i = 0; i < DMem.MemSize; i++)
        {
            dmem << "0x" << hex << DMem.Memory[i].to_ulong() << '\t';
            if (!((i + 1) % 8)) dmem << '\n';
        }
        else for (int i = 0; i < DMem.MemSize; i++)
        {
            dmem << DMem.Memory[i] << '\t';
            if (!((i + 1) % 8)) dmem << '\n';
        }
        string dmemdata = dmem.str();
        string dmemfile = "Data Memory.txt";
        print(dmemdata, printdmem, dmemfile);
    }

    if (printbuffers > 0)
    {
        stringstream buff;
        buff << "Clock Cycle " << CC << '\n'
        << '\n'
        << "IF/ID\n"
        << "Instruction " << IFID.In.Instruction.to_ulong() << '\n'
        << "PCPlus4 " << IFID.In.PCPlus4.to_ulong() << '\n'
        << '\n'
        << "ID/EX\n"
        << "ALUOp " << IDEX.In.ALUOp.to_ulong() << '\n'
        << "ALUSrc " << IDEX.In.ALUSrc.to_ulong() << '\n'
        << "DestReg " << IDEX.In.DestReg.to_ulong() << '\n'
        << "Imm " << IDEX.In.Imm.to_ulong() << '\n'
        << "MemRead " << IDEX.In.MemRead.to_ulong() << '\n'
        << "MemWrite " << IDEX.In.MemWrite.to_ulong() << '\n'
        << "ReadData1 " << IDEX.In.ReadData1.to_ulong() << '\n'
        << "ReadData2 " << IDEX.In.ReadData2.to_ulong() << '\n'
        << "RegDst " << IDEX.In.RegDst.to_ulong() << '\n'
        << "RegWrite " << IDEX.In.RegWrite.to_ulong() << '\n'
        << "SourceReg " << IDEX.In.SourceReg.to_ulong() << '\n'
        << "TargReg " << IDEX.In.TargReg.to_ulong() << '\n'
        << '\n'
        << "EX/MEM\n"
        << "ALUResult " << EXMEM.In.ALUResult.to_ulong() << '\n'
        << "MemRead " << EXMEM.In.MemRead.to_ulong() << '\n'
        << "MemToReg " << EXMEM.In.MemToReg.to_ulong() << '\n'
        << "MemWrite " << EXMEM.In.MemWrite.to_ulong() << '\n'
        << "RegWrite " << EXMEM.In.RegWrite.to_ulong() << '\n'
        << "WriteData " << EXMEM.In.WriteData.to_ulong() << '\n'
        << "WriteReg " << EXMEM.In.WriteReg.to_ulong() << '\n'
        << '\n'
        << "MEM/WB\n"
        << "ALUResult " << MEMWB.In.ALUResult.to_ulong() << '\n'
        << "ALUResult " << MEMWB.In.MemToReg.to_ulong() << '\n'
        << "ReadData " << MEMWB.In.ReadData.to_ulong() << '\n'
        << "RegWrite " << MEMWB.In.RegWrite.to_ulong() << '\n'
        << "WriteReg " << MEMWB.In.WriteReg.to_ulong() << '\n';
        string buffdata = buff.str();
        string bufffile = "Buffers.txt";
        print(buffdata, printdmem, bufffile);
    }
}

void CPU::run()
{
    while(running)
    {
        if (stepmode) cin.get(); // wait for user input (enter) if in step mode
        stall(IDEX.Out, EXMEM.Out, HDU); // detect any load-use hazards and stall accordingly
        if (forwarding) forward(IDEX.Out, EXMEM.Out, MEMWB.Out, FU); // detect any data hazards and forward values accordingly
        writeback(MEMWB.Out, WB); // write-back memory or ALU data to registers
        memaccess(EXMEM.Out, DMem, MEMWB.In); // read/write from/to data memory
        execute(IDEX.Out, EXMEM.Out, WB, FU.EX, EXMEM.In); // execute ALU operations
        decode(IFID.Out, HDU, MEMWB.Out, WB, EXMEM.Out, EXMEM.In, FU.ID, Reg, ID, IDEX.In); // decode instruction, calculate control signals, calculate jump/branch addresses, fetch data from registers
        fetch(HDU, ID, PC, IMem, IFID.In); // fetch instruction from memory
        print_cycle(basetype); // print various internal data from cycle
        if (PC.to_ulong() == IMem.PCMax + 16) stop(); // stop running if last instruction has been completed
        else clock(); // update stage buffers
    }
}

int CPU::get_cycles()
{
    return CC;
}
