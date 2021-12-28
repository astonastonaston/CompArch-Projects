`timescale 1ns/1ns

module test_CPU;

// enable and clk signals
reg clk, InsEn, DMEn;
// reset signals
reg InsMemRst, MainMemRst, regFileRst, ctrRst, aluRst;
reg wbifRst, ifidRst, idexRst, exmemRst, memwbRst, fwrst;
// printing Data Memory signal
reg printmem;
// output terminating signal
wire EndProgram;

// instantiating CPU
CPU test(printmem, regFileRst, MainMemRst, InsMemRst, ctrRst, 
aluRst, wbifRst, ifidRst, idexRst, exmemRst, memwbRst, 
fwrst, InsEn, DMEn, clk, EndProgram);

// clock with period = 2ns
always begin 
    #1 clk <= 1'b0;
    #1 clk <= 1'b1;
end

// testing starts here
initial begin
    // resetting:
    regFileRst <= 1; // Register File
    MainMemRst <= 1; // Main Memory
    ctrRst <= 1; // Ctr Unit
    aluRst <= 1; // ALU
    wbifRst <= 1; // WB/IF pipeline register
    ifidRst <= 1; // IF/ID pipeline register
    idexRst <= 1; // ID/EX pipeline register
    exmemRst <= 1; // EX/MEM pipeline register
    memwbRst <= 1; // MEM/WB pipeline register
    fwrst <= 1; // data forwarding unit reset
    InsEn <= 1; // Instruction Memory enable signal
    DMEn <= 1; // Data Memory enable signal

    // Load instruction file
    // Instruction execution starts
    InsMemRst <= 1;

    // wait till the program ends
    wait(EndProgram);

    // print out DM and ensure enough execution cycles
    printmem <= 1;

    // 1fs delay for printing results in file output.txt
    #1
    $display("cycle period: 2ns");
    $display("1ns delay given by the testbench to ensure the complete printing of DM");
    $finish;
end

initial
begin            
    // waveform .vcd file generator
    $dumpfile("test_CPU.vcd");        
    $dumpvars(0, test_CPU);    
end 
    
endmodule