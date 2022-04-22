#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "structures.hpp"

using std::cin, std::cout, std::cerr, std::endl, std::string, std::ofstream, std::stringstream, std::hex, std::filesystem::create_directories;

#define IMEMSIZE 16
#define DMEMSIZE 256

void Initialise(InsMem &IMem, RegFile &Reg, DataMem &DMem, IFIDBuff &IFID, IDEXBuff &IDEX, EXMEMBuff &EXMEM, MEMWBBuff &MEMWB, IDFUnit &IDFU, EXFUnit &EXFU, HDUnit &HDU)
{
    create_directories("output/");
    ofstream outfile;
    outfile.open("output/Stages History.txt");
    outfile.open("output/Register File.txt"),
    outfile.open("output/Data Memory.txt");
    outfile.close();
    cout << "Please enter name of the file to read assembly code from: ";
    string filename; cin >> filename;
    IMem.Initialise(filename, IMEMSIZE);
    Reg.Initialise();
    DMem.Initialise(DMEMSIZE);
    IFID.Initialise();
    IDEX.Initialise();
    EXMEM.Initialise();
    MEMWB.Initialise();
    IDFU.Initialise();
    EXFU.Initialise();
    HDU.Initialise();
}

void Clock(MEMWBBuff MEMWB, EXMEMBuff EXMEM, IDEXBuff IDEX, HDUnit HDU, IFIDBuff IFID)
{
    MEMWB.Clock();
    EXMEM.Clock();
    IDEX.Clock();
    if (HDU.IFIDWrite == 1) IFID.Clock();
}

void Configure(int &stagestyle, int &regstyle, int &basetype)
{
    cout << "Print stages to file, console, both, or neither? (0/1/2/3): ";
    while (1)
    {
        // cin >> stagestyle;
        stagestyle = 0;
        if (stagestyle >= 0 && stagestyle <= 3) break;
        else cerr << "Error: Input can only be 0, 1, 2, or 3. Please try again: ";
    }
    cout << "Print register file to file, console, both, or neither? (0/1/2/3): ";
    while (1)
    {
        // cin >> regstyle;
        regstyle = 0;
        if (regstyle >= 0 && regstyle <= 3) break;
        else cerr << "Error: Input can only be 0, 1, 2, or 3. Please try again: ";
    }
    cout << "Print values in binary, decimal, or hexadecimal? (0/1/2): ";
    while (1)
    {
        // cin >> basetype;
        basetype = 1;
        // cin.get();
        if (basetype >= 0 && basetype <= 2) break;
        else cerr << "Error: Input can only be 0, 1, or 2. Please try again: ";
    }
}

