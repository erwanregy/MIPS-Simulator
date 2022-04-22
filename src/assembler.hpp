#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

using std::string, std::bitset, std::vector, std::ifstream, std::ofstream, std::istringstream, std::cout, std::cin, std::hex, std::cerr, std::to_string, std::endl;

#define word bitset<32>

void assemble(string assembly, vector<string> &basic, word *memory, const int memsize)
{
    // Convert ASM to basic, and print to file, and store to basic vector
    
    ifstream infile("input/" + assembly); string line; vector<string> label;
    while (!infile.is_open())
    {
        cerr << "Error: Failed to open the specified file. Please try again: ";
        cin >> assembly;
        infile.open(assembly);
    }

    // Dump file contents and format:
    while (getline(infile, line))
        if (line.front() != '#' && !line.empty() && line.find_first_not_of('\t') + 1 && line.find_first_not_of(' ') + 1)
        {
            // Format instruction:
            while (line.find('\t') + 1) line.replace(line.find('\t'), 1, " "); // Replace tabs with spaces
            for (int i = line.rfind(' '); i > line.find(' '); i--) if (line[i] == ' ' && line[i] == line[i - 1]) line.erase(line.begin() + i); // Replace multiple spaces with one
            if (line.find('#') + 1)
            {
                line = line.substr(0, line.find('#')); // Remove comments
                if (line == " " || line.empty()) continue; // Skip line if empty
            }
            if (line.find(':') + 1)
            {
                label.push_back(line.substr(0, line.find(':'))); // Save label
                line = line.substr(line.find(':') + 1); // Remove label
            }
            else label.push_back("");
            if (line.front() == ' ') line.erase(line.begin()); // Remove space at beginning
            if (line.back() == ' ') line.erase(line.end() - 1); // Remove space at end
            if (line.empty()) continue; // Skip line if empty

            istringstream instring(line);
            string opcode; instring >> opcode; // Extract opcode

            // Convert register names to numbers:
            vector<string> ins;
            for (int i = 0; i < 3; i++)
            {
                ins.resize(i + 1);
                instring >> ins.at(i); // extract string from instruction
                if (ins.at(i).front() == '$') ins.at(i) = ins.at(i).substr(0, ins.at(i).find(',')); // extract strings starting with $
                else if (ins.at(i).back() == ')')
                {
                    ins.at(i) = ins.at(i).substr(ins.at(i).find('(') + 1);  // extract strings ending with )
                    ins.at(i).erase(ins.at(i).end() - 1); // remove ) at end
                }
                else break;
                string reg;
                // calculate number for name
                if (ins.at(i) == "$zero") reg = "$0";
                else if (ins.at(i) == "$v0") reg = "$2";
                else if (ins.at(i) == "$v1") reg = "$3";
                else if (ins.at(i) == "$a0") reg = "$4";
                else if (ins.at(i) == "$a1") reg = "$5";
                else if (ins.at(i) == "$a2") reg = "$6";
                else if (ins.at(i) == "$a3") reg = "$7";
                else if (ins.at(i) == "$t0") reg = "$8";
                else if (ins.at(i) == "$t1") reg = "$9";
                else if (ins.at(i) == "$t2") reg = "$10";
                else if (ins.at(i) == "$t3") reg = "$11";
                else if (ins.at(i) == "$t4") reg = "$12";
                else if (ins.at(i) == "$t5") reg = "$13";
                else if (ins.at(i) == "$t6") reg = "$14";
                else if (ins.at(i) == "$t7") reg = "$15";
                else if (ins.at(i) == "$s0") reg = "$16";
                else if (ins.at(i) == "$s1") reg = "$17";
                else if (ins.at(i) == "$s2") reg = "$18";
                else if (ins.at(i) == "$s3") reg = "$19";
                else if (ins.at(i) == "$s4") reg = "$20";
                else if (ins.at(i) == "$s5") reg = "$21";
                else if (ins.at(i) == "$s6") reg = "$22";
                else if (ins.at(i) == "$s7") reg = "$23";
                else if (ins.at(i) == "$t8") reg = "$24";
                else if (ins.at(i) == "$t9") reg = "$25";
                else
                {
                    cerr << "Error: Register " << ins.at(i) << " in instruction " << line << " not supported" << endl;
                    exit(1);
                }
                line.replace(line.find(ins.at(i)), ins.at(i).length(), reg); // replace name with number in line
            }

            // Convert pseudo instructions:
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
                line.replace(line.find("blt"), 3, "sltu"); // replace blt with slt
                string bltlabel = line.substr(line.rfind(' ') + 1); // save LABEL
                line = line.substr(0, line.rfind(',')); // remove LABEL
                line.replace(line.find(' '), 0, " $1,"); // add " $1," inbetween "opcode" and " $rs"
                basic.push_back(line); // store line to basic
                line = "bne $1, $0, "; line.append(bltlabel);
            }
            else if (opcode == "bgt")
            {
                line.replace(line.find("bgt"), 3, "sltu"); // replace bgt with slt
                string bgtlabel = line.substr(line.rfind(' ') + 1); // save LABEL
                line = line.substr(0, line.rfind(',')); // remove LABEL
                string rt = line.substr(line.rfind(' ')); // save $rt
                line = line.substr(0, line.find(',')); // remove $rt
                line.replace(line.find(' '), 0, rt.append(",")); // add " $rt," inbetween "opcode" and " $rs"
                line.replace(line.find(' '), 0, " $1,"); // add " $1," inbetween "opcode" and " $rt"
                basic.push_back(line); // store line to basic
                line = "bne $1, $0, "; line.append(bgtlabel);
            }

            // Branch delay switch instructions:
            if (opcode == "beq" || opcode == "bne")
            {
                string prev; prev = basic.back(); // save previous line
                basic.pop_back(); // delete previous line
                basic.push_back(line); // save current line to previous
                basic.push_back(prev); // save previous line to current
            }
            else basic.push_back(line); // Store line to basic
        }

    // Calculate and insert branch/jump addresses:
    for (int i = 0; i < basic.size(); i++)
    {
        string opcode; istringstream instring(basic.at(i)); instring >> opcode; // Extract opcode
        if (opcode == "beq" || opcode == "bne") // Branch Type
        {
            string branchlabel = basic.at(i).substr(basic.at(i).rfind(' ') + 1); // Extract label
            for (int j = 0; j < basic.size(); j++)
                if (branchlabel == label[j]) // Find matching label
                {
                    basic.at(i) = basic.at(i).substr(0, basic.at(i).rfind(' ') + 1); // Remove label
                    basic.at(i).append(to_string(j-i)); // Append branch address
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
                    basic.at(i).append(to_string(j+1)); // Append jump address
                    break;
                }
        }
    }

    // Output basic file:
    ofstream outfile("output/Basic.txt");
    for (int i = 0; i < basic.size(); i++) outfile << basic.at(i) << endl;
    outfile.close();

    // Convert basic to machine code, print to file, and store to memory

    // Ask user for instruction memory file style
    int imemstyle;
    cout << "Print instruction memory in binary, decimal, or hex? (0/1/2): ";
    while (1)
    {
        // cin >> imemstyle;
        imemstyle = 0;
        if (imemstyle >= 0 && imemstyle <= 2) break;
        else cerr << "Error: Input can only be 0, 1, 2. Please try again: ";
    }

    outfile.open("output/Instruction Memory.txt"); outfile << "--op-- --rs- --rt- --rd- shamt -funct   R-format\n--op-- --rs- --rt- ------imm-------     I-format\n--op-- ------------imm------------      J-format\n" << endl; // Prepare instruction memory output file

    for (int i = 0; i < basic.size(); i++)
    {
        istringstream instring(basic.at(i));
        string opstring; instring >> opstring;

        // Convert opstring to opcode
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
            if (!imemstyle) outfile << memory[i] << endl;
            if (imemstyle == 1) outfile << (int)memory[i].to_ulong() << endl;
            if (imemstyle == 2) outfile << hex << (int)memory[i].to_ulong() << endl;
            break;
        }
        else
        {
            cerr << "Error: Unsupported opcode " << opstring << endl;
            exit(1);
        }
        // Decide format based on opcode:
        if (opcode == 0) // RType format
        {
            bitset<5> rs, rt, rd, shamt; bitset<6> funct;
            string rtstring;

            // Decide template based on opstring:
            if (opstring == "sll" || opstring == "srl") // Shift template
            {
                string rdstring, shamtstring; instring >> rdstring >> rtstring >> shamtstring; // Extract corresponding strings
                // Convert strings to bitstreams:
                rs = 0;
                rd = stoi(rdstring.substr(1, rdstring.length() - 2));
                rt = stoi(rtstring.substr(1));
                shamt = stoi(shamtstring);
                if (opstring == "sll") funct = 0;
                else if (opstring == "srl") funct = 2;
                else funct = 3; // sra
            }
            /* else if (opstring == "jr") // JumpR template
            {
                string rsstring; instring >> rsstring; // Extract corresponding string
                rs = stoi(rsstring.substr(1, rsstring.length() - 2)); // Convert string to bitstream
                rd = rt = shamt = 0;
                funct = 8;
            } */
            else // Arithmetic template
            {
                string rdstring, rsstring; instring >> rdstring >> rsstring >> rtstring; // Extract corresponding strings
                // Convert strings to bitstreams:
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
            // Output instruction memory:
            if (!imemstyle) outfile << opcode << " " << rs << " " << rt << " " << rd << " " << shamt << " " << funct << endl;
            else if (imemstyle == 1) outfile << (int)opcode.to_ulong() << " " << (int)rs.to_ulong() << " " << (int)rt.to_ulong() << " " << (int)rd.to_ulong() << " " << (int)shamt.to_ulong() << " " << (int)funct.to_ulong() << endl;
            else outfile << hex << (int)opcode.to_ulong() << " " << hex << (int)rs.to_ulong() << " " << hex << (int)rt.to_ulong() << " " << hex << (int)rd.to_ulong() << " " << hex << (int)shamt.to_ulong() << " " << hex << (int)funct.to_ulong() << endl;
            memory[i] = ((rs.to_ulong() << 21) | (rt.to_ulong() << 16) | (rd.to_ulong() << 11) | (shamt.to_ulong() << 6) | funct.to_ulong()); // Concatenate and store in memory
        }

        else if (opcode == 2) // JType format 
        {
            bitset<26> address;
            string addrstring; instring >> addrstring; // Extract corresponding string
            address = stoi(addrstring); // Convert string to bitstream
            // Output instruction memory:
            if (!imemstyle) outfile << opcode << " " << address << endl;
            else if (imemstyle == 1) outfile << (int)opcode.to_ulong() << " " << (int)address.to_ulong() << endl;
            else outfile << hex << (int)opcode.to_ulong() << " " << hex << (int)address.to_ulong() << endl;
            memory[i] = ((opcode.to_ulong() << 26) | address.to_ulong()); // Concatenate and store in memory
        }

        else // IType format
        {
            bitset<5> rs, rt; bitset<16> imm;
            string rsstring, rtstring, immstring;
            // Decide template based on opcode
            if (opcode == 4 || opcode == 5) // Branch template
            {
                instring >> rsstring >> rtstring >> immstring; // Extract corresponding strings
                // Convert strings to bitstreams:
                rs = stoi(rsstring.substr(1, rsstring.length() - 2));
                rt = stoi(rtstring.substr(1, rtstring.length() - 2));
                imm = stoi(immstring) - 1;
            }
            else if (opcode.to_ulong() >= 35) // Load/Store template
            {
                // Extract corresponding strings:
                string immrs; instring >> rtstring >> immrs;
                immstring = immrs.substr(0, immrs.find('('));
                rsstring = immrs.substr(immrs.find('(') + 1, immrs.length() - 2);
                // Convert strings to bitstreams:
                rt = stoi(rtstring.substr(1, rtstring.length() - 2));
                rs = stoi(rsstring.substr(1, rsstring.length()));
                if (!immstring.empty()) imm = stoi(immstring);
                else imm = 0;
            }
            else // ArithmeticI template
            {
                instring >> rtstring >> rsstring >> immstring; // Extract corresponding strings
                // Convert strings to bitstreams:
                rt = stoi(rtstring.substr(1, rtstring.length() - 2));
                rs = stoi(rsstring.substr(1, rsstring.length() - 2));
                imm = stoi(immstring);
            }
            // Output instruction memory:
            if (!imemstyle) outfile << opcode << " " << rs << " " << rt << " " << imm << endl;
            else if (imemstyle == 1) outfile << (int)opcode.to_ulong() << " " << (int)rs.to_ulong() << " " << (int)rt.to_ulong() << " " << (int)imm.to_ulong() << endl;
            else outfile << hex << (int)opcode.to_ulong() << " " << hex << (int)rs.to_ulong() << " " << hex << (int)rt.to_ulong() << " " << hex << (int)imm.to_ulong() << endl;
            memory[i] = ((opcode.to_ulong() << 26) | (rs.to_ulong() << 21) | (rt.to_ulong() << 16) | imm.to_ulong()); // Concatenate and store in memory
        }
    }

    // Store and print remainder of memory (empty)
    for (int i = basic.size(); i < memsize; i++)
    {
        memory[i] = 0;
        if (!imemstyle) outfile << memory[i] << endl;
        if (imemstyle == 1) outfile << (int)memory[i].to_ulong() << endl;
        if (imemstyle == 2) outfile << hex << (int)memory[i].to_ulong() << endl;
    }
}