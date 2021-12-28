// Main Control Unit

module ControlUnit(
    input rst, // reset signal
    input [5:0] op, // op code of an instruction
    input [5:0] funct, // funct code of an instruciton
    input clk, // clock
    // control signals
    output reg bneD,  // bne instruction ctr signal
    output reg Jreg, // Jr ctr 
    output reg Jump, // J/Jal ctr
    output reg MemReadD, // lw ctr (reading memory)
    output reg JalWrite, // write to register[31] ctr
    output reg SftD, // shift data ctr
    output reg RegWriteD, // write back to register ctr
    output reg MemtoRegD, // data writing back to register ctr
    output reg MemWriteD, // write memory ctr
    output reg BranchD, // branch ctr
    output reg [3:0] ALUctrD, // ALU operation type ctr
    output reg ALUSrcD, // ALU input ctr
    output reg RegDstD, // write register address ctr
    output reg EndD  // end program ctr
);

// reset state
always @(posedge rst) begin
    Jreg <= 0;
    Jump <= 0;
    MemReadD <= 0;
    JalWrite <= 0;
    SftD <= 0;
    RegWriteD <= 0;
    MemtoRegD <= 0;
    MemWriteD <= 0;
    BranchD <= 0;
    ALUctrD <= 0;
    ALUSrcD <= 0;
    RegDstD <= 0;
    EndD <= 0; 
    bneD <= 0;
end

// negative-edge triggered
always @(negedge clk) begin
    case (op)
        6'b111111: begin // end instruction
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 0;
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 0;
            RegDstD <= 0;
            EndD <= 1; 
            bneD <= 0;
        end

        6'b101011: begin // sw
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 0;
            MemtoRegD <= 0;
            MemWriteD <= 1; // store word in memory
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end
        
        6'b100011: begin // lw
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 1; // read from mem
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 1; // write back
            MemtoRegD <= 1;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end

        6'b001001: begin // addiu
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 1; // calculate and write back
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end

        6'b001000: begin // addi
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 1; // calculate and write back
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b001100: begin // andi
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 1; // calculate and write back
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 2;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b001101: begin // ori
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 1; // calculate and write back
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 4;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b001110: begin // xori
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 1; // calculate and write back
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 5;
            ALUSrcD <= 1;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b000101: begin // bne
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 0;
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 1; // branch signal
            ALUctrD <= 10;
            ALUSrcD <= 0;
            RegDstD <= 1;
            EndD <= 0;
            bneD <= 1;
        end 

        6'b000100: begin // beq
            Jreg <= 0;
            Jump <= 0;
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 0;
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 1; // branch signal
            ALUctrD <= 1;
            ALUSrcD <= 0;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b000011: begin // jal
            Jreg <= 0;
            Jump <= 1; // jump signal
            MemReadD <= 0;
            JalWrite <= 1; // write to $ra
            SftD <= 0;
            RegWriteD <= 1;
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 0;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b000010: begin // j
            Jreg <= 0;
            Jump <= 1; // jump signal
            MemReadD <= 0;
            JalWrite <= 0;
            SftD <= 0;
            RegWriteD <= 0;
            MemtoRegD <= 0;
            MemWriteD <= 0;
            BranchD <= 0;
            ALUctrD <= 0;
            ALUSrcD <= 0;
            RegDstD <= 0;
            EndD <= 0;
            bneD <= 0;
        end 

        6'b000000: begin
            case (funct)
                6'b100000: begin // add
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 0;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end
                
                6'b100001: begin // addu
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 0;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b100010: begin // sub
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 1;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b100011: begin // subu
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 1;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b100100: begin // and
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 2;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b100111: begin // nor
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 3;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b100101: begin // or
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 4;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b100110: begin // xor
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 5;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b000010: begin // srl
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 1; // shift amount considered
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 7;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b000110: begin // srlv
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 7;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b000000: begin // sll
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 1;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 6;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b000100: begin // sllv
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 6;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b000011: begin // sra
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 1; // shift amount considered
                    RegWriteD <= 1;
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 8;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b000111: begin // srav
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 8;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b101010: begin // slt
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 1; // write res back
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 9;
                    ALUSrcD <= 0;
                    RegDstD <= 1;
                    EndD <= 0;
                    bneD <= 0;
                end

                6'b001000: begin // jr
                    Jreg <= 1; // jump regisre ctr signal
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 0;
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 0;
                    ALUSrcD <= 0;
                    RegDstD <= 0;
                    EndD <= 0;
                    bneD <= 0;
                end

                // Bubble/invalid operation (for bubble: op=000000, funct=111111, regarded as default case too)
                default: begin 
                    Jreg <= 0;
                    Jump <= 0;
                    MemReadD <= 0;
                    JalWrite <= 0;
                    SftD <= 0;
                    RegWriteD <= 0;
                    MemtoRegD <= 0;
                    MemWriteD <= 0;
                    BranchD <= 0;
                    ALUctrD <= 0;
                    ALUSrcD <= 0;
                    RegDstD <= 0;
                    EndD <= 0;
                    bneD <= 0;
                end
            endcase
        end
    endcase
end
endmodule


