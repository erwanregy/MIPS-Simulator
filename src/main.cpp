#include <iostream>
#include <string>
#include <chrono>
#include "cpu.hpp"

using std::cin, std::cout, std::cerr, std::stoi;

#define IMEMSIZE 16
#define DMEMSIZE 256

template <typename Clock = std::chrono::steady_clock>
class Timer
{
    typename Clock::time_point last_;
public:
    Timer() : last_(Clock::now()) {}
    void reset()
    {
        *this = Timer();
    }
    typename Clock::duration elapsed() const
    {
        return Clock::now() - last_;
    }
    typename Clock::duration tick()
    {
        auto now = Clock::now();
        auto elapsed = now - last_;
        last_ = now;
        return elapsed;
    }
};

template <typename T, typename Rep, typename Period>
T duration_cast(const std::chrono::duration<Rep, Period>& duration)
{
    return duration.count() * static_cast<T>(Period::num) / static_cast<T>(Period::den);
}

int main(const int argc, char* argv[])
{
    string argstr[argc];
    for (int i = 1; i < argc; i++) argstr[i].append(argv[i]);

    bool timing = 0;
    bool step_mode = 0;
    int base_type = 1;
    bool forwarding = 1;
    bool print_basic = 1;
    bool print_imem = 1;
    int print_pipeline = 1;
    int print_regfile = 1;
    int print_dmem = 1;
    int print_buffers = 1;
    bool testing_mode = 0;
    int i = 1;
    while(i < argc)
    {
        if (argstr[i] == "-t" || argstr[i] == "--timing") timing = 1;
        else if (argstr[i] == "-s" || argstr[i] == "--step-mode") step_mode = 1;
        else if (argstr[i] == "-b" || argstr[i] == "--base_type") base_type = stoi(argstr[++i]);
        else if (argstr[i] == "-f" || argstr[i] == "--forwarding") forwarding = stoi(argstr[++i]);
        else if (argstr[i] == "-a" || argstr[i] == "--print-basic") print_basic = stoi(argstr[++i]);
        else if (argstr[i] == "-i" || argstr[i] == "--print-imem") print_imem = stoi(argstr[++i]);
        else if (argstr[i] == "-p" || argstr[i] == "--print-pipeline") print_pipeline = stoi(argstr[++i]);
        else if (argstr[i] == "-r" || argstr[i] == "--print-regfile") print_regfile = stoi(argstr[++i]);
        else if (argstr[i] == "-d" || argstr[i] == "--print-dmem") print_dmem = stoi(argstr[++i]);
        else if (argstr[i] == "-d" || argstr[i] == "--print-buffers") print_buffers = stoi(argstr[++i]);
        else if (argstr[i] == "-x" || argstr[i] == "--testing-mode") testing_mode = 1;
        else if (argstr[i] == "-h" || argstr[i] == "--help")
        {
            cout << "Commands:" << argstr[0] << "[-t] [-s] [-b base-type] [-f forwarding] [-b print-basic] [-i print-imem] [-p print-pipeline] [-r print-regfile] [-d print-dmem] [-x]\n";
            exit(1);
        }
        else
        {
            cerr << "Error: Unrecognised command\n";
            exit(1);
        }
        i++;
    }
    
    string assembly_file;
    if (!testing_mode)
    {
        cout << "Please enter the name of the file to read assembly from: ";
        cin >> assembly_file;
        cin.get();
    }

    CPU cpu(assembly_file, IMEMSIZE, DMEMSIZE, step_mode, base_type, forwarding, print_basic, print_imem, print_pipeline, print_regfile, print_dmem, testing_mode, print_buffers);

    Timer<> timer;

    cpu.run();

    cout << "Total number of clock cycles: " << cpu.get_cycles() << '\n';

    if (timing) std::cout << "Time elapsed: " << duration_cast<double>(timer.elapsed()) << " sec\n";
    
    cin.get();

    return 0;
}


