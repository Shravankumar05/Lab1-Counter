#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char **argv, char **env)
{
    // initializing variables
    int i;
    int clk;
    int pause_cycles = 3;    // Number of cycles to pause the counter
    bool pause_flag = false; // Flag to indicate when to pause

    Verilated::commandArgs(argc, argv);
    // init top Verilog instance
    Vcounter *top = new Vcounter;
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC *tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("counter.vcd");

    // initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0;

    // run simulation for i many clock cycles
    for (i = 0; i < 300; i++)
    {
        // dump variables into VCD file and toggle clock
        for (clk = 0; clk < 2; clk++)
        {
            tfp->dump(2 * i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        // Logic to pause the counter for 3 cycles when the count reaches 0x9
        if (top->count == 9 && !pause_flag)
        {
            pause_flag = true; // Set flag to start pause
            pause_cycles = 3;  // Reset pause cycle counter
        }

        // If pause flag is set, decrement the pause cycle counter
        if (pause_flag)
        {
            top->en = 0; // Disable counting (pause)
            pause_cycles--;

            // Once 3 cycles have passed, clear the pause flag and resume counting
            if (pause_cycles == 0)
            {
                pause_flag = false;
                top->en = 1; // Re-enable counting
            }
        }
        else
        {
            // Normal operation: control reset and enable signals
            top->rst = (i < 2) | (i == 15); // Reset on specific cycles
            top->en = (i > 4);              // Enable counting after cycle 4
        }

        if (Verilated::gotFinish())
            break; // Break out if finish signal is triggered
    }

    tfp->close();
    exit(0);
}
