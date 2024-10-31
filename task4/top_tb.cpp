#include "Vtop.h" // Include the correct header for the top module
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

int main(int argc, char **argv, char **env)
{
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);
    // Initialize the top-level Verilog instance
    Vtop *top = new Vtop; // Replace Vcounter with Vtop

    // Initialize trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC *tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("counter.vcd");

    if (vbdOpen() != 1)
        return -1;
    vbdHeader("Lab 1: Counter");
    vbdSetMode(0);

    top->clk = 1;
    top->rst = 1;
    top->en = 0;

    for (i = 0; i < 300; i++)
    {
        for (clk = 0; clk < 2; clk++)
        {
            tfp->dump(2 * i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        // Send each of the BCD digits to Vbuddy
        vbdHex(4, (int(top->bcd) >> 8) & 0xF);
        vbdHex(3, (int(top->bcd) >> 4) & 0xF);
        vbdHex(2, (int(top->bcd)) & 0xF);

        vbdCycle(i + 1);

        top->rst = (i < 2) | (i == 15); // Reset on specific cycles
        top->en = vbdFlag();

        if (Verilated::gotFinish())
            break; // Break out if finish signal is triggered
    }

    vbdClose();
    tfp->close();
    exit(0);
}
