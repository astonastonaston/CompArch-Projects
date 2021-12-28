// jj1.cpp : including "main" function. Assembling and simulating a MIPS program

#include <iostream>
#include <string>
#include <fstream>	
#include <iostream>
#include <cstdio>
#include <typeinfo>
#include <map>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

//using bit fields to store instructions
typedef struct {
	unsigned opcode : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	unsigned rd : 5;
	unsigned shamt : 5;
	unsigned funct : 6;
} RIns;

typedef struct {
	unsigned opcode : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	unsigned imm : 16;
} IIns;

typedef struct {
	unsigned opcode : 6;
	unsigned target : 26;
} JIns;

typedef struct {
	unsigned syscall : 32;
} SysIns;

// the number difference to the first instruction
extern int insct = 0;

// address of the first instruction (unit: word) in text segment
const int initaddr = (int)pow(2, 20);

// allocate space for instruction
typedef char EmpIns[4];

// recording instruction type
enum insType { Rtype, Itype, Jtype, Systype };

// input/output file stream
std::ifstream inFile;
std::ofstream outFile;

// register information storage: 
// {register name(string): register number}
map <string, int> RegMap;


// label information storage:
// {label: difference with 1st instruction}
// Recording difference to the first instruction.
// When invoking, add it to the address of the first instruction to gain simulated label address. 
map <string, int> LabMap;


// assembling-assistant function: 
// input: mips row pointer，pointer to an empty instruction，instruction type
// output：changed insType and insptr
void assemble(EmpIns* insptr, insType& type, string& currlin);

// main assembling function
int Assemble(string inpath, string outpath);

// adding an R/I/J/Syscall instruction (for convenience of printing out)
void AddR(RIns* ins, unsigned opcode, unsigned rs, unsigned rt, unsigned rd, unsigned shamt, unsigned funct) {
	ins->opcode = opcode;
	ins->rs = rs;
	ins->rt = rt;
	ins->rd = rd;
	ins->shamt = shamt;
	ins->funct = funct;
	//std::printf("opcode: %d rd rs rt: %d %d %d", ins->opcode, ins->rd, ins->rs, ins->rt);
}

void AddI(IIns* ins, unsigned opcode, unsigned rs, unsigned rt, unsigned imm) {
	ins->opcode = opcode;
	ins->rs = rs;
	ins->rt = rt;
	ins->imm = imm;
}

void AddJ(JIns* ins, unsigned opcode, unsigned target) {
	ins->opcode = opcode;
	ins->target = target;
}

void AddSys(SysIns* ins) {
	ins->syscall = 12;
}

// initialize regMap
void regMapInit(map <string, int>& rgmap) {
	rgmap["$zero"] = 0;
	rgmap["$at"] = 1;
	rgmap["$v0"] = 2;
	rgmap["$v1"] = 3;
	rgmap["$a0"] = 4;
	rgmap["$a1"] = 5;
	rgmap["$a2"] = 6;
	rgmap["$a3"] = 7;
	rgmap["$t0"] = 8;
	rgmap["$t1"] = 9;
	rgmap["$t2"] = 10;
	rgmap["$t3"] = 11;
	rgmap["$t4"] = 12;
	rgmap["$t5"] = 13;
	rgmap["$t6"] = 14;
	rgmap["$t7"] = 15;
	rgmap["$s0"] = 16;
	rgmap["$s1"] = 17;
	rgmap["$s2"] = 18;
	rgmap["$s3"] = 19;
	rgmap["$s4"] = 20;
	rgmap["$s5"] = 21;
	rgmap["$s6"] = 22;
	rgmap["$s7"] = 23;
	rgmap["$t8"] = 24;
	rgmap["$t9"] = 25;
	rgmap["$k0"] = 26;
	rgmap["$k1"] = 27;
	rgmap["$gp"] = 28;
	rgmap["$sp"] = 29;
	rgmap["$fp"] = 30;
	rgmap["$ra"] = 31;
}

// printing an R/I/J instruction 
void printJIns(EmpIns* ori) {
	JIns nw;
	JIns* ins = (JIns*)ori;
	nw.opcode = ins->opcode;
	nw.target = ins->target;
	ins = &nw;

	uint32_t* value = (uint32_t*)ins;
	int* res = new int[32];
	int i;

	for (i = 5; i >= 0; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 31; i >= 6; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 0; i <= 31; i++) {
		outFile << res[i];
	}
	outFile << endl;
	delete[] res;
}

void printIIns(EmpIns* ori) {
	IIns nw;
	IIns* ins = (IIns*)ori;
	nw.opcode = ins->opcode;
	nw.rs = ins->rs;
	nw.rt = ins->rt;
	nw.imm = ins->imm;
	ins = &nw;

	uint32_t* value = (uint32_t*)ins;
	int* res = new int[32];
	int i;

	for (i = 5; i >= 0; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 10; i >= 6; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 15; i >= 11; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 31; i >= 16; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 0; i <= 31; i++) {
		outFile << res[i];
	}
	outFile << endl;
	delete[] res;
}

void printRIns(EmpIns* ori) {
	RIns nw;
	RIns* ins = (RIns*)ori;
	nw.opcode = ins->opcode;
	nw.rs = ins->rs;
	nw.rt = ins->rt;
	nw.rd = ins->rd;
	nw.shamt = ins->shamt;
	nw.funct = ins->funct;
	ins = &nw;

	uint32_t* value = (uint32_t*)ins;
	int* res = new int[32];
	int i;

	for (i = 5; i >= 0; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 10; i >= 6; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 15; i >= 11; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 20; i >= 16; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 25; i >= 21; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 31; i >= 26; i--) {
		res[i] = (*value & 1);
		*value = *value >> 1;
	}

	for (i = 0; i <= 31; i++) {
		outFile << res[i];
	}
	outFile << endl;
	delete[] res;
}

// for input formatting: 
// trim space on the two sides
void trim(string& lin) {
	if (lin.empty()) {
		return;
	}
	lin.erase(0, lin.find_first_not_of(" "));
	lin.erase(lin.find_last_not_of(" ") + 1);
}

// remove comments
void rmvCom(string& currin) {
	currin = currin.substr(0, currin.find('#'));
}

// eliminate selected escape characters in current line 
void killEsCharSelect(string& currin, char esca) {
	// kill \t in two sides of the current line and preserve the \t in .ascii str
	int a = currin.find_first_of(esca);
	while ((a < currin.find_first_of('"') || a > currin.find_last_of('"')) && a != string::npos) {
		currin.erase(a, 1);
		a = currin.find_first_of(esca);
	}

	a = currin.find_last_of(esca);
	while ((a < currin.find_first_of('"') || a > currin.find_last_of('"')) && a != string::npos) {
		currin.erase(a, 1);
		a = currin.find_first_of(esca);
	}
}

// store label in current line
void stolab(string& currin) {
	if (currin.find(':') != string::npos) {
		string lab;
		lab = currin.substr(currin.find_first_not_of(" "), currin.find_first_of(":"));
		LabMap[lab] = insct;
	}
}

// eliminate label in current line
void killLab(string& currin) {
	if (currin.find(':') != string::npos)
		currin = currin.substr(currin.find(':') + 1, string::npos);
}

// first-time formatting (for storing labels particularly)
void firstformatinp(string& currin) {
	rmvCom(currin); // remove the comments
	killEsCharSelect(currin, '\t');
	stolab(currin); // store the label addresses
	killLab(currin);
	trim(currin);
	if (currin != "") insct++;
}

