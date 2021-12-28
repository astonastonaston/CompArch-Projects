// WB/IF pipeline register
module WBIFppreg
  (
      // Inputs
      input [31:0] PC // Program Counter
    , input clk // clock
    , input rst // reset signal
    , input stallF
      // Outputs
    , output reg [31:0] PCF // fetched Program Counter
  );
  always @(posedge clk) begin
    if (~stallF) begin
      PCF <= PC;
    end
  end
  always @(posedge rst) begin
    PCF <= 0; // Bubble instruction
  end
endmodule


// IF/ID pipeline register
module IFIDppreg
  (
      // Inputs
    input JrHaz, // hazard of jr detector
    input jrFlush // jr flush ctr signal 
    , input [31:0] InstrF // Program Counter
    , input [31:0] PCPlus4F // PC + 4 
    , input clk // clock
    , input jumpFlush // jump as a Flush signal
    , input rst // reset signal
    , input shouldBranch
    , input stallD
      // Outputs
    , output reg [31:0] InstrD // fetched Program Counter
    , output reg [31:0] PCPlus4D // PC + 4 
  );
  always @(posedge clk) begin
    if (~stallD) begin
      // give bubble instruction if flushing
      // flushing when: 
      // 1. Jr flush and not encountering Jr data hazard 
      // 2. J/Jal execution
      // 3. Branch execution
      InstrD <= (((jrFlush && ~JrHaz) | jumpFlush | shouldBranch) == 0? InstrF: 32'b0000_0000_0000_0000_0000_0000_0011_1111); 
      PCPlus4D <= PCPlus4F;
    end
  end

  // reset state
  always @(posedge rst) begin
    // bubble ins
    InstrD <= 32'b0000_0000_0000_0000_0000_0000_0011_1111; 
    PCPlus4D <= 0; 
  end
endmodule


// ID/EX pipeline register
module IDEXppreg
  (
    // data inputs
    input rst,  // reset signal
    input clk // clock
    , input [4:0] RsD // Rs register in prev stage
    , input [31:0] RegD1 // value in reg instr[25:21]
    , input [31:0] RegD2 // value in reg instr[20:16]
    , input [4:0]  RtD // register destination for lw/sw
    , input [4:0]  RdD // register destination for R-type ins
    , input [31:0] SignImmD // Signed immediate from ins[15:0] 

    // ctr signal inputs
    , input       flushE // flush signal
    , input       MemReadD // judge whether trying to read mem
    , input       RegWriteD // write register enable 
    , input       MemtoRegD // select ALUOut/Memdata to register write data
    , input       MemWriteD // write memory enable
    , input [3:0] ALUControlD // ALUctr
    , input       ALUSrcD // select imm/register_value to ALU
    , input       RegDstD // select ins[20:16](lw, sw)/ins[15:11](others) as write destination register
    , input [4:0] SftaD // shift amount
    , input       EndD // end instr signal
    , input       SftD // shift control sig
      
    // data outputs
    , output reg [4:0] RsE
    , output reg [31:0] RegE1 // value in reg instr[25:21]
    , output reg [31:0] RegE2 // value in reg instr[20:16]
    , output reg [4:0]  RtE // register destination for lw/sw
    , output reg [4:0]  RdE // register destination for R-type ins
    , output reg [31:0] SignImmE // Signed immediate from ins[15:0] 
    , output reg MemReadE // judge whether trying to read mem
    , output reg RegWriteE // write register enable 
    , output reg MemtoRegE // select ALUOut/Memdata to register write data
    , output reg MemWriteE // write memory enable
    , output reg [3:0] ALUControlE // ALUctr
    , output reg ALUSrcE // select imm/register_value to ALU
    , output reg RegDstE // select ins[20:16](lw, sw)/ins[15:11](others) as write destination register
    , output reg [4:0] SftaE // shift ins, one indicating shift amount load
    , output reg EndE // end instr signal
    , output reg SftE // shift control sig

  );
  always @(posedge clk) begin
    if (~flushE) begin
      RegE1 <= RegD1;
      RegE2 <= RegD2;
      RsE <= RsD;
      RtE <= RtD;
      RdE <= RdD;
      SignImmE <= SignImmD;
      MemReadE <= MemReadD;
      RegWriteE <= RegWriteD;
      MemtoRegE <= MemtoRegD;
      MemWriteE <= MemWriteD;
      ALUControlE <= ALUControlD;
      ALUSrcE <= ALUSrcD;
      RegDstE <= RegDstD;
      SftaE <= SftaD;
      SftE <= SftD;
      EndE <= EndD;
    end
    else begin // flushing all ctr signals
      RegE1 <= 0;
      RegE2 <= 0;
      RsE <= 0;
      RtE <= 0;
      RdE <= 0;
      SignImmE <=  0;
      MemReadE <=  0;
      RegWriteE <= 0;
      MemtoRegE <= 0;
      MemWriteE <= 0;
      ALUControlE <= 0;
      ALUSrcE <= 0;
      RegDstE <= 0;
      SftaE <= 0;
      SftE <= 0;
      EndE <= 0;
    end
  end

  always @(posedge rst) begin // reset state
    RegE1 <= 0;
    RegE2 <= 0;
    RtE <= 0;
    RsE <= RsD;
    RdE <= 0;
    SignImmE <= 0;
    MemReadE <=  0;
    RegWriteE <= 0;
    MemtoRegE <= 0;
    MemWriteE <= 0;
    ALUControlE <= 0;
    ALUSrcE <=     0;
    RegDstE <=     0;
    SftaE <= 0;
    SftE <=  0;
    EndE <=  0;
  end
endmodule


// EX/MEM pipeline register
module EXMEMppreg
  (
    // Inputs
    input rst, // reset signal
    input clk // clock
    , input [31:0] ALUOutE // result of ALU calculation
    , input [31:0] WriteDataE // data to be written to register
    , input [4:0]  WriteRegE // write register destination address
    , input        MemReadE // judge whether trying to read mem
    , input        RegWriteE // write register enable 
    , input        MemtoRegE // select ALUOut/Memdata to register write data
    , input        MemWriteE // write memory enable
    , input        EndE // end instr signal

    // Outputs
    , output reg [31:0] ALUOutM // result of ALU calculation
    , output reg [31:0] WriteDataM // data to be written to register
    , output reg [4:0]  WriteRegM // write register destination address
    , output reg        MemReadM // judge whether trying to read mem
    , output reg        RegWriteM // write register enable 
    , output reg        MemtoRegM // select ALUOut/Memdata to register write data
    , output reg        MemWriteM // write memory enable
    , output reg        EndM // end instr signal
  );

  always @(posedge clk) begin
    ALUOutM <= ALUOutE;
    WriteDataM <= WriteDataE;
    WriteRegM <= WriteRegE;
    MemReadM <= MemReadE;
    RegWriteM <= RegWriteE;
    MemtoRegM <= MemtoRegE;
    MemWriteM <= MemWriteE;
    EndM <= EndE;
  end

  // reset state
  always @(posedge rst) begin
    ALUOutM <=    0;
    WriteDataM <= 0;
    WriteRegM <= 0;
    MemReadM <=  0;
    RegWriteM <= 0;
    MemtoRegM <= 0;
    MemWriteM <= 0;
    EndM <= 0;
  end
endmodule


// MEM/WB pipeline register
module MEMWBppreg
  (
    // data inputs
    input rst, // reset signal
    input clk // clock
    , input        RegWriteM // write register enable
    , input        MemtoRegM // select alu output or memory data to register
    , input [31:0] ALUOutM // result of ALU calculation 
    , input [31:0] ReadDataM // data read from main memory
    , input [4:0]  WriteRegM // register address to be written
    , input        EndM // end instr signal

    // data outputs
    , output reg        RegWriteW // write register enable
    , output reg        MemtoRegW // select alu output or memory data to register
    , output reg [31:0] ALUOutW // result of ALU calculation 
    , output reg [31:0] ReadDataW // data read from main memory
    , output reg [4:0]  WriteRegW // register address to be written
    , output reg        EndW // end instr signal
  );

  always @(posedge clk) begin
    RegWriteW <= RegWriteM;
    MemtoRegW <= MemtoRegM;
    ALUOutW <= ALUOutM;
    ReadDataW <= ReadDataM;
    WriteRegW <= WriteRegM;
    EndW <= EndM;
  end

  // reset state  
  always @(posedge rst) begin
    RegWriteW <= 0;
    MemtoRegW <= 0;
    ALUOutW <=   0;
    ReadDataW <= 0;
    WriteRegW <= 0;
    EndW <= 0;
  end
endmodule