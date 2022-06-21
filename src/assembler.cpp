#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "assembler.hpp"

using std::ifstream, std::ofstream, std::istringstream, std::cout, std::cin, std::hex, std::cerr, std::to_string;

void assemble(string assembly, vector<string>& basic, word* memory, unsigned const int memsize, const bool print_basic, const bool print_imem, const int basetype, unsigned int& PCMax)
{
    // --- Convert line to basic, store to vector, and print to file ---

    // Open asm file
    ifstream infile("input/" + assembly);
    while (!infile.is_open())
    {
        cerr << "Error: Failed to open the specified file. Please try again: ";
        cin >> assembly;
        infile.open(assembly);
    }

    // Dump file contents, convert to basic, and store lines
    string line; vector<string> label;
    while (getline(infile, line))
        if (line.front() != '#' && !line.empty() && line.find_first_not_of('\t') + 1 && line.find_first_not_of(' ') + 1) // ensure line is not comments/empty
        {
            // Replace tabs with spaces
            while (line.find('\t') + 1) line.replace(line.find('\t'), 1, " "); // replace tabs with spaces

            // Remove repeat spaces
            for (int i = line.rfind(' '); i > line.find(' '); i--) if (line[i] == ' ' && line[i] == line[i - 1])
                line.erase(line.begin() + i); // replace multiple spaces with one

            // Remove comments
            if (line.find('#') + 1)
            {
                line = line.substr(0, line.find('#')); // remove comments
                if (line == " " || line.empty()) continue; // skip line if empty
            }

            // Save and remove labels
            if (line.find(':') + 1)
            {
                label.push_back(line.substr(0, line.find(':'))); // save label
                line = line.substr(line.find(':') + 1); // remove label
            }
            else label.push_back("");

            // Remove remaining spaces
            if (line.front() == ' ') line.erase(line.begin()); // remove space at beginning
            if (line.back() == ' ') line.erase(line.end() - 1); // remove space at end
            if (line.empty()) continue; // skip line if empty

            istringstream instruction(line);

            // Extract opcode
            string opcode; instruction >> opcode;

            // Convert register names to numbers
            vector<string> arg;
            for (int i = 0; i < 3; i++)
            {
                string arg; instruction >> arg; // extract argument from instruction

                // Extract register uses
                string name;
                if (arg.front() == '$') name = arg.substr(0, arg.find(',')); // extract register uses starting with '$'
                else if (arg.back() == ')')
                {
                    name = arg.substr(arg.find('(') + 1);  // extract register uses ending with ')'
                    name.erase(name.end() - 1); // remove ')'
                }
                else break;

                // Convert number to name
                string num;
                if (name == "$zero") num = "$0";
                else if (name == "$at" || name == "$1")
                {
                    cerr << "Error: Register " << name << " in instruction " << line << " not allowed, reserved for pseudo-instructions\n";
                    exit(1);
                }
                else if (name == "$v0") num = "$2";
                else if (name == "$v1") num = "$3";
                else if (name == "$a0") num = "$4";
                else if (name == "$a1") num = "$5";
                else if (name == "$a2") num = "$6";
                else if (name == "$a3") num = "$7";
                else if (name == "$t0") num = "$8";
                else if (name == "$t1") num = "$9";
                else if (name == "$t2") num = "$10";
                else if (name == "$t3") num = "$11";
                else if (name == "$t4") num = "$12";
                else if (name == "$t5") num = "$13";
                else if (name == "$t6") num = "$14";
                else if (name == "$t7") num = "$15";
                else if (name == "$s0") num = "$16";
                else if (name == "$s1") num = "$17";
                else if (name == "$s2") num = "$18";
                else if (name == "$s3") num = "$19";
                else if (name == "$s4") num = "$20";
                else if (name == "$s5") num = "$21";
                else if (name == "$s6") num = "$22";
                else if (name == "$s7") num = "$23";
                else if (name == "$t8") num = "$24";
                else if (name == "$t9") num = "$25";
                else if (name == "$0" || name == "$2" || name == "$3" || name == "$4" || name == "$5" || name == "$6" || name == "$7" || name == "$8" || name == "$9" || name == "$10" || name == "$11" || name == "$12" || name == "$13" || name == "$14" || name == "$15" || name == "$16" || name == "$17" || name == "$18" || name == "$19" || name == "$20" || name == "$21" || name == "$22" || name == "$23" || name == "$24" || name == "$25") break;
                else
                {
                    cerr << "Error: Register " << name << " in instruction " << line << " not supported\n";
                    exit(1);
                }

                // Replace register name with number
                line.replace(line.find(name), name.length(), num);
            }

            // Convert pseudo instructions
            if (opcode == "li")
            {
                line.replace(line.find("li"), 2, "addiu"); // replace "li" with "addi"
                line.replace(line.find(' ', line.find(' ') + 1), 0, " $0,"); // add " $0" inbetween "$rd" and " $rt"
            }
            else if (opcode == "move")
            {
                line.replace(line.find("move"), 4, "addu"); // replace "move" with "add"
                line.replace(line.find(' ', line.find(' ') + 1), 0, " $0,"); // add " $0" inbetween "$rd" and " $rt"
            }
            else if (opcode == "blt")
            {
                line.replace(line.find("blt"), 3, "sltu"); // replace blt with sltu
                string bltlabel = line.substr(line.rfind(' ') + 1); // save LABEL
                line = line.substr(0, line.rfind(',')); // remove LABEL
                line.replace(line.find(' '), 0, " $1,"); // add " $1," inbetween "opcode" and " $rs"
                basic.push_back(line); // store line to basic
                line = "bne $1, $0, "; line.append(bltlabel); // write new instruction
            }
            else if (opcode == "bgt")
            {
                line.replace(line.find("bgt"), 3, "sltu"); // replace bgt with sltu
                string bgtlabel = line.substr(line.rfind(' ') + 1); // save LABEL
                line = line.substr(0, line.rfind(',')); // remove LABEL
                string rt = line.substr(line.rfind(' ')); // save $rt
                line = line.substr(0, line.find(',')); // remove $rt
                line.replace(line.find(' '), 0, rt.append(",")); // add " $rt," inbetween "opcode" and " $rs"
                line.replace(line.find(' '), 0, " $1,"); // add " $1," inbetween "opcode" and " $rt"
                basic.push_back(line); // store line to basic
                line = "bne $1, $0, "; line.append(bgtlabel); // write new instruction
            }

            // Branch delay - switch instructions
            if (opcode == "beq" || opcode == "bne")
            {
                string prev; prev = basic.back(); // save previous line
                basic.pop_back(); // delete previous line
                basic.push_back(line); // save current line to previous
                basic.push_back(prev); // save previous line to current
            }
            else basic.push_back(line); // Store line to basic
        }

    // Calculate and insert branch/jump addresses
    for (int i = 0; i < basic.size(); i++)
    {
        string opcode; istringstream instruction(basic.at(i)); instruction >> opcode; // Extract opcode
        if (opcode == "beq" || opcode == "bne") // Branch Type
        {
            string branchlabel = basic.at(i).substr(basic.at(i).rfind(' ') + 1); // Extract label
            for (int j = 0; j < basic.size(); j++)
                if (branchlabel == label[j]) // Find matching label
                {
                    basic.at(i) = basic.at(i).substr(0, basic.at(i).rfind(' ') + 1); // Remove label
                    basic.at(i).append(to_string(j - i)); // Append branch address
                    break;
                }
        }
        else if (opcode == "j") // JType
        {
            string jumplabel = basic.at(i).substr(basic.at(i).find(' ') + 1); // Extract label
            for (int j = 0; j < basic.size(); j++)
                if (jumplabel == label[j]) // Find matching label
                {
                    basic.at(i) = basic.at(i).substr(0, basic.at(i).rfind(' ') + 1); // Remove label
                    basic.at(i).append(to_string(j + 1)); // Append jump address
                    break;
                }
        }
    }

    ofstream outfile;

    // Print to file
    if (print_basic)
    {
        outfile.open("output/Basic.txt");
        for (int i = 0; i < basic.size(); i++) outfile << basic.at(i) << '\n';
        outfile.close();
    }

    // Define maximum PC value
    PCMax = basic.size() * 4;


    // --- Convert basic to machine code, print to file, and store to memory ---

    // Open and format instruction memory file
    outfile.open("output/Instruction Memory.txt"); outfile << "--op-- --rs- --rt- --rd- shamt -funct   R-format\n--op-- --rs- --rt- ------imm-------     I-format\n--op-- ------------imm------------      J-format\n\n";

    // Convert strings to binary, print to file, and store to memory
    for (int i = 0; i < basic.size(); i++)
    {
        istringstream instruction(basic.at(i));
        string opstring; instruction >> opstring;

        // Convert supported opcode strings to binary
        bitset<6> opcode;
        if (opstring == "sll" || opstring == "srl" || opstring == "addu" || opstring == "subu" || opstring == "and" || opstring == "or" || opstring == "xor" || opstring == "nor" || opstring == "sltu") opcode = 0;
        else if (opstring == "j") opcode = 2;
        else if (opstring == "beq") opcode = 4;
        else if (opstring == "bne") opcode = 5;
        else if (opstring == "addiu") opcode = 9;
        else if (opstring == "sltiu") opcode = 11;
        else if (opstring == "andi") opcode = 12;
        else if (opstring == "ori") opcode = 13;
        else if (opstring == "xori") opcode = 14;
        else if (opstring == "lw") opcode = 35;
        else if (opstring == "sw") opcode = 43;
        else if (opstring == "nop" || opstring.empty())
        {
            memory[i] = 0;
            // Print to file
            if (print_imem)
            {
                if (!basetype) outfile << memory[i] << '\n';
                if (basetype == 1) outfile << (int)memory[i].to_ulong() << '\n';
                if (basetype == 2) outfile << hex << (int)memory[i].to_ulong() << '\n';
            }
            break;
        }
        else
        {
            cerr << "Error: Unsupported opcode " << opstring << '\n';
            exit(1);
        }

        // Decide format based on opcode:

        // RType format
        if (opcode == 0)
        {
            bitset<5> rs, rt, rd, shamt; bitset<6> funct;
            string rtstring;

            // Decide template based on opstring:

            // Shift template
            if (opstring == "sll" || opstring == "srl")
            {
                // Extract corresponding strings
                string rdstring, shamtstring; instruction >> rdstring >> rtstring >> shamtstring;
                
                // Convert strings to bitstreams
                rs = 0;
                rd = stoi(rdstring.substr(1, rdstring.length() - 2));
                rt = stoi(rtstring.substr(1));
                shamt = stoi(shamtstring);
                if (opstring == "sll") funct = 0;
                else if (opstring == "srl") funct = 2;
                else /* sra */ funct = 3;
            }

            // JumpR template
            /* else if (opstring == "jr")
            {
                // Extract corresponding string
                string rsstring; instruction >> rsstring;

                // Convert string to bitstream
                rs = stoi(rsstring.substr(1, rsstring.length() - 2));
                rd = rt = shamt = 0;
                funct = 8;
            } */

            // Arithmetic template
            else
            {
                // Extract corresponding strings
                string rdstring, rsstring; instruction >> rdstring >> rsstring >> rtstring;
                
                // Convert strings to bitstreams
                rs = stoi(rsstring.substr(1, rsstring.length() - 2));
                rd = stoi(rdstring.substr(1, rdstring.length() - 2));
                rt = stoi(rtstring.substr(1));
                shamt = 0;
                if (opstring == "addu") funct = 33;
                else if (opstring == "subu") funct = 35;
                else if (opstring == "and") funct = 36;
                else if (opstring == "or") funct = 37;
                else if (opstring == "xor") funct = 38;
                else if (opstring == "nor") funct = 39;
                else /* opstring == sltu */ funct = 43;
            }

            // Print to file
            if (print_imem)
            {
                if (!basetype) outfile << opcode << " " << rs << " " << rt << " " << rd << " " << shamt << " " << funct << '\n';
                else if (basetype == 1) outfile << (int)opcode.to_ulong() << " " << (int)rs.to_ulong() << " " << (int)rt.to_ulong() << " " << (int)rd.to_ulong() << " " << (int)shamt.to_ulong() << " " << (int)funct.to_ulong() << '\n';
                else outfile << hex << (int)opcode.to_ulong() << " " << hex << (int)rs.to_ulong() << " " << hex << (int)rt.to_ulong() << " " << hex << (int)rd.to_ulong() << " " << hex << (int)shamt.to_ulong() << " " << hex << (int)funct.to_ulong() << '\n';
            }
            
            // Concatenate and store in memory
            memory[i] = ((rs.to_ulong() << 21) | (rt.to_ulong() << 16) | (rd.to_ulong() << 11) | (shamt.to_ulong() << 6) | funct.to_ulong());
        }

        // JType format 
        else if (opcode == 2)
        {
            bitset<26> address;

            // Extract corresponding string
            string addrstring; instruction >> addrstring; 

            // Convert string to bitstream
            address = stoi(addrstring) - 1;

            // Print to file
            if (print_imem)
            {
                if (!basetype) outfile << opcode << " " << address << '\n';
                else if (basetype == 1) outfile << (int)opcode.to_ulong() << " " << (int)address.to_ulong() << '\n';
                else outfile << hex << (int)opcode.to_ulong() << " " << hex << (int)address.to_ulong() << '\n';
            }

            // Concatenate and store in memory
            memory[i] = ((opcode.to_ulong() << 26) | address.to_ulong());
        }

        // IType format
        else
        {
            bitset<5> rs, rt; bitset<16> imm;
            string rsstring, rtstring, immstring;
            
            // Decide template based on opcode:

            // Branch template
            if (opcode == 4 || opcode == 5)
            {
                // Extract corresponding strings
                instruction >> rsstring >> rtstring >> immstring;

                // Convert strings to bitstreams
                rs = stoi(rsstring.substr(1, rsstring.length() - 2));
                rt = stoi(rtstring.substr(1, rtstring.length() - 2));
                imm = stoi(immstring) - 1;
            }

            // Load/Store template
            else if (opcode.to_ulong() >= 35)
            {
                // Extract corresponding strings
                string immrs; instruction >> rtstring >> immrs;
                immstring = immrs.substr(0, immrs.find('('));
                rsstring = immrs.substr(immrs.find('(') + 1, immrs.length() - 2);

                // Convert strings to bitstreams
                rt = stoi(rtstring.substr(1, rtstring.length() - 2));
                rs = stoi(rsstring.substr(1, rsstring.length()));
                if (!immstring.empty()) imm = stoi(immstring);
                else imm = 0;
            }

            // ArithmeticI template
            else
            {
                // Extract corresponding strings
                instruction >> rtstring >> rsstring >> immstring;

                // Convert strings to bitstream
                rt = stoi(rtstring.substr(1, rtstring.length() - 2));
                rs = stoi(rsstring.substr(1, rsstring.length() - 2));
                imm = stoi(immstring);
            }

            // Print to file
            if (print_imem)
            {
                if (!basetype) outfile << opcode << " " << rs << " " << rt << " " << imm << '\n';
                else if (basetype == 1) outfile << (int)opcode.to_ulong() << " " << (int)rs.to_ulong() << " " << (int)rt.to_ulong() << " " << (int)imm.to_ulong() << '\n';
                else outfile << hex << (int)opcode.to_ulong() << " " << hex << (int)rs.to_ulong() << " " << hex << (int)rt.to_ulong() << " " << hex << (int)imm.to_ulong() << '\n';
            }

            // Concatenate and store in memory
            memory[i] = ((opcode.to_ulong() << 26) | (rs.to_ulong() << 21) | (rt.to_ulong() << 16) | imm.to_ulong());
        }
    }

    // Save and print remaineder of memory (empty)
    for (int i = basic.size(); i < memsize; i++)
    {
        memory[i] = 0;
        // Print to file
        if (print_imem)
        {            
            if (!basetype) outfile << memory[i] << '\n';
            if (basetype == 1) outfile << (int)memory[i].to_ulong() << '\n';
            if (basetype == 2) outfile << hex << (int)memory[i].to_ulong() << '\n';
        }
    }
}