// second-time formatting (for eliminating redundant elements and executing)
void secondformatinp(string& currin) {
	rmvCom(currin); // remove the comments
	killLab(currin); // store the label addresses
	killEsCharSelect(currin, '\t');
	trim(currin); // remove empty space on two sides
}

// extract elements of certain format instruction 
// Type: clo rd, rs
void cloTypeExtract(string& rd, string& rs, string& currlin) {
	bool rflag = false;
	string::iterator itr = currlin.begin();

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rd += *itr;
		}
		itr++;
	}

	rd = rd.substr(0, rd.find(' '));
	rflag = false;
	itr++;

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rs += *itr;
		}
		itr++;
	}

	rs = rs.substr(0, rs.find(' '));
}

// Type: addiu rt, rs, imm
void AddiuTypeExtract(string& rt, string& rs, string& imm, string& currlin) {
	bool rflag = false;
	string::iterator itr = currlin.begin();

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rt += *itr;
		}
		itr++;
	}

	rt = rt.substr(0, rt.find(' '));
	rflag = false;
	itr++;

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rs += *itr;
		}
		itr++;
	}

	rs = rs.substr(0, rs.find(' '));
	rflag = false;
	itr++;

	while (itr != currlin.end()) {
		if (*itr != ' ') { // go and find immediate
			rflag = true;
		}
		if (rflag) {
			imm += *itr;
		}
		itr++;
	}
	imm = imm.substr(0, rs.find(' '));
	rflag = false;
}

// Type: add rd, rs, rt
void addTypeExtract(string& rd, string& rs, string& rt, string& currlin) {
	bool rflag = false;
	string::iterator itr = currlin.begin();

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rd += *itr;
		}
		itr++;
	}

	rd = rd.substr(0, rd.find(' '));
	rflag = false;
	itr++;

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rs += *itr;
		}
		itr++;
	}

	rs = rs.substr(0, rs.find(' '));
	rflag = false;
	itr++;

	while (itr != currlin.end()) {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rt += *itr;
		}
		itr++;
	}

	rt = rt.substr(0, rt.find(' '));
}

// Type: teqi rs, imm
void teqiTypeExtract(string& rs, string& imm, string& currlin) {
	bool rflag = false;
	string::iterator itr = currlin.begin();

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rs += *itr;
		}
		itr++;
	}

	rs = rs.substr(0, rs.find(' '));

	imm = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);
}

// Type: lw rt, address
void lwTypeExtract(string& rt, string& rs, string& addr, string& currlin) {
	bool rflag = false;
	string::iterator itr = currlin.begin();

	while (*itr != ',') {
		if (*itr == '$') {
			rflag = true;
		}
		if (rflag) {
			rt += *itr;
		}
		itr++;
	}
	//avoid the case where " " is between register and ","
	rflag = false;
	itr++;

	while (*itr != '(') {
		if (*itr != ' ') {
			rflag = true;
		}
		if (rflag) {
			addr += *itr;
		}
		itr++;
	}
	itr++;

	while (*itr != ')') {
		rs += *itr;
		itr++;
	}//avoid the case where " " is between register and ","
}

