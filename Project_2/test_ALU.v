`timescale 1ns/1ps
`include "./ALU.v"

module alu_test;
    // addr(regA) = 00000
    // addr(regB) = 00001

    wire[2:0] flgs; // output connection preparation
    // flag bits:
    // flg[0]: zero flag(bne, beq)
    // flg[1]: negative value flag
    // flg[2]: overflow flag
    
    wire[31:0] res;
    reg[31:0] Areg, Breg, ins; // input connection preparation

    task showstate(); // show the current state of alu 
        begin
        $display("Input:");
        $display("instruction(bin)\t\t\t:regA(hex)\t:regB(hex)");
        $display("%b\t:%h\t:%h",
        ins, Areg, Breg);
        $display("Output:");
        $display("instruction(bin)\t\t\t:regA(hex)\t:regB(hex)");
        $display("%b\t:%h\t:%h",
        ins, Areg, Breg);
        $display("flags(bin)\t:result(hex)\t\t");
        $display("%b\t\t:%h\t", flgs, res);
        $display();
        end
    endtask

    alu testalu( ins, Areg, Breg, res, flgs); 
    initial begin
        $display("hello, running starts!!!!");
        #15 showstate();

        // sll test
        $display("sll test: regB left shift 2 bits");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100;
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0001; // regB set 1
        ins <= 32'b000000_00000_00001_00000_00010_000000; // regB left shift 2 bits
        #15 showstate();

        // sllv test
        $display("sllv test: regB left shift regA bits");
        #10 ins <= 32'b000000_00000_00001_00000_00000_000100; // regB left shift regA bits
        #15 showstate();

        // srl test
        $display("srl test: regB right shift 2 bits");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100;
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0001_0001; // regB set 8
        ins<=32'b000000_00000_00001_00000_00010_000010; // regB right shift 2 bits
        #15 showstate();

        // srlv test
        $display("srlv test: regB right shift regA bits");
        #10 ins <= 32'b000000_00000_00001_00000_00000_000110; // regB right shift regA bits
        #15 showstate();

        // sra test
        $display("sra test: regB right shift arithmetically 2 bits");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100; // regA set 4
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0010_0001; // regB set 16
        ins<=32'b000000_00000_00001_00000_00010_000011; // regB right shift arithmetically 2 bits
        #15 showstate();

        // srav test
        $display("srav test: regB right shift arithmetically regA bits");
        #10 ins <= 32'b000000_00000_00001_00000_00000_000111; // regB right shift arithmetically regA bits
        #15 showstate();

        // add without overflow test
        $display("add without overflow test: regB + regA");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100; // regA set 4
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0001; // regB set 1
        ins <= 32'b000000_00000_00001_00000_00000_100000; // regB + regA
        #15 showstate();
        
        // add with positive overflow test
        $display("add with positive overflow test: regB + regA");
        #10 Breg <= 32'b0111_1111_1111_1111_1111_1111_1111_1110; // regB set max(int)
        #15 showstate();

        // add with negative overflow test
        $display("add with negative overflow test: regB + regA");
        #10 Areg <= 32'b1111_1111_1111_1111_1111_1111_1111_1111; // regA set -1
        Breg <= 32'b1000_0000_0000_0000_0000_0000_0000_0000; // regB set min(int)
        #15 showstate();

        // addu test
        $display("addu test: u'regB + u'regA");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100; // regA set 4
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0001; // regB set 1
        ins <= 32'b000000_00000_00001_00000_00000_100001; // u'regB + u'regA
        #15 showstate();
        
        // sub without overflow test
        $display("sub without overflow test: regA - regB");
        #10 ins <= 32'b000000_00000_00001_00000_00000_100010; // regA - regB
        #15 showstate();

        // sub with negative overflow test
        $display("sub with negative overflow test: regA - regB");
        #10 Areg <= 32'b1000_0000_0000_0000_0000_0000_0000_0000; // regA set min(int)
        #15 showstate();

        // sub with positive overflow test
        $display("sub with positive overflow test: regA - regB");
        #10 Areg <= 32'b0111_1111_1111_1111_1111_1111_1111_1111; // regA set max(int)
        Breg <= 32'b1111_1111_1111_1111_1111_1111_1111_1111; // regB set -1
        #15 showstate();

        // subu test
        $display("subu test: u'regA - u'regB");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100; // regA set 4
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0001; // regB set 1
        ins <= 32'b000000_00000_00001_00000_00000_100011; // u'regA - u'regB
        #15 showstate();
        
        // and test
        $display("and test: regA AND regB");
        #10 ins <= 32'b000000_00000_00001_00000_00000_100100; // regA AND regB
        #15 showstate();

        // nor test
        $display("nor test: regA NOR regB");
        #10 ins <= 32'b000000_00000_00001_00000_00000_100111; // regA NOR regB
        #15 showstate();

        // or test
        $display("or test: regA OR regB");
        #10 ins <= 32'b000000_00000_00001_00000_00000_100101; // regA OR regB
        #15 showstate();

        // xor test
        $display("xor test: regA XOR regB");
        #10 ins <= 32'b000000_00000_00001_00000_00000_100110; // regA XOR regB
        #15 showstate();

        // slt with negative flag test
        $display("slt with negative flag test: regA < regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100; // regA set 4
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_1000; // regB set 8
        ins <= 32'b000000_00000_00001_00000_00000_101010; // regA < regB 
        #15 showstate();

        // slt without negative flag test
        $display("slt without negative flag test: regA < regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_1100; // regA set 12
        #15 showstate();

        // sltu with negative flag test
        $display("sltu with negative flag test: u'regA < u'regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0100; // regA set 4
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0110; // regB set 6
        ins <= 32'b000000_00000_00001_00000_00000_101011; // u'regA < u'regB 
        #15 showstate();

        // sltu without negative flag test
        $display("sltu without negative flag test: u'regA < u'regB ?");
        #10 Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        #15 showstate();

        // beq with 0 flag test
        $display("beq with 0 flag test: regA == regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regA set 2
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b000100_00000_00001_00000_00000_000000; // regA == regB 
        #15 showstate();
        
        // beq without 0 flag test
        $display("beq without 0 flag test: regA == regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_1000; // regA set 8
        #15 showstate();

        // bne without 0 flag test
        $display("bne without 0 flag test: regA != regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regA set 2
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b000101_00000_00001_00000_00000_000000; // regA != regB 
        #15 showstate();
        
        // bne with 0 flag test
        $display("bne with 0 flag test: regA != regB ?");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0000; // regA set 0
        #15 showstate();

        // addiu test
        $display("addiu test: u'regA + 4");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0000; // regA set 0
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001001_00000_00001_0000000000000100; // u'regA + 4 
        #15 showstate();

        // addi without overflow test
        $display("addi without overflow test: regA + 2");
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_1000; // regA set 8
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001000_00000_00001_0000000000000010; // regA + 2 
        #15 showstate();

        // addi with positive overflow test
        $display("addi with positive overflow test: regA + 1");
        #10 Areg <= 32'b0111_1111_1111_1111_1111_1111_1111_1111; // regA set max(int)
        ins <= 32'b001000_00000_00001_0000000000000001;; // imm set 1
        #15 showstate();

        // addi with negative overflow test
        $display("addi with negative overflow test: regA + -1");
        #10 Areg <= 32'b1000_0000_0000_0000_0000_0000_0000_0000; // regA set min(int)
        ins <= 32'b001000_00000_00001_1111111111111111; // imm set -1
        #15 showstate();

        // slti with negative flag test 
        $display("slti with negative flag test: regA < 8 ?"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0110; // regA set 6
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001010_00000_00001_0000000000001000; // regA < 8 
        #15 showstate();

        // slti without negative flag test 
        $display("slti without negative flag test: regA < 8 ?"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_1001; // regA set 9
        #15 showstate();

        // sltiu with negative flag test 
        $display("sltiu with negative flag test: u'regA < u'8 ?"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0001; // regA set 1
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001011_00000_00001_0000000000001000; // u'regA < u'8 
        #15 showstate();

        // sltiu without negative flag test 
        $display("sltiu without negative flag test: u'regA < u'8 ?"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_1111; // regA set 15
        #15 showstate();

        // andi test 
        $display("andi test: 1010 AND 0111"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0111; // regA set 4'b0111
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001100_00000_00001_0000000000001010; // 4'b1010 AND 4'b0111 
        #15 showstate();

        // ori test 
        $display("ori test: 1010 OR 0111"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0111; // regA set 4'b0111
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001101_00000_00001_0000000000001010; // 4'b1010 OR 4'b0111 
        #15 showstate();

        // xori test 
        $display("xori test: 1010 XOR 0110"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_0110; // regA set 4'b0110
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b001110_00000_00001_0000000000001010; // 4'b1010 XOR 4'b0110 
        #15 showstate();

        // lw test 
        $display("lw test: expected result addr = (15(regA)+10(imm)) = 25 in decimal"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0000_1111; // regA set (15)4'b1111
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b100011_00000_00001_0000000000001010; // go for addr (15+10) 
        #15 showstate();

        // sw test 
        $display("sw test: expected result addr = (32(regA)+10(imm)) = 42 in decimal"); 
        #10 Areg <= 32'b0000_0000_0000_0000_0000_0000_0010_0000; // regA set 32
        Breg <= 32'b0000_0000_0000_0000_0000_0000_0000_0010; // regB set 2
        ins <= 32'b101011_00000_00001_0000000000001010; // go for addr (32+10) 
        #15 showstate();

        #10 $finish;
    end
endmodule