#pragma once

#include "structures.hpp"
#include "stages.hpp"

class CPU
{
    word PC;
    InsMem IMem;
    RegFile Reg;
    DataMem DMem;
    IFIDBuff IFID;
    IDStage ID;
    IDEXBuff IDEX;
    EXMEMBuff EXMEM;
    MEMWBBuff MEMWB;
    WBStage WB;
    FUnit FU;
    HDUnit HDU;
    int running, CC;
    bool stepmode, forwarding, printbasic, printimem, testingmode;
    struct Stages
    {
        string IF, ID, EX, MEM, WB;
    };
    Stages stages;
    int basetype, printpipeline, printregfile, printdmem, printbuffers;
    void clock();
    void stop();
    void print_cycle(const int basetype);
public:
    CPU(const string assembly, unsigned const int IMemSize, unsigned const int DMemSize, const bool step_mode, const int base_type, const bool forwarding, const bool print_basic, const bool print_imem, const int print_pipeline, const int print_regfile, const int print_dmem, const bool testing_mode, const int print_buffers);
	void run();
    int get_cycles();
};