void assemble(EmpIns* insptr, insType& type, string& currlin) {
	insct++; //difference between PC+4 and the first instruction

	string det = currlin.substr(0, currlin.find_first_of(' '));
	//assemble according to function type
	switch (det[0]) //function-based assembling
	{
	case 'a':
		switch (det[det.length() - 1])
		{
		case 'd':
			switch (det[det.length() - 2])
			{
			case 'n': //and
			{
				string rd, rs, rt;

				addTypeExtract(rd, rs, rt, currlin);

				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 36);

				printRIns(insptr);
				
				break;
			}
			case 'd': //add
			{
				string rd, rs, rt;

				addTypeExtract(rd, rs, rt, currlin);

				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 32);

				printRIns(insptr);
				break;
			}
			}
			break;
		case 'u':
			switch (det[det.length() - 2])
			{
			case 'd': //addu
			{
				string rd, rs, rt;

				addTypeExtract(rd, rs, rt, currlin);

				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 33);

				printRIns(insptr);
				
				break;
			}
			case 'i': //addiu
			{
				string rs, rt, imm;
				AddiuTypeExtract(rt, rs, imm, currlin);

				AddI((IIns*)insptr, 9, RegMap[rs], RegMap[rt], stoi(imm));

				printIIns(insptr);
				
				break;
			}
			}
			break;
		case 'i':
			switch (det[1])
			{
			case 'n': //andi
			{
				string rs, rt, imm;

				AddiuTypeExtract(rt, rs, imm, currlin);
				AddI((IIns*)insptr, 12, RegMap[rs], RegMap[rt], stoi(imm));

				printIIns(insptr);
				std::printf("\n");
				break;
			}
			case 'd': //addi
			{
				string rs, rt, imm;

				AddiuTypeExtract(rt, rs, imm, currlin);
				AddI((IIns*)insptr, 8, RegMap[rs], RegMap[rt], stoi(imm));

				printIIns(insptr);
				
				break;
			}
			}
			break;
		}
		break;
	case 'c': //clo //clz
		{
			string rd, rs;
			cloTypeExtract(rd, rs, currlin);
			if (det[det.length() - 1] == 'o') AddR((RIns*)insptr, 28, RegMap[rs], 0, RegMap[rd], 0, 33);
			else AddR((RIns*)insptr, 28, RegMap[rs], 0, RegMap[rd], 0, 32);
			printRIns(insptr);
			
			break;
		}
	case 'd':
		switch (det[det.length() - 1])
		{
		case 'v': //div
		{
			string rt, rs;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 26);

			printRIns(insptr);
			
			break;
		}
		case 'u': //divu
		{
			string rt, rs;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 27);

			printRIns(insptr);
			
			break;
		}
		}
		break;
	case 'm':
		switch (det[det.length() - 1])
		{
		case 't': //mult
		{
			string rt, rs;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 24);

			printRIns(insptr);
			
			break;
		}
		case 'u':
			switch (det[det.length() - 2])
			{
			case 't': //multu
			{
				string rt, rs;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 25);

				printRIns(insptr);
				
				break;
			}
			case 'd': //maddu
			{
				string rt, rs;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 28, RegMap[rs], RegMap[rt], 0, 0, 1);

				printRIns(insptr);
				
				break;
			}
			case 'b': //msubu
			{
				string rt, rs;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 28, RegMap[rs], RegMap[rt], 0, 0, 5);

				printRIns(insptr);
				
				break;
			}
			}
			break;
		case 'l': //mul
		{
			string rd, rs, rt;

			addTypeExtract(rd, rs, rt, currlin);
			AddR((RIns*)insptr, 28, RegMap[rs], RegMap[rt], RegMap[rd], 0, 2);

			printRIns(insptr);
			
			break;
		}
		case 'd': //madd
		{
			string rt, rs;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 28, RegMap[rs], RegMap[rt], 0, 0, 0);

			printRIns(insptr);
			
			break;
		}
		case 'b': //msub
		{
			string rt, rs;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 28, RegMap[rs], RegMap[rt], 0, 0, 4);

			printRIns(insptr);
			
			break;
		}
		case 'i':
			switch (det[1]) 
			{
			case 'f': //mfhi
			{
				string rd = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);

				AddR((RIns*)insptr, 0, 0, 0, RegMap[rd], 0, 16);

				printRIns(insptr);
				
				break;
			}
			case 't': //mthi
			{
				string rs = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);

				AddR((RIns*)insptr, 0, RegMap[rs], 0, 0, 0, 17);

				printRIns(insptr);
				
				break;
			}
			}
			break;
		case 'o':
			switch (det[1])
			{
			case 'f': //mflo
			{
				string rd = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);

				AddR((RIns*)insptr, 0, 0, 0, RegMap[rd], 0, 18);

				printRIns(insptr);
				
				break;
			}
			case 't': //mtlo
			{
				string rs = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);

				AddR((RIns*)insptr, 0, RegMap[rs], 0, 0, 0, 19);

				printRIns(insptr);
				
				break;
			}
			}
			break;
		}
		break;
	case 'n': //nor
	{
		string rd, rs, rt;

		addTypeExtract(rd, rs, rt, currlin);
		AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 39);

		printRIns(insptr);
		
		break;
	}
	case 'o':
		switch (det[det.length() - 1])
		{
		case 'r': //or
		{
			string rd, rs, rt;

			addTypeExtract(rd, rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 37);

			printRIns(insptr);
			
			break;
		}
		case 'i': //ori
		{
			string rs, imm, rt;

			AddiuTypeExtract(rt, rs, imm, currlin);
			AddI((IIns*)insptr, 13, RegMap[rs], RegMap[rt], stoi(imm));

			printIIns(insptr);
			
			break;
		}
		}
		break;
	case 's':
		switch (det[det.length() - 1])
		{
		case 'v':
			switch (det[1])
			{
			case 'r':
				switch (det[2])
				{
				case 'a': //srav
				{
					string rd, rs, rt;

					addTypeExtract(rd, rt, rs, currlin);
					AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 7);

					printRIns(insptr);
					
					break;
				}
				case 'l': //srlv
				{
					string rd, rs, rt;

					addTypeExtract(rd, rt, rs, currlin);
					AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 6);

					printRIns(insptr);
					
					break;
				}
				}
				break;
			case 'l': //sllv
			{
				string rd, rs, rt;

				addTypeExtract(rd, rt, rs, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 4);

				printRIns(insptr);
				
				break;
			}
			}
			break;
		case 'l':
			switch (det[1])
			{
			case 'l': //sll
			{
				string rd, shamt, rt;

				AddiuTypeExtract(rd, rt, shamt, currlin);
				AddR((RIns*)insptr, 0, 0, RegMap[rt], RegMap[rd], stoi(shamt), 0);

				printRIns(insptr);
				break;
			}
			case 'r': //srl
			{
				string rd, shamt, rt;

				AddiuTypeExtract(rd, rt, shamt, currlin);
				AddR((RIns*)insptr, 0, 0, RegMap[rt], RegMap[rd], stoi(shamt), 2);

				printRIns(insptr);
				
				break;
			}
			case 'w': //swl
			{
				string addr, rs, rt;

				lwTypeExtract(rt, rs, addr, currlin);
				AddI((IIns*)insptr, 42, RegMap[rs], RegMap[rt], stoi(addr));

				printIIns(insptr);
				 

				break;
			}
			case 'y': //syscall
				AddSys((SysIns*)insptr);

				//std::printf("00000000000000000000000000001100");
				 
				outFile << "00000000000000000000000000001100" << endl;
				break;
			}
			break;
		case 'a': //sra
		{
			string rd, shamt, rt;

			AddiuTypeExtract(rd, rt, shamt, currlin);
			AddR((RIns*)insptr, 0, 0, RegMap[rt], RegMap[rd], stoi(shamt), 3);

			printRIns(insptr);
			 
			break;
		}
		case 'b':
			switch (det[1])
			{
			case 'b': //sb
			{
				string addr, rs, rt;

				lwTypeExtract(rt, rs, addr, currlin);
				AddI((IIns*)insptr, 40, RegMap[rs], RegMap[rt], stoi(addr));

				printIIns(insptr);
				 

				break;
			}
			case 'u': //sub
			{
				string rd, rs, rt;

				addTypeExtract(rd, rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 34);

				printRIns(insptr);
				 
				break;
			}
			}
			break;
		case 'u':
			switch (det[det.length() - 2])
			{
			case 'i': //sltiu
			{
				string imm, rs, rt;

				AddiuTypeExtract(rt, rs, imm, currlin);
				AddI((IIns*)insptr, 11, RegMap[rs], RegMap[rt], stoi(imm));

				printIIns(insptr);
				 

				break;
			}
			case 't': //sltu
			{
				string rd, rs, rt;

				addTypeExtract(rd, rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 43);

				printRIns(insptr);
				 
				break;
			}
			case 'b': //subu
			{
				string rd, rs, rt;

				addTypeExtract(rd, rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 35);

				printRIns(insptr);
				 
				break;
			}
			}
			break;
		case 'c': //sc
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 56, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		case 'h': //sh
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 41, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		case 't': //slt
		{
			string rd, rs, rt;

			addTypeExtract(rd, rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 42);

			printRIns(insptr);
			
			break;
		}
		case 'i': //slti
		{
			string rs, imm, rt;

			AddiuTypeExtract(rt, rs, imm, currlin);
			AddI((IIns*)insptr, 10, RegMap[rs], RegMap[rt], stoi(imm));

			printIIns(insptr);
			
			break;
		}
		case 'r': //swr
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 46, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		case 'w': //sw
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 43, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		}
		break;
	case 'x':
		switch (det[det.length() - 1])
		{
		case 'r': //xor
		{
			string rd, rs, rt;

			addTypeExtract(rd, rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], RegMap[rd], 0, 38);

			printRIns(insptr);
			
			break;
		}
		case 'i': //xori
		{
			string imm, rs, rt;

			AddiuTypeExtract(rt, rs, imm, currlin);
			AddI((IIns*)insptr, 14, RegMap[rs], RegMap[rt], stoi(imm));

			printIIns(insptr);
			

			break;
		}
		}
		break;
	case 'l': 
		switch (det[det.length() - 1])
		{
		case 'b': //lb
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 32, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		case 'u':
			switch (det[1])
			{
			case 'b': //lbu
			{
				string addr, rs, rt;

				lwTypeExtract(rt, rs, addr, currlin);
				AddI((IIns*)insptr, 36, RegMap[rs], RegMap[rt], stoi(addr));

				printIIns(insptr);
				

				break;
			}
			case 'h': //lhu
			{
				string addr, rs, rt;

				lwTypeExtract(rt, rs, addr, currlin);
				AddI((IIns*)insptr, 37, RegMap[rs], RegMap[rt], stoi(addr));

				printIIns(insptr);
				

				break;
			}
			}
			break;
		case 'h': //lh
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 33, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		case 'l':
			switch (det[1])
			{
			case 'w': //lwl
			{
				string addr, rs, rt;

				lwTypeExtract(rt, rs, addr, currlin);
				AddI((IIns*)insptr, 34, RegMap[rs], RegMap[rt], stoi(addr));

				printIIns(insptr);
				

				break;
			}
			case 'l': //ll
			{
				string addr, rs, rt;

				lwTypeExtract(rt, rs, addr, currlin);
				AddI((IIns*)insptr, 48, RegMap[rs], RegMap[rt], stoi(addr));

				printIIns(insptr);
				

				break;
			}
			}
			break;
		case 'i': //lui
		{
			string rt, imm;

			teqiTypeExtract(rt, imm, currlin);
			AddI((IIns*)insptr, 15, 0, RegMap[rt], stoi(imm));

			printIIns(insptr);
			
			break;
		}
		case 'w': //lw
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 35, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}

		case 'r': //lwr
		{
			string addr, rs, rt;

			lwTypeExtract(rt, rs, addr, currlin);
			AddI((IIns*)insptr, 38, RegMap[rs], RegMap[rt], stoi(addr));

			printIIns(insptr);
			

			break;
		}
		}
		break;
	case 'b': 
		switch (det[1])
		{
		case 'e': //beq
		{
			string rs, lab, rt;

			AddiuTypeExtract(rs, rt, lab, currlin);
			AddI((IIns*)insptr, 4, RegMap[rs], RegMap[rt], LabMap[lab] - insct);

			printIIns(insptr);
			
			break;
		}
		case 'g':
			switch (det[det.length() - 2])
			{
			case 'e': //bgez
			{
				string rs, lab;

				teqiTypeExtract(rs, lab, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 1, LabMap[lab] - insct);

				printIIns(insptr);
				
				break;
			}
			case 'a': //bgezal
			{
				string rs, lab;

				teqiTypeExtract(rs, lab, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 17, LabMap[lab] - insct);

				printIIns(insptr);
				
				break;
			}
			case 't': //bgtz
			{
				string rs, lab;

				teqiTypeExtract(rs, lab, currlin);
				AddI((IIns*)insptr, 7, RegMap[rs], 0, LabMap[lab] - insct);

				printIIns(insptr);
				
				break;
			}
			}
			break;
		case 'l':
			switch (det[det.length() - 2])
			{
			case 'e': //blez
			{
				string rs, lab;

				teqiTypeExtract(rs, lab, currlin);
				AddI((IIns*)insptr, 6, RegMap[rs], 0, LabMap[lab] - insct);

				printIIns(insptr);
				
				break;
			}
			case 'a': //bltzal
			{
				string rs, lab;

				teqiTypeExtract(rs, lab, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 16, LabMap[lab] - insct);

				printIIns(insptr);
				
				break;
			}
			case 't': //bltz
			{
				string rs, lab;

				teqiTypeExtract(rs, lab, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 0, LabMap[lab] - insct);

				printIIns(insptr);
				
				break;
			}
			}
			break;
		case 'n': //bne
		{
			string rs, lab, rt;

			AddiuTypeExtract(rs, rt, lab, currlin); 
			AddI((IIns*)insptr, 5, RegMap[rs], RegMap[rt], LabMap[lab] - insct);

			printIIns(insptr);
			
			break;
		}
		}
		break;
	case 'j':
		switch (det[det.length() - 1])
		{
		case 'j': //j
		{ // addr map: {label_name: diff_to_virtual_base_addr}
			string label;
			label = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);
			AddJ((JIns*)insptr, 2, LabMap[label] + initaddr);

			printJIns(insptr);
			
			break;
		}
		case 'l': //jal
		{
			string label;
			label = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);
			AddJ((JIns*)insptr, 3, LabMap[label] + initaddr); //Save the address of the next instruction in register $ra

			printJIns(insptr);
			

			break;
		}
		case 'r':
			switch (det[1])
			{
			case 'r': //jr
			{
				string reg = currlin.substr(currlin.find_last_of(' ') + 1, string::npos);
				AddR((RIns*)insptr, 0, RegMap[reg], 0, 0, 0, 8);

				printRIns(insptr);
				break;
			}
			case 'a': //jalr
			{
				string rs, rd;

				teqiTypeExtract(rs, rd, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], 0, RegMap[rd], 0, 9);

				printRIns(insptr);
				break;
			}
			}
			break;
		}
		break;
	case 't':
		switch (det[det.length() - 1])
		{
		case 'q': //teq
		{
			string rs, rt;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 52);

			printRIns(insptr);
			break;
		}
		case 'i':
			switch (det[1])
			{
			case 'e': //teqi
			{
				string rs, imm;

				teqiTypeExtract(rs, imm, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 12, stoi(imm));

				printIIns(insptr);
				break;
			}
			case 'g': //tgei
			{
				string rs, imm;

				teqiTypeExtract(rs, imm, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 12, stoi(imm));

				printIIns(insptr);
				break;
			}
			case 'l': //tlti
			{
				string rs, imm;

				teqiTypeExtract(rs, imm, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 10, stoi(imm));

				printIIns(insptr);
				break;
			}
			case 'n': //tnei
			{
				string rs, imm;

				teqiTypeExtract(rs, imm, currlin);
				AddI((IIns*)insptr, 1, RegMap[rs], 14, stoi(imm));

				printIIns(insptr);
				break;
			}
			}
			break;
		case 'e':
			switch (det[1]) 
			{
			case 'n': //tne
			{
				string rs, rt;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 54);

				printRIns(insptr);
				break;
			}
			case 'g': //tge
			{
				string rs, rt;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 48);

				printRIns(insptr);
				break;
			}
			}
			break;
		case 'u':
			switch (det[det.length() - 2])
			{
			case 'i':
				switch (det[1])
				{
				case 'g': //tgeiu
				{
					string rs, imm;

					teqiTypeExtract(rs, imm, currlin);
					AddI((IIns*)insptr, 1, RegMap[rs], 9, stoi(imm));

					printIIns(insptr);
					
					break;
				}
				case 'l': //tltiu
				{
					string rs, imm;

					teqiTypeExtract(rs, imm, currlin);
					AddI((IIns*)insptr, 1, RegMap[rs], 11, stoi(imm));

					printIIns(insptr);
					
					break;
				}
				}
				break;
			case 'e': //tgeu
			{
				string rs, rt;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 49);

				printRIns(insptr);
				
				break;
			}
			case 't': //tltu
			{
				string rs, rt;

				teqiTypeExtract(rs, rt, currlin);
				AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 51);

				printRIns(insptr);
				
				break;
			}
			}
			break;
		case 't': //tlt
		{
			string rs, rt;

			teqiTypeExtract(rs, rt, currlin);
			AddR((RIns*)insptr, 0, RegMap[rs], RegMap[rt], 0, 0, 50);

			printRIns(insptr);
			
			break;
		}
		}
		break;
	}

	if (typeid(insptr) == typeid(RIns*)) type = Rtype;
	else if (typeid(insptr) == typeid(IIns*)) type = Itype;
	else if (typeid(insptr) == typeid(JIns*)) type = Jtype;
	else if (typeid(insptr) == typeid(SysIns*)) type = Systype;
}
//assemble function: input: mips row pointer，pointer to an empty instruction，instruction type，output：changed insType and insptr

