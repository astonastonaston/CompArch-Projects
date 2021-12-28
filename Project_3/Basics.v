// Simple basic components

`timescale 100fs/100fs

// Adder
module adder 
    ( // Inputs
      input[31:0] A
    , input[31:0] B

      // Outputs
    , output reg [31:0] res
    );
always @(A, B) begin
    res <= A + B;
end
endmodule

// 3-way MUX
module ThreeMux(
  input [31:0] up,
  input [31:0] mid,
  input [31:0] dn,
  input [1:0] sel,
  output reg [31:0] res
);
  always @(up, mid, dn, sel) begin
    case (sel) // selection based on sel
      00: res <= up;
      01: res <= mid;
      10: res <= dn;
      default: begin
        res <= dn; // invalid input (default state): select down road
      end
    endcase
  end
endmodule

// sign extender
module signExtender 
    ( // Inputs
      input[5:0] op,
      input[15:0] A
      // Output
    , output reg [31:0] res
    );
always @(A) begin
    case (op) // extend depending on opcode
      // arithmetic sign-extension
      6'b001000, 6'b001001, 6'b000101, 6'b000100: begin 
        if (A[15] == 1'b1) begin
          res <= {16'b1111_1111_1111_1111, A};
        end
        else begin
          res <= {16'b0000_0000_0000_0000, A};
        end
      end

      // logical operation 0-extension and branch addr calculation
      default: begin
        res <= {16'b0000_0000_0000_0000, A};
      end
    endcase
end
endmodule


// 2-way multiplexer
// for register address selection
module twoMux_FiveBit 
    ( // Inputs
      input[4:0] uproad
    , input[4:0] dnroad
    , input ctr
      // Output
    , output reg [4:0] res
    );
always @(uproad, ctr, dnroad) begin
    res <= (ctr == 1'b0 ? uproad: dnroad);
end
endmodule


// 2-way multiplexer
module twoMux 
    ( // Inputs
      input[31:0] uproad
    , input[31:0] dnroad
    , input ctr
      // Output
    , output reg [31:0] res
    );
always @(uproad, ctr, dnroad) begin
    res <= (ctr == 1'b0 ? uproad: dnroad);
end
endmodule


// 2-bit shifter
module twobitshifter 
    ( // Inputs
      input[31:0] imm
      // Output
    , output reg [31:0] res
    );
always @(imm) begin
    res <= (imm <<< 2);
end
endmodule


// address concatenator
// for calculating target jump address
module addrconca 
    ( // Inputs
      input[27:0] immprt // jumpIns[25:0] << 2
    , input[3:0] PCprt // PC[31:28]
      // Output
    , output reg [31:0] jmpaddr // jump target address
    );
always @(immprt, PCprt) begin
    jmpaddr <= {PCprt, immprt};
end
endmodule


// AND gate
// for branch signal judgement
module ANDgate(
  input a,
  input b,
  output reg c
);
  always @(a, b) begin
    c <= a & b;
    if (c) begin
      $display("Jump now!");
    end
  end
endmodule

