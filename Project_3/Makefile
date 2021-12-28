G = iverilog
BASFILE = test_CPU.v CPU.v ALU.v Control.v InstructionRAM.v MainMemory.v ppregs.v RegFile.v Basics.v BranchHazDet.v DataHazDet.v

test.out: $(BASFILE)
	$(G) -o test.out $^ 