int Assemble(string inpath, string outpath)
{
	inFile.open(inpath); //file reading
	outFile.open(outpath);

	// In total, I read the file two times
	// Round1: store labels
	string currlin;
	bool texfin = false;
	regMapInit(RegMap); // Initialize register and regID

	while (getline(inFile, currlin)) {
		if (texfin) {
			firstformatinp(currlin);
			if (currlin.find(".data") != std::string::npos) texfin = false;
		}
		if (currlin.find(".text") != std::string::npos) {
			texfin = true;
		}
	}

	// Round2: assembling
	inFile.clear();
	inFile.seekg(0, inFile.beg);
	insct = 0; // reinistialize instruction count
	texfin = false;
	insType tpr; // record instruction type

	while (getline(inFile, currlin))
	{
		EmpIns ins;
		if (texfin) {
			if (currlin.find(".data") != std::string::npos) { //finishing .text and coming across .data
				texfin = false;
				break;
			}

			secondformatinp(currlin); // after formatting: "add   $s0 $s1 $s2"
			if (currlin == "") continue; // one-label-only row case (label:), ignore it

			assemble(&ins, tpr, currlin);
		}

		if (currlin.find(".text") != std::string::npos) {
			texfin = true;
		}
	}
	return 0;
}




/*---------------------------------- ASSEMBLER-SIMULATOR SEPARATION LINE ----------------------------------------*/




