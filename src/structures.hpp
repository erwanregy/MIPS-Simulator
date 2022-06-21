#pragma once

#include <bitset>
#include <string>
#include <vector>
#include "buffers.hpp"

using std::bitset, std::string, std::vector;

#define bit bitset<1>
#define word bitset<32>

struct RegFile
{
    word Register[32];
    void Initialise();
    void Read(const bitset<5> ReadReg1, const bitset<5> ReadReg2, word& ReadData1, word& ReadData2);
    void Write(const bitset<5> WriteReg, const word WriteData);
};

struct Mem
{
    word *Memory;
    unsigned int MemSize;
    void Initialise(unsigned const int Size);
    void Read(const word Address, word &Data);
};

struct InsMem : Mem
{
    vector<string> basic;
    unsigned int PCMax;
    void Initialise(const string assembly, unsigned const int size, const bool print_basic, const bool print_imem, const int base_type);
};

struct DataMem : Mem
{
    void Write(const word Address, const word Data);
};
struct HDUnit
{
    bit IFFlush, PCWrite, IFIDWrite, Control;
    void Initialise();
};

struct FUnit
{
    struct Forward
    {
        bitset<2> ForwardA, ForwardB;
    } ID, EX;
    void Initialise();
};