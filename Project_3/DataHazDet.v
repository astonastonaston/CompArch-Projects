module DataHazardDetUnit(
    input branchD, // branch ctr
    input Jreg, // jr ctr
    input [4:0] writeregE, // write register address
    input [4:0] RsD, // 1st read register address at ID/EX
    input [4:0] IFIDRt, // 2nd read register address at IF/ID
    input [4:0] IFIDRs, // 1st read register address at IF/ID
    input memrd, // read mem ctr
    input MEMWBregwrite, // write mem ctr
    input [4:0] MEMWBregrd, // write register addr at MEM/WB
    input EXMEMregwrite, // write reg ctr EX/MEM
    input [4:0] EXMEMregrd, // write reg addr at EX/MEM 
    input [4:0] IDEXregrt, // 2nd read reg addr at ID/EX
    input [4:0] IDEXregrs, // 1st read reg addr at ID/EX
    input rst, // reset signal
    output reg JrHaz, // ctr signal to stop flushing in IFID ppreg
    output reg [1:0] forwardAE, // forward selection for 1st ALU input
    output reg [1:0] forwardBE, // forward selection for 2nd ALU input
    output reg stallF, // IF stalling ctr (for stalling)
    output reg stallD, // ID stalling ctr (for stalling)
    output reg flushE // EX flushing ctr (for stalling)
);

// reset state
always @(posedge rst) begin
    forwardAE <= 0;
    forwardBE <= 0;
end

always @(*) begin
    // beq/bne data hazard stalling logic 
    if (branchD && ((writeregE == IFIDRs) || (writeregE == IFIDRt))) begin
        JrHaz = 0;
        stallF = 1;
        stallD = 1;
        flushE = 1;
    end
    else if (branchD && ((EXMEMregrd == IFIDRs) || (EXMEMregrd == IFIDRt))) begin
        JrHaz = 0;
        stallF = 1;
        stallD = 1;
        flushE = 1;
    end
    else if (branchD && ((MEMWBregrd == IFIDRs) || (MEMWBregrd == IFIDRt))) begin
        JrHaz = 0;
        stallF = 1;
        stallD = 1;
        flushE = 1;
    end

    // jr data hazard stalling logic
    else if (Jreg && (writeregE == RsD)) begin
        JrHaz = 1;
        stallF = 1;
        stallD = 1;
        flushE = 1;
    end
    else if (Jreg && (EXMEMregrd == RsD)) begin
        JrHaz = 1;
        stallF = 1;
        stallD = 1;
        flushE = 1;
    end
    else if (Jreg && (MEMWBregrd == RsD)) begin
        JrHaz = 1;
        stallF = 1;
        stallD = 1;
        flushE = 1;
    end

    // lw stalling logic
    else if (memrd && ((IDEXregrt == IFIDRs) || (IDEXregrt == IFIDRt))) begin
        stallF = 1;
        stallD = 1;
        flushE = 1;
        JrHaz = 0;
    end
    else begin
        stallF = 0;
        stallD = 0;
        flushE = 0;
        JrHaz = 0;
    end

    // forward from DM to Rs
    if (EXMEMregwrite && (EXMEMregrd != 0) && (EXMEMregrd == IDEXregrs)) begin
        forwardAE <= 2'b10;
    end

    // forward from WB to Rs
    else if (MEMWBregwrite && (MEMWBregrd != 0) && (MEMWBregrd == IDEXregrs)) begin
        forwardAE <= 2'b01;
    end

    // default: no forwarding
    else begin
        forwardAE <= 2'b00;
    end

    // forward from DM to Rt
    if (EXMEMregwrite && (EXMEMregrd != 0) && (EXMEMregrd == IDEXregrt)) begin
        forwardBE <= 2'b10;
    end

    // forward from WB to Rt
    else if (MEMWBregwrite && (MEMWBregrd != 0) && (MEMWBregrd == IDEXregrt)) begin
        forwardBE = 2'b01;
    end

    // default: no forwarding
    else begin
        forwardBE <= 2'b00;
    end

end
    
endmodule