// register number: register pointer
map<int, int*> regMap;

// sign extension converter
typedef union sign_extension {
	uint8_t eight_bit[4];
	uint16_t sixteen_bit[2];
	uint32_t thirtytwo_bit[1];
} sign_extension;

typedef union sixtyfour_bit_extension {
	uint32_t thirtytwo_bit[2];
	uint64_t sixtyfour_bit[1];
} sixtyfour_bit_extension;

//simulated memory start address
const int memstt = 0x400000;

//6MB memory initialization
int* bs = (int*)malloc(0x600000);

//where static data starts/ends
int* static_end = bs + 0x40000;

//where text segment starts/ends
int* txtseg_end = bs;
int* PC = txtseg_end;

//file stream handling
fstream file;

//Register initialization
int* $zero = (int*)malloc(4);
int* $at = (int*)malloc(4);
int* $v0 = (int*)malloc(4);
int* $v1 = (int*)malloc(4);
int* $a0 = (int*)malloc(4);
int* $a1 = (int*)malloc(4);
int* $a2 = (int*)malloc(4);
int* $a3 = (int*)malloc(4);
int* $t0 = (int*)malloc(4);
int* $t1 = (int*)malloc(4);
int* $t2 = (int*)malloc(4);
int* $t3 = (int*)malloc(4);
int* $t4 = (int*)malloc(4);
int* $t5 = (int*)malloc(4);
int* $t6 = (int*)malloc(4);
int* $t7 = (int*)malloc(4);
int* $s0 = (int*)malloc(4);
int* $s1 = (int*)malloc(4);
int* $s2 = (int*)malloc(4);
int* $s3 = (int*)malloc(4);
int* $s4 = (int*)malloc(4);
int* $s5 = (int*)malloc(4);
int* $s6 = (int*)malloc(4);
int* $s7 = (int*)malloc(4);
int* $t8 = (int*)malloc(4);
int* $t9 = (int*)malloc(4);
int* $k0 = (int*)malloc(4);
int* $k1 = (int*)malloc(4);
int* $gp = (int*)malloc(4);
int* $sp = (int*)malloc(4);
int* $fp = (int*)malloc(4);
int* $ra = (int*)malloc(4);
int* $hi = (int*)malloc(4);
int* $lo = (int*)malloc(4);

void regMapInit(map <int, int*>& rgmap) {
	rgmap[0] = $zero;
	rgmap[1] = $at;
	rgmap[2] = $v0;
	rgmap[3] = $v1;
	rgmap[4] = $a0;
	rgmap[5] = $a1;
	rgmap[6] = $a2;
	rgmap[7] = $a3;
	rgmap[8] = $t0;
	rgmap[9] = $t1;
	rgmap[10] = $t2;
	rgmap[11] = $t3;
	rgmap[12] = $t4;
	rgmap[13] = $t5;
	rgmap[14] = $t6;
	rgmap[15] = $t7;
	rgmap[16] = $s0;
	rgmap[17] = $s1;
	rgmap[18] = $s2;
	rgmap[19] = $s3;
	rgmap[20] = $s4;
	rgmap[21] = $s5;
	rgmap[22] = $s6;
	rgmap[23] = $s7;
	rgmap[24] = $t8;
	rgmap[25] = $t9;
	rgmap[26] = $k0;
	rgmap[27] = $k1;
	rgmap[28] = $gp;
	rgmap[29] = $sp;
	rgmap[30] = $fp;
	rgmap[31] = $ra;
}

// input virtual difference address and output real address
int* vir2real(int vir) {
	return (vir / 4 + bs);
}

// input real address and output virtual difference address
int real2vir(int* real) {
	return (real - bs) * 4;
}

void asciiPut(string& currlin, bool end) {
	char* mem = (char*)static_end, * stt = mem;
	string::iterator ite = currlin.begin();
	while (*ite++ != '\"') {}// move to start of str

	while (*ite != '\"') { // \n \t \r \" etc
		if (*ite == '\\') { // dealing with escape characters 
			++ite;
			switch (*ite++)
			{
			case 'n':
			{
				*mem++ = '\n';
				break;
			}
			case 't':
			{
				*mem++ = '\t';
				break;
			}
			case 'r':
			{
				*mem++ = '\r';
				break;
			}
			case '\"':
			{
				*mem++ = '\"';
				break;
			}
			case '\\':
			{
				*mem++ = '\\';
				break;
			}
			}
		}
		else *mem++ = *ite++;
	}// move data to memory
	if (end) {
		*mem++ = '\0'; // case for .asciiz
	}

	mem += (4 - (mem - stt) % 4) % 4; // start new block though not full

	static_end = (int*)mem;
}

void formatIn(string& currin) {
	rmvCom(currin); // remove the comments
	killLab(currin); // kill the label 
	killEsCharSelect(currin, '\t'); // delete extra escape characters
	trim(currin); // remove empty space on two sides
}

string dec2bin(int dec) {
	string res;
	unsigned int ins = (unsigned int)(dec);
	for (int i = 31; i >= 0; i--) {
		if (ins - pow(2, i) >= 0) {
			res += '1';
			ins -= pow(2, i);
		}
		else res += '0';
	}
	return res;
}

int bin2dec(string bin, bool signd) {
	int res = 0;
	for (int i = bin.length() - 1; i >= 0; i--) {
		if (bin[i] == '1') {
			if (i == 0 && signd) res -= pow(2, bin.length() - 1);
			// if signed number convertion, using 2's complement
			else res += pow(2, bin.length() - 1 - i);
		}
	}
	return res;
}

void R_Extract(string& ins, int& rs, int& rt, int& rd, int& shamt, int& funct) {
	rs = bin2dec(ins.substr(6, 5), false);
	rt = bin2dec(ins.substr(11, 5), false);
	rd = bin2dec(ins.substr(16, 5), false);
	shamt = bin2dec(ins.substr(21, 5), false);
	funct = bin2dec(ins.substr(26, 6), false);
}

void I_Extract(string& ins, int& rs, int& rt, int& imm) {
	rs = bin2dec(ins.substr(6, 5), false);
	rt = bin2dec(ins.substr(11, 5), false);
	imm = bin2dec(ins.substr(16, 16), true);
}

void J_Extract(string& ins, int& addr) {
	addr = bin2dec(ins.substr(6, 26), false);
}

int countLeading(string bin, char ch) {
	return count(bin.begin(), bin.end(), ch);
}

