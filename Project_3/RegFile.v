/* AUTOMATICALLY GENERATED VERILOG-2001 SOURCE CODE.
** GENERATED BY CLASH 1.2.0. DO NOT MODIFY.
*/
`timescale 1ns/100ps

module RegFile 
    ( // Inputs
      input  clk // clock
    , input  rst // reset
    , input  wriEn // write enable signal
    , input JalWrite // writing reg31 for jal ins
    , input [31:0] PCPlus4 // PC + 4 to write in 31st reg
    , input [4:0] wriRegAddr // the reg addr to write back
    , input [4:0] rdRegAddrOne // bit 25:21 reg read 
    , input [4:0] rdRegAddrTwo // bit 20:16 reg read
    , input [31:0] wriValue // the value to write back
      // Outputs
    , output reg [31:0] rdValOne // fetched register value One
    , output reg [31:0] rdValTwo // fetched register value Two
    );

  // register file declaration
  // REG_FILE[x] indicates the values in xth register
  reg [31:0] REG_FILE [31:0];

  // reset all registers to 0
  always @(rst) begin
    if (rst == 1) begin
      for (integer i = 0; i < 32; i++) begin
        REG_FILE[i] <= 32'b0000_0000_0000_0000_0000_0000_0000_0000;
      end
    end
  end

  always @(negedge clk) begin
    if (JalWrite) begin // meeting jal, write PC+4 to 31st reg
      REG_FILE[31] = PCPlus4 - 4;
    end
    if (wriEn == 1'b1) begin // write register value
      REG_FILE[wriRegAddr] = wriValue; 
    end // read register values
    rdValOne <= REG_FILE[rdRegAddrOne];
    rdValTwo <= REG_FILE[rdRegAddrTwo];
  end

endmodule







