#pragma once

#include <bitset>
#include <string>
#include <vector>

using std::string, std::bitset, std::vector;

#define word bitset<32>

void assemble(string assembly, vector<string>& basic, word* memory, unsigned const int memsize, const bool print_basic, const bool print_imem, const int basetype, unsigned int& PCMax);