/* void PrintCycle(int CC, word PC, InsMem IMem, RegFile Reg, DataMem DMem, IFIDBuff IFID, IDEXBuff IDEX, EXMEMBuff EXMEM, MEMWBBuff MEMWB, IDFUnit IDFU, EXFUnit EXFU, HDUnit HDU, int style, int basetype)
{
    if (style != 3)
    {
        stringstream buffer;
        ofstream outfile;
        if (CC == 1) outfile.open("output/Stages History.txt");
        else outfile.open("output/Stages History.txt", stringstream::app);
        if ((regstyle == 1 || regstyle == 2) && (style || CC > 1)) cout << "================================================================" << endl
        << endl;
        else if (style || CC > 1) cout << "----------------------------------------------------------------" << endl
        << endl;
        if (!style && CC > 1) outfile << "----------------------------------------------------------------" << endl
        << endl;
        buffer << "Clock Cycle " << CC << endl
        << endl
        << "PC = " << PC.to_ulong() << endl
        << endl
        << "IF Stage" << endl
        << "PCPlus4 = " << IF.PCPlus4.to_ulong() << endl
        << "Instruction = " << IF.Instruction << " = "; if (IF.Instruction == 0) buffer << "nop"; else buffer << IMem.basic[PC.to_ulong()/4]; buffer << endl
        << endl
        << "ID Stage" << endl
        << "PCPlus4 = " << ID.PCPlus4.to_ulong() << endl
        << "Instruction = " << ID.Instruction << " = "; if (ID.Instruction == 0) buffer << "nop"; else buffer << IMem.basic[PC.to_ulong()/4 - 1];
        buffer << endl
        << "RegReadData1 = " << ID.RegReadData1 << " = " << int(ID.RegReadData1.to_ulong()) << endl
        << "RegReadData2 = " << ID.RegReadData2 << " = " << int(ID.RegReadData2.to_ulong()) << endl
        << endl;
        if (style)
        {
            cout << buffer.str();
            if (style == 2) outfile << buffer.str();
        }
        else outfile << buffer.str();
    }

    
    if (style != 3)
    { 
        stringstream buffer;
        ofstream outfile("output/Register File.txt");
        if ((stagestyle == 1 || stagestyle == 2) && (style || CC > 1)) cout << "-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -" << endl
        << endl;
        else if (style && CC > 1) cout << "----------------------------------------------------------------" << endl
        << endl;
        if (style && (stagestyle == 0 || stagestyle == 3)) cout << "Clock Cycle " << CC << endl
        << endl;
        if (style && CC > 1) outfile << "----------------------------------------------------------------" << endl
        << endl;
        if (style) outfile << endl
        << "Clock Cycle " << CC << endl
        << endl;
        buffer
        << "  Name |   Num   |  Val  " << endl
        << "-------+---------+-------" << endl
        << " $zero |   $0    |"; if (Reg.Register[0].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[0].to_ulong() << endl
        << "  $at  |   $1    |"; if (Reg.Register[1].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[1].to_ulong() << endl
        << "  $v0  |   $2    |"; if (Reg.Register[2].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[2].to_ulong() << endl
        << "  $v1  |   $3    |"; if (Reg.Register[3].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[3].to_ulong() << endl
        << "  $a0  |   $4    |"; if (Reg.Register[4].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[4].to_ulong() << endl
        << "  $a1  |   $5    |"; if (Reg.Register[5].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[5].to_ulong() << endl
        << "  $a2  |   $6    |"; if (Reg.Register[6].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[6].to_ulong() << endl
        << "  $a3  |   $7    |"; if (Reg.Register[7].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[7].to_ulong() << endl
        << "  $t0  |   $8    |"; if (Reg.Register[8].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[8].to_ulong() << endl
        << "  $t1  |   $9    |"; if (Reg.Register[9].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[9].to_ulong() << endl
        << "  $t2  |   $10   |"; if (Reg.Register[10].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[10].to_ulong() << endl
        << "  $t3  |   $11   |"; if (Reg.Register[11].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[11].to_ulong() << endl
        << "  $t4  |   $12   |"; if (Reg.Register[12].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[12].to_ulong() << endl
        << "  $t5  |   $13   |"; if (Reg.Register[13].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[13].to_ulong() << endl
        << "  $t6  |   $14   |"; if (Reg.Register[14].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[14].to_ulong() << endl
        << "  $t7  |   $15   |"; if (Reg.Register[15].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[15].to_ulong() << endl
        << "  $s0  |   $16   |"; if (Reg.Register[16].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[16].to_ulong() << endl
        << "  $s1  |   $17   |"; if (Reg.Register[17].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[17].to_ulong() << endl
        << "  $s2  |   $18   |"; if (Reg.Register[18].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[18].to_ulong() << endl
        << "  $s3  |   $19   |"; if (Reg.Register[19].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[19].to_ulong() << endl
        << "  $s4  |   $20   |"; if (Reg.Register[20].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[20].to_ulong() << endl
        << "  $s5  |   $21   |"; if (Reg.Register[21].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[21].to_ulong() << endl
        << "  $s6  |   $22   |"; if (Reg.Register[22].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[22].to_ulong() << endl
        << "  $s7  |   $23   |"; if (Reg.Register[23].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[23].to_ulong() << endl
        << "  $t8  |   $24   |"; if (Reg.Register[24].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[24].to_ulong() << endl
        << "  $t9  |   $25   |"; if (Reg.Register[25].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[25].to_ulong() << endl
        << "  $k0  |   $26   |"; if (Reg.Register[26].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[26].to_ulong() << endl
        << "  $k1  |   $27   |"; if (Reg.Register[27].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[27].to_ulong() << endl
        << "  $gp  |   $28   |"; if (Reg.Register[28].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[28].to_ulong() << endl
        << "  $sp  |   $29   |"; if (Reg.Register[29].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[29].to_ulong() << endl
        << "  $fp  |   $30   |"; if (Reg.Register[30].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[30].to_ulong() << endl
        << "  $ra  |   $31   |"; if (Reg.Register[31].to_ulong() < 10) buffer << ' '; buffer << "  " << (int)Reg.Register[31].to_ulong() << endl
        << "   pc  |         |"; if (PC.to_ulong() < 10) buffer << ' '; buffer << "  " << PC.to_ulong() << endl;
        if (style)
        {
            cout << buffer.str();
            if (style == 2) outfile << buffer.str() << endl;
        }
        else outfile << buffer.str();
    }

    ofstream outfile("output/Data Memory.txt");
    if (basetype == 1) for (int i = 0; i < DMEMSIZE; i++)
    {
        outfile << int(DMem.Memory[i].to_ulong()) << endl;
    }
    else if (basetype == 2) for (int i = 0; i < DMEMSIZE; i++)
    {
        outfile << "0x" << hex << DMem.Memory[i].to_ulong() << '\t';
        if (!((i + 1) % 8)) outfile << endl;
    }
    else for (int i = 0; i < DMEMSIZE; i++)
    {
        outfile << DMem.Memory[i] << '\t';
        if (!((i + 1) % 8)) outfile << endl;
    } 
} */

