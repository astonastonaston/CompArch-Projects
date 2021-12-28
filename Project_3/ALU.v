// Arithmetics and Logic Unit

`timescale 1fs/1fs
module ALU
    ( // Inputs
    input rst, // reset
    input  clk // clock, triggered by negedge
    , input [31:0] SrcAE // data source A 
    , input [31:0] SrcBE // data source B
    , input [3:0] ALUctr // 4-bit ALU control

      // Outputs
    , output reg [31:0] res // arithmetic result
    );

always @(posedge rst) begin // reset ALU
  res <= 0;
end

always @(negedge clk) begin
    // ALUctr: 
    // 0000 -> add/ Unused
    // 0001 -> sub, beq
    // 0010 -> and
    // 0011 -> nor
    // 0100 -> or
    // 0101 -> xor
    // 0110 -> sll
    // 0111 -> srl
    // 1000 -> sra
    // 1001 -> slt
    // 1010 -> bne
    // j, jal, jr categorized to invalid
    case (ALUctr)
      4'b0000: res = SrcAE + SrcBE; // add (reset-state invalid)
      4'b0001, 4'b1010: res = SrcAE - SrcBE; // sub 
      4'b0010: res = SrcAE & SrcBE; // and/Unused
      4'b0011: res = ~(SrcAE | SrcBE); // nor
      4'b0100: res = SrcAE | SrcBE; // or
      4'b0101: res = SrcAE ^ SrcBE; // xor
      4'b0110: res = SrcBE << SrcAE[4:0]; // sll
      4'b0111: res = SrcBE >> SrcAE[4:0]; // srl
      4'b1000: res = $signed(SrcBE) >>> $signed(SrcAE[4:0]); // sra
      4'b1001: res = ($signed(SrcAE - SrcBE) < 0? 1: 0); // slt
      default: res = 999999999; // invalid ALUctr 
    endcase
end

endmodule