// put .word data in memory
int* place_data_int(string& currlin, int* mem) {
	string data, arg = currlin.substr(currlin.find_first_of(" "), string::npos);
	trim(arg);
	int* stt = mem;

	while (true) {
		data = arg.substr(0, arg.find_first_of(","));
		trim(data);
		*mem++ = stoi(data, nullptr, 0); 
		if (arg.find_first_of(",") == string::npos) break; //break when no more args
		arg = arg.substr(arg.find_first_of(",") + 1, string::npos);
	}
	return ((int*)mem);
}

// put .char data in memory
int* place_data_char(string& currlin, char* mem) {
	string data, arg = currlin.substr(currlin.find_first_of(" "), string::npos);
	trim(arg);
	char* stt = mem;

	while (true) {
		data = arg.substr(0, arg.find_first_of(","));
		trim(data);
		*mem++ = stoi(data, nullptr, 0); 
		if (arg.find_first_of(",") == string::npos) break; //break when no more args
		arg = arg.substr(arg.find_first_of(",") + 1, string::npos);
	}

	mem += (4 - (mem - stt) % 4) % 4;  // start new block though not full
	return ((int*)mem);
}

// put .short data in memory
int* place_data_short(string& currlin, short* mem) {
	string data, arg = currlin.substr(currlin.find_first_of(" "), string::npos);
	trim(arg);
	short* stt = mem;

	while (true) {
		data = arg.substr(0, arg.find_first_of(","));
		trim(data);
		*mem++ = (short)stoi(data, nullptr, 0); 
		if (arg.find_first_of(",") == string::npos) break; //break when no more args
		arg = arg.substr(arg.find_first_of(",") + 1, string::npos);
	}
	mem += ((mem - stt) % 2);  // start new block though not full
	
	return ((int*)mem);
}

// calculate offset based on virtual base addr and immediate
int* offset(int virbase, int imm) {
	return (int*)((char*)vir2real(virbase - memstt) + imm);
}