int main(int argc, char *argv[])
{
    word PC = 0;
    IFStage IF;
    InsMem IMem;
    IFIDBuff IFID;
    IDStage ID;
    RegFile Reg;
    IDEXBuff IDEX;
    EXStage EX;
    EXMEMBuff EXMEM;
    MEMStage MEM;
    DataMem DMem;
    MEMWBBuff MEMWB;
    WBStage WB;
    IDFUnit IDFU;
    EXFUnit EXFU;
    HDUnit HDU;
    int CC = 1;
    bool running = 1;
    Initialise(IMem, Reg, DMem, IFID, IDEX, EXMEM, MEMWB, IDFU, EXFU, HDU);

    // int stagestyle, regstyle, basetype;
    // Configure(stagestyle, regstyle, basetype);

    while (running)
    {
        // cin.get(); // Wait for user input (enter)
        word PCCycle = PC;
        // HDU.HazardDetect(IDEX, EXMEM);
        // IDFU.run(ID.SourceReg, ID.TargReg, EX.WriteReg, MEM.WriteReg);
        // EXFU.run(EX.SourceReg, EX.TargReg, MEM.RegWrite, MEM.WriteReg, WB.RegWrite, WB.WriteReg);
        WB.writeback(MEMWB::Out);
        MEM.memaccess();
        EX.execute();
        ID.decode();
        IF.fetch();
        EX.run(WB.RegWriteData, MEM.ALUResult, EXFU.ForwardA, EXFU.ForwardB);
        ID.run(Reg, HDU.Control, IDFU.ForwardA, IDFU.ForwardB, MEM.ALUResult, EX.ALUResult, WB.RegWrite, WB.RegWriteData, WB.WriteReg);
        IF.run(PC, IMem, HDU.PCWrite, ID.Branch, ID.BranchAddr, ID.Jump, ID.JumpAddr, HDU.IFFlush, running);
        // PrintStages(CC, PCCycle, IMem, Reg, DMem, IF, ID, EX, MEM, WB, regstyle, stagestyle, basetype);
        PrintReg(CC, PCCycle, Reg, stagestyle, regstyle, basetype);
        PrintDMem(DMem, basetype);
        RisingClockEdge(IF, HDU.IDWrite, ID, EX, MEM, WB, CC);
    }

    cout << "Total number of clock cycles = " << CC;
    cin.get();

    return 0;
}