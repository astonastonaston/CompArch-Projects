module alu(instruction, regA, regB, result, flags);
    input[31:0] instruction, regA, regB;
    // address of regA: 00000
    // address of regB: 00001

    output[31:0] result;
    output[2:0] flags; 
    // flag bits:
    // flags[0]: zero flag(bne, beq)
    // flags[1]: negative value flag
    // flags[2]: overflow flag

    reg[31:0] result, regs[1:0]; // regs[1:0] is like a register file indexing register through address
    reg[5:0] opcode, func;
    reg[2:0] fgs; // temporal storage of flags
    
    // triggered by the change of instruction/regA/regB 
    always @(instruction, regA, regB) begin 
        opcode = instruction[31:26];
        func = instruction[5:0];
        fgs = 3'b000;
        regs[0] = regA;
        regs[1] = regB; 
        case(opcode)
            6'b000000: begin 
                case(func)
                    6'b000000: begin // sll
                        result = regs[instruction[20:16]] << instruction[10:6];
                    end

                    6'b000100: begin // sllv
                        result = regs[instruction[20:16]] << regs[instruction[25:21]];
                    end

                    6'b000010: begin // srl
                        result = regs[instruction[20:16]] >> instruction[10:6];
                    end

                    6'b000110: begin // srlv
                        result = regs[instruction[20:16]] >> regs[instruction[25:21]];
                    end

                    6'b000011: begin // sra
                        result = $signed(regs[instruction[20:16]]) >>> instruction[10:6];
                    end

                    6'b000111: begin // srav
                        result = $signed(regs[instruction[20:16]]) >>> regs[instruction[25:21]];
                    end

                    6'b100000: begin // add overflow flag consideration
                        result = regs[instruction[20:16]] + regs[instruction[25:21]];
                        fgs[2] = ((regs[instruction[20:16]][31] ^~ regs[instruction[25:21]][31]) &&
                            (regs[instruction[25:21]][31] ^ result[31]));
                    end

                    6'b100001: begin // addu
                        result = regs[instruction[20:16]] + regs[instruction[25:21]];
                    end

                    6'b100010: begin // sub overflow flag consideration
                        result = regs[instruction[25:21]] - regs[instruction[20:16]];
                        fgs[2] = ((regs[instruction[20:16]][31] ^ regs[instruction[25:21]][31]) && 
                            (regs[instruction[25:21]][31] ^ result[31]))? 1: 0;
                    end

                    6'b100011: begin // subu
                        result = regs[instruction[25:21]] - regs[instruction[20:16]];
                    end

                    6'b100100: begin // and
                        result = regs[instruction[25:21]] & regs[instruction[20:16]];
                    end

                    6'b100111: begin // nor
                        result = regs[instruction[25:21]] ~| regs[instruction[20:16]];
                    end

                    6'b100101: begin // or
                        result = regs[instruction[25:21]] | regs[instruction[20:16]];
                    end

                    6'b100110: begin // xor
                        result = regs[instruction[25:21]] ^ regs[instruction[20:16]];
                    end

                    6'b101010: begin // slt - flag consideration
                        fgs[1] = ($signed(regs[instruction[25:21]]) < $signed(regs[instruction[20:16]])); 
                    end

                    6'b101011: begin // sltu - flag consideration
                        fgs[1] = (regs[instruction[25:21]] < regs[instruction[20:16]]);
                    end
                endcase
            end

            6'b000100: begin // beq 0 flag consideration
                fgs[0] = (regs[instruction[25:21]] == regs[instruction[20:16]]);
            end
            
            6'b000101: begin // bne 0 flag consideration
                fgs[0] = (regs[instruction[25:21]] != regs[instruction[20:16]]);
            end
            
            6'b001000: begin // addi overflow flag consideration
                result = $signed(instruction[15:0]) + $signed(regs[instruction[25:21]]);
                fgs[2] = ((instruction[15] ^~ regs[instruction[25:21]][31]) && 
                    (regs[instruction[25:21]][31] ^ result[31]))? 1: 0; 
            end
            
            6'b001001: begin // addiu
                result = instruction[15:0] + regs[instruction[25:21]];
            end
            
            6'b001010: begin // slti - flag consideration
                fgs[1] = ($signed(regs[instruction[25:21]]) < $signed(instruction[15:0]));
                
            end
            
            6'b001011: begin // sltiu - flag consideration
                fgs[1] = (regs[instruction[25:21]] < instruction[15:0]);
            end
            
            6'b001100: begin // andi
                result = $signed(regs[instruction[25:21]]) & $signed(instruction[15:0]);
            end
            
            6'b001101: begin // ori
                result = $signed(regs[instruction[25:21]]) | $signed(instruction[15:0]);
            end
            
            6'b001110: begin // xori
                result = $signed(regs[instruction[25:21]]) ^ $signed(instruction[15:0]);
            end
            
            6'b100011, 6'b101011: begin // lw // sw
                result = $signed(regs[instruction[25:21]]) + $signed(instruction[15:0]);
            end
        endcase
    end
    assign flags = fgs;
endmodule