int main(int argc, char** argv)
{
	// assemble the file to a binary code file first
	if (argc < 4) {
		printf("wrong number of arguments");
		abort();
	}
	
	FILE* syscall_inputs = fopen(argv[2], "r");
	FILE* output_file = fopen(argv[3], "w");

	Assemble(argv[1], "assem.out");

	regMapInit(regMap);
	*regMap[0] = 0;
	*regMap[1] = 0;
	*regMap[2] = 0;
	*regMap[3] = 0;
	*regMap[4] = 0;
	*regMap[5] = 0;
	*regMap[6] = 0;
	*regMap[7] = 0;
	*regMap[8] = 0;
	*regMap[9] = 0;
	*regMap[10] = 0;
	*regMap[11] = 0;
	*regMap[12] = 0;
	*regMap[13] = 0;
	*regMap[14] = 0;
	*regMap[15] = 0;
	*regMap[16] = 0;
	*regMap[17] = 0;
	*regMap[18] = 0;
	*regMap[19] = 0;
	*regMap[20] = 0;
	*regMap[21] = 0;
	*regMap[22] = 0;
	*regMap[23] = 0;
	*regMap[24] = 0;
	*regMap[25] = 0;
	*regMap[26] = 0;
	*regMap[27] = 0;
	*regMap[28] = real2vir(static_end);
	*regMap[29] = 0x600000;
	*regMap[30] = *regMap[29];
	*regMap[31] = 0;

	std::ifstream inFile;

	//file reading for .data loading
	inFile.open(argv[1]); 

	bool datafin = false;
	string currlin; // to get current line input

	// load static datas to memory
	while (getline(inFile, currlin)) {
		// if ".data" found, load them in static data memory zone
		if (datafin) {
			formatIn(currlin); // after formatting: ".asciiz    "shit happens""
			if (currlin == "") continue;

			string det = currlin.substr(1, currlin.find(' ') - 1); //find data type

			switch (det[det.length() - 1])
			{
			case 'i': //.ascii
			{
				asciiPut(currlin, false);
				break;
			}
			case 'z': //.asciiz
			{
				asciiPut(currlin, true);
				break;
			}
			case 'd': //.word
			{
				static_end = place_data_int(currlin, static_end);
				break;
			}
			case 'e': //.byte
			{
				static_end = place_data_char(currlin, (char*)static_end);
				break;
			}
			case 'f': //.half
			{
				static_end = place_data_short(currlin, (short*)static_end);
				break;
			}
			}
			if (currlin.find(".text") != std::string::npos) datafin = false;
		}
		if (currlin.find(".data") != std::string::npos) datafin = true;
	}


	// read the machine code file
	inFile.close();
	inFile.open("assem.out");

	// put machine code in text segment
	while (getline(inFile, currlin)) {
		*txtseg_end++ = bin2dec(currlin, true);
	}

	//main loop
	int* dynamic_stt = static_end;
	bool terminate = false;
	uint32_t ins;
	string binIns, det;
	int insNum = 0;
	while (true) {
		insNum++;
		ins = *PC++; // fetch insstruction
		binIns = dec2bin(ins);

		det = binIns.substr(0, 3);
		if (det == "111") { //sc I
			int rs, rt, imm;
			I_Extract(binIns, rs, rt, imm);

			*offset(*regMap[rs], imm) = *(regMap[rt]);
			*(regMap[rt]) = 1;
		}
		else if (det == "110") { //ll I 
			int rs, rt, imm;
			I_Extract(binIns, rs, rt, imm);
			*(regMap[rt]) = *offset(*regMap[rs], imm);
		}
		else if (det == "101") {
			int rs, rt, imm;
			I_Extract(binIns, rs, rt, imm);
			det = binIns.substr(3, 3);
			if (det == "000") { //sb I
				
				*(char*)(offset(*regMap[rs], imm)) = *((char*)regMap[rt]);
			}

			else if (det == "110" || det == "010") { //swr I //swl I
				char* effc_addr = (char*)(offset(*regMap[rs], imm));
				char* reg_ptr = (char*)regMap[rt];
				// difference to end of last alignment
				int align_diff = (effc_addr - (char*)bs) % 4;
				//move align_diff bytes in reverse order in rs to memory at effc_addr in reverse order
				if (det == "110") {
					for (int i = 3; i > (3 - align_diff); i--) {
						effc_addr[i] = reg_ptr[i];
					}
				}
				//else, move (4-align_diff) bytes in rs to memory at effc_addr
				else {
					for (int i = 0; i < (4 - align_diff); i++) {
						effc_addr[i] = reg_ptr[i];
					}
				}
			}

			else if (det == "011") { //sw I
				*offset(*regMap[rs], imm) = *(regMap[rt]);
			}


			else if (det == "001") { //sh I
				*(uint16_t*)(offset(*regMap[rs], imm)) = *((uint16_t*)regMap[rt]);
			}
		}
		else if (det == "100") { 
			int rs, rt, imm;
			I_Extract(binIns, rs, rt, imm);
			det = binIns.substr(3, 3);
			if (det == "000" || det == "100") { //lb I //lbu I
				sign_extension buffer;
				buffer.eight_bit[0] = *(char*)(offset(*regMap[rs], imm));
				buffer.eight_bit[1] = 0;
				buffer.eight_bit[2] = 0;
				buffer.eight_bit[3] = 0; // sign_extension the byte to 32 bits
				if (det == "000") *regMap[rt] = buffer.thirtytwo_bit[0];
				else *regMap[rt] = (int)buffer.eight_bit[0];
			}
			else if (det == "110" || det == "010") { //lwr I //lwl I
				uint8_t* effc_addr = (uint8_t*)offset(*regMap[rs], imm);
				uint8_t* reg_ptr = (uint8_t*)regMap[rt];
				int align_diff = (effc_addr - (uint8_t*)bs) % 4; // difference to end of last alignment

				//move align_diff bytes in reverse order in rs to memory at effc_addr in reverse order
				if (det == "110") {
					for (int i = 3; i > (3 - align_diff); i--) {
						reg_ptr[i] = effc_addr[i];
					}
				}
				//move (4-align_diff) bytes in rs to memory at effc_addr
				else {
					for (int i = 0; i < (4 - align_diff); i++) {
						reg_ptr[i] = effc_addr[i];
					}
				}
			}
			else if (det == "011") { //lw I
				*regMap[rt] = *(offset(*regMap[rs], imm));
			}
			else if (det == "001" || det == "101") { //lh I //lhu I
				sign_extension buffer;
				buffer.sixteen_bit[0] = *((uint16_t*)offset(*regMap[rs], imm));
				buffer.sixteen_bit[1] = 0; // sign_extension the halfword to 32 bits
				if (det == "001") *regMap[rt] = buffer.thirtytwo_bit[0];
				else *regMap[rt] = (int)buffer.sixteen_bit[0]; 
			}
		}
		else if (det == "011") {
			int rs, rt, rd, shamt, funct;
			R_Extract(binIns, rs, rt, rd, shamt, funct);
			det = binIns.substr(26, 6);
			if (det == "100001") { //clo R
				*(regMap[rd]) = countLeading(dec2bin(*regMap[rs]), '1'); // count in MSB -> LSB order
			}
			else if (det == "100000") { //clz R
				*(regMap[rd]) = countLeading(dec2bin(*regMap[rs]), '0'); // count in MSB -> LSB order
			}
			else if (det == "000010") { //mul R
				sixtyfour_bit_extension multOne, multTwo;
				multOne.thirtytwo_bit[0] = *(regMap[rs]);
				multTwo.thirtytwo_bit[0] = *(regMap[rt]);
				multOne.sixtyfour_bit[0] = multOne.sixtyfour_bit[0] * multTwo.sixtyfour_bit[0];
				*$lo = multOne.thirtytwo_bit[0];
				*$hi = multOne.thirtytwo_bit[1];
			}
			else { //madd R //maddu R //msub R //msubu R
				sixtyfour_bit_extension multOne, multTwo, hilo;
				multOne.thirtytwo_bit[0] = *(regMap[rs]);
				multTwo.thirtytwo_bit[0] = *(regMap[rt]);
				hilo.thirtytwo_bit[0] = *$lo;
				hilo.thirtytwo_bit[1] = *$hi;
				// multiply using 64-bit unsigned ints to avoid overflow
				if (det == "000000" || det == "000001") {
					hilo.sixtyfour_bit[0] = hilo.sixtyfour_bit[0] + (multOne.sixtyfour_bit[0] * multTwo.sixtyfour_bit[0]);
				}
				else {
					hilo.sixtyfour_bit[0] = hilo.sixtyfour_bit[0] - (multOne.sixtyfour_bit[0] * multTwo.sixtyfour_bit[0]);
				}
				*$lo = hilo.thirtytwo_bit[0];
				*$hi = hilo.thirtytwo_bit[1];
			}
		}
		else if (det == "001") { //all I
			int rs, rt, imm;
			I_Extract(binIns, rs, rt, imm);
			det = binIns.substr(3, 3);
			if (det == "111") { //lui
				*(regMap[rt]) = (imm << 16);
			}
			else if (det == "110") { //xori
				*(regMap[rt]) = (*(regMap[rs]) ^ imm);
			}
			else if (det == "101") { //ori
				*(regMap[rt]) = (*(regMap[rs]) | imm);
			}
			else if (det == "100") { //andi
				*(regMap[rt]) = (*(regMap[rs]) & imm);
			}
			else if (det == "011") { //sltiu
				*(regMap[rt]) = (uint32_t(*(regMap[rs])) < uint32_t(imm));
			}
			else if (det == "010") { //slti
				*(regMap[rt]) = (*(regMap[rs]) < imm);
			}
			else if (det == "001") { //addiu 
				*(regMap[rt]) = uint32_t(*(regMap[rs])) + uint32_t(imm);
			}
			else if (det == "000") { //addi 
				if (__builtin_sadd_overflow(*(regMap[rs]), imm, (regMap[rt]))) {
					cout << "Trapped error due to immediate addition overflow!!!";
					exit(0);
				};
			}
		}
		else if (det == "000") {
			det = binIns.substr(3, 3);
			if (det == "111") { //bgtz I
				int rs, rt, imm;
				I_Extract(binIns, rs, rt, imm);
				if (*(regMap[rs]) > 0) PC += imm;
			}
			else if (det == "110") { //blez I
				int rs, rt, imm;
				I_Extract(binIns, rs, rt, imm);
				if (*(regMap[rs]) <= 0) PC += imm;
			}
			else if (det == "101") { //bne I
				int rs, rt, imm;
				I_Extract(binIns, rs, rt, imm);
				if (*(regMap[rs]) != *(regMap[rt])) PC += imm;
			}
			else if (det == "100") { //beq I
				int rs, rt, imm;
				I_Extract(binIns, rs, rt, imm);
				if (*(regMap[rs]) == *(regMap[rt])) PC += imm;
			}
			else if (det == "011" || det == "010") { //jal J //j J
				int rs, addr;
				J_Extract(binIns, addr);
				if (det == "011") *$ra = real2vir(PC) + memstt;
				// extract 4 MSBs of PC and add it with addr*4 
				PC = vir2real(((real2vir(PC) + memstt) & 0b0000000000000000000000000000) + (addr << 2) - memstt);
			}
			else if (det == "001") { //all I
				int rs, rt, imm;
				I_Extract(binIns, rs, rt, imm);
				det = binIns.substr(11, 5);
				if (det == "10001" || det == "00001") { //bgezal //bgez 
					if (det == "10001") *$ra = real2vir(PC);
					if (*(regMap[rs]) >= 0) PC += imm;
				}
				else if (det == "00000" || det == "10000") { //bltzal //bltz
					if (det == "10000") *$ra = real2vir(PC);
					if (*(regMap[rs]) < 0) PC += imm;
				}
				else {
					//teqi
					if (det == "01100") terminate = (*(regMap[rs]) == imm);

					//tgei
					else if (det == "01000") terminate = (*(regMap[rs]) >= imm);

					//tgeiu
					else if (det == "01001") terminate = (uint32_t(*(regMap[rs])) >= uint32_t(imm));

					//tlti
					else if (det == "01010") terminate = (*(regMap[rs]) < imm);

					//tltiu
					else if (det == "01011") terminate = (uint32_t(*(regMap[rs])) < uint32_t(imm));

					//tnei					
					else if (det == "01110") terminate = (*(regMap[rs]) != imm);

					if (terminate) printf("%s", "Trapped error!!!");
				}
			}
			else if (det == "000") {
				int rs, rt, rd, shamt, funct;
				R_Extract(binIns, rs, rt, rd, shamt, funct);
				det = binIns.substr(26, 3);
				if (det == "000") { // all R
					det = binIns.substr(29, 3);
					if (det == "111") { //srav
						*(regMap[rd]) = (*(regMap[rt]) >> *(regMap[rs]));
					}
					else if (det == "110") { //srlv
						*(regMap[rd]) = ((unsigned int)*(regMap[rt]) >> *(regMap[rs]));
					}
					else if (det == "100") { //sllv
						*(regMap[rd]) = ((unsigned int)*(regMap[rt]) << *(regMap[rs]));
					}
					else if (det == "011") { //sra
						*(regMap[rd]) = (*(regMap[rt]) >> shamt);
					}
					else if (det == "010") { //srl
						*(regMap[rd]) = ((unsigned int)*(regMap[rt]) >> shamt);
					}
					else if (det == "000") { //sll
						*(regMap[rd]) = ((unsigned int)*(regMap[rt]) << shamt);
					}
				}
				else if (det == "110") { // all R
					det = binIns.substr(29, 3);
					//teq
					if (det == "100") terminate = (*(regMap[rs]) == *(regMap[rt]));

					//tne
					else if (det == "110") terminate = (*(regMap[rs]) != *(regMap[rt]));

					//tltu
					else if (det == "011") terminate = ((uint32_t) * (regMap[rs]) < (uint32_t) * (regMap[rt]));

					//tlt
					else if (det == "010") terminate = (*(regMap[rs]) < *(regMap[rt]));

					//tgeu
					else if (det == "001") terminate = ((uint32_t) * (regMap[rs]) >= (uint32_t) * (regMap[rt]));

					//tge					
					else if (det == "000") terminate = (*(regMap[rs]) >= *(regMap[rt]));

					if (terminate) printf("%s", "Trapped error!!!");
				}
				else if (det == "101") { // all R
					det = binIns.substr(29, 3);
					if (det == "010") { //slt
						*(regMap[rd]) = (*(regMap[rs]) < *(regMap[rt]));
					}
					else if (det == "011") { //sltu
						*(regMap[rd]) = ((uint32_t) * (regMap[rs]) < (uint32_t) * (regMap[rt]));
					}
				}
				else if (det == "100") { // all R
					det = binIns.substr(29, 3);
					if (det == "111") { //nor 
						*(regMap[rd]) = ~(*(regMap[rs]) | *(regMap[rt]));
					}
					else if (det == "110") { //xor
						*(regMap[rd]) = *(regMap[rs]) ^ *(regMap[rt]);
					}
					else if (det == "101") { //or
						*(regMap[rd]) = *(regMap[rs]) | *(regMap[rt]);
					}
					else if (det == "100") { //and
						*(regMap[rd]) = *(regMap[rs]) & *(regMap[rt]);
					}
					else if (det == "010") { //sub 
						if (__builtin_ssub_overflow(*(regMap[rs]), *(regMap[rt]), (regMap[rd]))) {
							cout << "Trapped error due to subtraction overflow!!!";
							exit(0);
						};
					}
					else if (det == "011") { //subu
						*(regMap[rd]) = *(regMap[rs]) - *(regMap[rt]);
					}
					else if (det == "001") { //addu  
						*(regMap[rd]) = *(regMap[rs]) + *(regMap[rt]);
					}
					else if (det == "000") { //add
						if (__builtin_sadd_overflow(*(regMap[rs]), *(regMap[rt]), (regMap[rd]))) {
							cout << "Trapped error due to addition overflow!!!";
							exit(0);
						};
					}
				}
				else if (det == "011") { // all R
					det = binIns.substr(29, 3);
					sixtyfour_bit_extension rsbits, rtbits, res;
					rsbits.thirtytwo_bit[0] = *(regMap[rs]);
					rtbits.thirtytwo_bit[0] = *(regMap[rt]);
					if (det == "000" || det == "001") { //mult //multu
						res.sixtyfour_bit[0] = rsbits.sixtyfour_bit[0] * rtbits.sixtyfour_bit[0];
					}
					else if (det == "010" || det == "011") { //div //divu
						res.thirtytwo_bit[0] = rsbits.sixtyfour_bit[0] / rtbits.sixtyfour_bit[0];
						res.thirtytwo_bit[1] = rsbits.sixtyfour_bit[0] % rtbits.sixtyfour_bit[0];
					}
					*$lo = res.thirtytwo_bit[0];
					*$hi = res.thirtytwo_bit[1];

				}
				else if (det == "010") { // all R
					det = binIns.substr(29, 3);
					if (det == "011") { //mtlo
						*$lo = *(regMap[rs]);
					}
					else if (det == "010") { //mflo
						*(regMap[rd]) = *$lo;
					}
					else if (det == "001") { //mthi
						*$hi = *(regMap[rs]);
					}
					else if (det == "000") { //mfhi
						*(regMap[rd]) = *$hi;
					}
				}

				else if (det == "001") { // all R, except for syscall
					det = binIns.substr(29, 3);
					if (det == "000") { //jr
						PC = vir2real(*regMap[rs] - memstt);
					}
					else if (det == "001") { //jalr
						*(regMap[rd]) = real2vir(PC) + memstt;
						PC = vir2real(*regMap[rs] - memstt);
					}
					else if (det == "100") { //syscall
						switch (*$v0)
						{
						case 1: //print_int
						{
							fprintf(output_file, "%d", *$a0);
							break;
						}
						case 4: //print_string 
						{
							fprintf(output_file, "%s", (char*)vir2real((*$a0 - memstt)));
							break;
						}
						case 11: //print_char 
						{
							fprintf(output_file, "%c", *$a0);
							break;
						}
						case 5: //read_int
						{
							char* inp = (char*)malloc(15), *stt = inp;
							// max(int) = 2147483647 only has 10 digits (with possibly minus sign, 11)
							// the capacity of 15 should be enough
							fgets(inp, 15, syscall_inputs);
							while (*inp != '\n') inp++; 
							*inp = '\0'; // eliminate newline char
							*$v0 = atoi(stt); // reading integer by convertion
							free(stt);
							break;
						}
						case 8: //read_string 
						{
							char* inp = (char*)vir2real(*$a0 - memstt), *stt = inp;
							fgets(inp, *$a1, syscall_inputs);
							while (*inp != '\n') inp++; 
							*inp = '\0'; // eliminate newline char
							break;
						}
						case 12: //read_char
						{
							*$v0 = fgetc(syscall_inputs);
							break;
						}
						case 9: //sbrk
						{
							*$v0 = memstt + real2vir(dynamic_stt);
							char* dyst = (char*)dynamic_stt;
							dyst += *$a0;
							dyst += (4 - (*$a0 % 4)) % 4; //for alignment
							dynamic_stt = (int*) dyst;
							break;
						}
						case 10: //exit
						{
							exit(0);
							break;
						}
						case 13: //open $a0 = filename (string), $a1 = flags, $a2 = mode，then store file descriptor in $v0
						{ 
							*$v0 = open((char*)vir2real(*$a0 - memstt), *$a1, *$a2);
							break;
						}
						case 14: //read
						{
							*$v0 = read(*$a0, (char*)vir2real(*$a1 - memstt), *$a2);
							break;
						}

						case 15: //write
						{
							*$v0 = write(*$a0, (char*)vir2real(*$a1 - memstt), *$a2);
							break;
						}
						case 16: //close
						{
							close(*$a0);
							break;
						}
						case 17: //exit2
						{
							exit(0);
							break;
						}
						}
					}
				}
			}
		}
		if (terminate) break;
	} //main loop ends

	fclose(output_file);
	fclose(syscall_inputs);

	return 0;
}