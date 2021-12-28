// Central Processing Unit

// input:
// reset signals --> reset values in relevant components to 0
// Enable signals --> for IM and DM
// clk

// output:
// EndProgram --> end signal for the program

`timescale 1ns/100ps

module CPU (
    // reset inputs for components:
    input printmem,
    input regFileRst, // regFile
    input MainMemRst, // main memory
    input InsMemRst, // instr memory 
    input ctrRst, // ctr unit
    input aluRst, // alu
    // reset inputs for ppline registers
    input wbifRst, 
    input ifidRst,
    input idexRst,
    input exmemRst,
    input memwbRst,
    // reset for forward unit
    input fwunitrst,
    // enable signals and clk
    input InsEn,
    input DMEn,
    input clk,
    // terminate flag
    output wire EndProgram
);
    // Data wires (37 ones)
    wire [27:0] insAddrcon;
    wire [31:0] PCPlus4F, conAddr, 
    PCBranchM, PCF, JrPC, InstrF, InstrD, 
    WriteDataM, ALUOutE, ResultW, PCPlus4D, SignImmD,
    ReadDataW, ALUOutM, RegD1, RegD2, RegE1, RegE2,
    ALUOutW, ReadDataM, SignImmE,
    JJalPC, BranchPC, PCPlus4E, SrcAE, SrcBE, 
    ExtSignImmE, PCBranchE, forwardSelRegA,
    forwardSelRegB;
    wire [4:0] WriteRegW, WriteRegE, 
    SftaE, WriteRegM, RtE, RdE, RsE;
    
    // control wires (32 ones)
    wire Jump, Jreg, MemReadD, RegWriteD, MemtoRegD,
    RegWriteW, MemtoRegW, EndD, EndE, EndM, bneD,
    MemWriteD, BranchD, SftD, ALUSrcD, shouldBranch,
    RegDstD, JalWrite, MemReadE, RegWriteE, MemtoRegE,
    MemWriteE, BranchE, SftE, ALUSrcE, equalbit, RsEtoDet, 
    RegDstE, MemReadM, RegWriteM, JrHaz,
    MemtoRegM, MemWriteM, BranchM, stallF, stallD, flushE;
    wire [1:0] forwardA, forwardB;
    wire [3:0] ALUControlE, ALUControlD;

    // Instruction memory
    InstructionRAM insram(
        // Inputs
        clk
        , InsMemRst
        , InsEn
        , PCF
        // Outputs
        , InstrF);

    // Main Memory
    MainMemory DM(
        // Inputs
        printmem,
        clk // clock
        , MainMemRst // reset
        , DMEn // enable
        , ALUOutM // address calculated from ALU
        , MemReadM // CTR read signal
        , MemWriteM // CTR write signal
        , WriteDataM // writing data
        // Output
        , ReadDataM);

    // Register File
    RegFile regfile(
        // Inputs
        clk
        , regFileRst
        , RegWriteW
        , JalWrite 
        , PCPlus4D
        , WriteRegW
        , InstrD[25:21]
        , InstrD[20:16]
        , ResultW
        // Outputs
        , RegD1
        , RegD2);

    // ALU
    ALU alu(
        // Inputs
        aluRst,
        clk
        , SrcAE  
        , SrcBE 
        , ALUControlE
        // Outputs
        , ALUOutE);

    // Main Control Unit
    ControlUnit Ctr(
        // Inputs
        ctrRst,
        InstrD[31:26],
        InstrD[5:0],
        clk,
        // Outputs
        bneD,
        Jreg,
        Jump,
        MemReadD,
        JalWrite,
        SftD,
        RegWriteD,
        MemtoRegD,
        MemWriteD,
        BranchD,
        ALUControlD,
        ALUSrcD,
        RegDstD,
        EndD  
    );

    // WB/IF pipeline register
    WBIFppreg WBtoIFppreg (
        // Inputs
        JrPC // Effective Program Counter
        , clk // clock
        , wbifRst // reset 
        , stallF // stalling ctr signal
        // Outputs
        , PCF // fetched Program Counter
    );

    // IF/ID pipeline register
    IFIDppreg IFtoIDppreg ( 
        // Inputs
        JrHaz,
        Jreg,
        InstrF // Program Counter
        , PCPlus4F // PC + 4 
        , clk // clock
        , Jump // jump flush
        , ifidRst // reset
        , shouldBranch
        , stallD
        // Outputs
        , InstrD // fetched Program Counter
        , PCPlus4D // PC + 4 
    );

    // ID/EX pipeline register
    IDEXppreg IDtoEXppreg (
        // data inputs
        idexRst, // reset
        clk // clock
        , InstrD[25:21] // Rs in instruction for forwarding judgement
        , RegD1 // value in reg instr[25:21]
        , RegD2 // value in reg instr[20:16]
        , InstrD[20:16] // register destination for lw/sw
        , InstrD[15:11] // register destination for R-type instr
        , SignImmD // Signed immediate from ins[15:0] 

        // ctr signal inputs
        , flushE // flushing signal
        , MemReadD // judge whether trying to read mem
        , RegWriteD // write register enable 
        , MemtoRegD // select ALUOut/Memdata to register write data
        , MemWriteD // write memory enable
        , ALUControlD // ALUctr
        , ALUSrcD // select imm/register_value to ALU
        , RegDstD // select ins[20:16](lw, sw)/ins[15:11](others) as write destination register
        , InstrD[10:6] // shift amount (SftaD)
        , EndD // end instr signal
        , SftD // shift control sig
        
        // data outputs
        , RsE // Rs in prev instruction
        , RegE1 // value in reg instr[25:21]
        , RegE2 // value in reg instr[20:16]
        , RtE // register destination for lw/sw
        , RdE // register destination for R-type ins
        , SignImmE // Signed immediate from ins[15:0] 

        // ctr signal outputs
        , MemReadE // judge whether trying to read mem
        , RegWriteE // write register enable 
        , MemtoRegE // select ALUOut/Memdata to register write data
        , MemWriteE // write memory enable
        , ALUControlE // ALUctr
        , ALUSrcE // select imm/register_value to ALU
        , RegDstE // select ins[20:16](lw, sw)/ins[15:11](others) as write destination register
        , SftaE // shift ins, one indicating shift amount load
        , EndE // end instr signal
        , SftE // shift control sig
    );

    // EX/MEM pipeline register
    EXMEMppreg EXtoMEMppreg (
        // Inputs
        exmemRst,
        clk // clock
        , ALUOutE // result of ALU calculation
        , forwardSelRegB // data to be written to main mem
        , WriteRegE // write register destination address
        , MemReadE // judge whether trying to read mem
        , RegWriteE // write register enable 
        , MemtoRegE // select ALUOut/Memdata to register write data
        , MemWriteE // write memory enable
        , EndE // end instr signal

        // Outputs
        , ALUOutM // result of ALU calculation
        , WriteDataM // data to be written to register
        , WriteRegM // write register destination address
        , MemReadM // judge whether trying to read mem
        , RegWriteM // write register enable 
        , MemtoRegM // select ALUOut/Memdata to register write data
        , MemWriteM // write memory enable
        , EndM // end instr signal
    );

    // MEM/WB pipeline register
    MEMWBppreg MEMtoWBppreg (
        // data inputs
        memwbRst,
        clk // clock
        , RegWriteM // write register enable
        , MemtoRegM // select alu output or memory data to register
        , ALUOutM // result of ALU calculation 
        , ReadDataM // data read from main memory
        , WriteRegM // register address to be written
        , EndM // end instr signal

        // data outputs
        , RegWriteW // write register enable
        , MemtoRegW // select alu output or memory data to register
        , ALUOutW // result of ALU calculation 
        , ReadDataW // data read from main memory
        , WriteRegW // register address to be written
        , EndProgram // end instr signal
    );

    // PC increment adder (PC +4)
    adder pcPlus4(PCF, 4, PCPlus4F);
    
    // Branch address calculation adder
    adder branchAddrCal(ExtSignImmE, PCPlus4D, PCBranchM);
    
    // Bit extender (sign/0 extension depending on instr)
    signExtender extendImm(InstrD[31:26], InstrD[15:0], SignImmD);

    // 2-bit shifter for immediate and address calculation
    twobitshifter WordByteAddrTrans(SignImmD, ExtSignImmE);
    twobitshifter WordByteAddrConvr(InstrD, insAddrcon);

    // Address MSBs and LSBs concatenator
    addrconca JumpAddrConca(insAddrcon[27:0], PCPlus4F[31:28], conAddr);

    // Multiplexers:

    // PC selections:
    // branch selection
    twoMux BranchAndPlus4Sel(PCPlus4F, PCBranchM, shouldBranch, BranchPC);

    // Jal target address selection
    twoMux JJalSel(BranchPC, conAddr, Jump, JJalPC);

    // Jr target address selection
    twoMux JrSel(JJalPC, RegD1, Jreg, JrPC);

    // write back register selection
    twoMux_FiveBit WriteRegSel(RtE, RdE, RegDstE, WriteRegE);

    // ALU input selections:
    // immediate number selection
    twoMux ALUBSrcSel(forwardSelRegB, SignImmE, ALUSrcE, SrcBE);

    // shift amount selection
    twoMux ALUASrcSel(forwardSelRegA, SftaE, SftE, SrcAE);

    // EXMEM/MEMWB-forwarded value selection
    ThreeMux forwardSelA(RegE1, ResultW, ALUOutM, forwardA, forwardSelRegA);
    ThreeMux forwardSelB(RegE2, ResultW, ALUOutM, forwardB, forwardSelRegB);

    // Branch hazard detectors and branch-in-advance controllers:
    // equal judging unit for branching in advance
    equalDet equalUnit(RegD1, RegD2, equalbit);
    
    // detect whether a branch is needed in advance
    DetectIC BranchDetector(bneD, BranchD, equalbit, shouldBranch);

    // write back register data selection
    twoMux WBDataSel(ALUOutW, ReadDataW, MemtoRegW, ResultW);

    // Data hazard detector with forwarding and stalling controller 
    DataHazardDetUnit DatHazDetector(BranchD, Jreg, WriteRegE, InstrD[25:21], InstrD[20:16], InstrD[25:21], MemReadE, RegWriteW, WriteRegW, RegWriteM, WriteRegM, RtE, RsE, fwunitrst, JrHaz, forwardA, forwardB, stallF, stallD, flushE);
endmodule











