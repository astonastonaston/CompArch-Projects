// Branch Hazard Detection Unit

// Equal data detector:
// for branch hazard detection
module equalDet(
    input [31:0] dataA,
    input [31:0] dataB,
    output reg equal
);
    always @(dataA, dataB) begin
        if (dataA == dataB) begin
            equal = 1;
        end
        else begin
            equal = 0;
        end
    end
endmodule

// branch hazard detector IC
module DetectIC(
    input bned,
    input BranchD,
    input equal,
    output reg shouldBranch
);

always @(bned, BranchD, equal) begin
    // need branch & (bne/beq condition satisfied)
    shouldBranch = BranchD & (bned ^ equal); 
end
    
endmodule


