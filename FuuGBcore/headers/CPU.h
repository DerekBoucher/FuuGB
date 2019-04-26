//
//  CPU.h
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#ifndef CPU_h
#define CPU_h

#include "Core.h"
#include "Memory.h"
#include "Logger.h"

#define CPU_CLOCK_PERIOD_NS 239
#define CLOCK_FREQUENCY 4190
#define CPU_FLAG_BIT_SET(...) FuuGB::CPU::FlagBits->set(__VA_ARGS__)
#define CPU_FLAG_BIT_TEST(...) FuuGB::CPU::FlagBits->test(__VA_ARGS__)
#define CPU_FLAG_BIT_RESET(...) FuuGB::CPU::FlagBits->reset(__VA_ARGS__)
#define ALU_BIT_SET(...) FuuGB::CPU::AluBits->set(__VA_ARGS__)
#define ALU_BIT_TEST(...) FuuGB::CPU::AluBits->test(__VA_ARGS__)
#define ALU_BIT_RESET(...) FuuGB::CPU::AluBits->reset(__VA_ARGS__)

#define Z_FLAG 7
#define N_FLAG 6
#define H_FLAG 5
#define C_FLAG 4

namespace FuuGB
{
	class FUUGB_API CPU
	{
	public:
		CPU(Memory*);
		virtual ~CPU();
		void stop();

	private:
		union Register
		{
			uWORD data;
			struct {
				uBYTE lo;
				uBYTE hi;
			};
			void operator = (uWORD data)
			{
				this->data = data;
			}
		};

		Register    AF; //Z N H C X X X X ---- Flag Register
		Register    BC;
		Register    DE;
		Register    HL;

		uWORD       SP;
		uWORD       PC;

		Memory*         memory;
		Cartridge*      gameCart;
		std::thread*    _cpuTHR;
		bool            _cpuRunning;
		std::bitset<sizeof(uBYTE)*8>* FlagBits;
		std::bitset<sizeof(uBYTE)*8>* AluBits;

		enum opCode
		{
			NOP = 0x00, //No instruction
			LD_16IMM_BC = 0x01, //Load immediate 16-bit value nn into BC
			LD_A_adrBC = 0x02, //Load Contents of A into the location pointed by BC
			INC_BC = 0x03, //Increment BC
			INC_B = 0x04, //Increment B
			DEC_B = 0x05, //Decrement B
			LD_8IMM_B = 0x06, //Load immediate 8 bit value into B
			RLC_A = 0x07, //Rotate A register left with carry
			LD_SP_adr = 0x08, //Load Stack Pointer address into a given address location
			ADD_BC_HL = 0x09, //Add the contents of BC to HL (BC remains the same)
			LD_adrBC_A = 0x0A, //Load the contents of the address pointed by BC into register A
			DEC_BC = 0x0B, //Decrement BC
			INC_C = 0x0C, //Increment C
			DEC_C = 0x0D, //Decrement C
			LD_8IMM_C = 0x0E, //Load immediate 8bit value into register C
			RRC_A = 0x0F, //Rotate register A to the right with carry
			STOP = 0x10, //Stop processor
			LD_16IMM_DE = 0x11, //Load immediate 16bit value into register pair DE
			LD_A_adrDE = 0x12, //Load contents of register A into address pointed by DE
			INC_DE = 0x13, //Increment DE
			INC_D = 0x14, //Increment D
			DEC_D = 0x15, //Decrement D
			LD_8IMM_D = 0x16, //Load immediate 8-bit value into register D
			RL_A = 0x17, //Rotate register A to the left
			RJmp_IMM = 0x18, //Jump to address (PC + e) where e can be any value between -127 to +129 (the value of e is contained in the following 8 bit instruction)
			ADD_DE_HL = 0x19, //Add contents of DE to HL
			LD_adrDE_A = 0x1A, //Load contents of memory location pointed by DE into A
			DEC_DE = 0x1B, //Decrement DE
			INC_E = 0x1C, //Increment E
			DEC_E = 0x1D, //Decrement E
			LD_8IMM_E = 0x1E, // Load 8 bit immediate value into E
			RR_A = 0x1F, // Rotate register A to the right
			RJmp_NOTZERO = 0x20, //Jump to address (PC+e) if last result was not zero. -127<e<129. e is specfied in the following instruction
			LD_16IMM_HL = 0x21, // Load immediate 16 bit value into HL
			LDI_A_adrHL = 0x22, // Load Contents of A into address pointed by HL. Increment HL afterwards
			INC_HL = 0x23, //Increment HL
			INC_H = 0x24, //Increment H
			DEC_H = 0x25, //Decrement H
			LD_8IMM_H = 0x26, //Load immediate 8bit value into H
			DAA = 0x27, //Adjust A for BCD addition
			RJmp_ZERO = 0x28, //Jump to address specified by (PC+e) if last result was zero. -127<e<129. e is specified in the following instruction
			ADD_HL_HL = 0x29, //Add HL to itself
			LDI_adrHL_A = 0x2A, //Load contents of address pointed by HL into A. Increment HL afterwards.
			DEC_HL = 0x2B, //Decrement HL
			INC_L = 0x2C, //Increment L
			DEC_L = 0x2D, //Decrement L
			LD_8IMM_L = 0x2E, //Load 8 bit immediate value into L
			NOT_A = 0x2F, //Logical NOT on A (One's complement)
			RJmp_NOCARRY = 0x30, //Jump to address specified by (PC+e) if last result did not generate a carry
			LD_16IM_SP = 0x31, //Load 16 bit immediate value into SP
			LDD_A_adrHL = 0x32, //Load contents of A into address pointed by HL, then decrement HL afterwards.
			INC_SP = 0x33, //Increment SP
			INC_valHL = 0x34, //Increment value pointed by HL
			DEC_valHL = 0x35, //Decrement value pointed by HL
			LD_8IMM_adrHL = 0x36, //Load immediate 8bit value into address pointed by HL
			SET_CARRY_FLAG = 0x37, //Set Carry Flag
			RJmp_CARRY = 0x38, //Jump to address (PC+e) if last result generated a Carry.
			ADD_SP_HL = 0x39, //Add value of SP to value of HL
			LDD_adrHL_A = 0x3A, //Load contents of address pointed by HL into A, then decrement HL afterwards.
			DEC_SP = 0x3B, //Decrement SP
			INC_A = 0x3C, //Increment A
			DEC_A = 0x3D, //Decrement A
			LD_8IMM_A = 0x3E, //Load 8 bit immediate value into A
			COMP_CARRY_FLAG = 0x3F, //Compliment carry flag
			LD_B_B = 0x40, //Load B into B (Redundant)
			LD_C_B = 0x41, //Load C into B
			LD_D_B = 0x42, //Load D into B
			LD_E_B = 0x43, //Load E into B
			LD_H_B = 0x44, //Load H into B
			LD_L_B = 0x45, //Load L into B
			LD_adrHL_B = 0x46, //Load content pointed by HL into B
			LD_A_B = 0x47, //Load A into B
			LD_B_C = 0x48, //Load B into C
			LD_C_C = 0x49, //Load C into C (Redundant)
			LD_D_C = 0x4A, //Load D into C
			LD_E_C = 0x4B, //Load E into C
			LD_H_C = 0x4C, //Load H into C
			LD_L_C = 0x4D, //Load L into C
			LD_adrHL_C = 0x4E, //Load contents of address pointed by HL into C
			LD_A_C = 0x4F, //Load A into C
			LD_B_D = 0x50, //Load B into D
			LD_C_D = 0x51, //Load C into D
			LD_D_D = 0x52, //Load D into D (Redundant)
			LD_E_D = 0x53, //Load E into D
			LD_H_D = 0x54, //Load H into D
			LD_L_D = 0x55, //Load L into D
			LD_adrHL_D = 0x56, //Load contents of address pointed by HL into D
			LD_A_D = 0x57, //Load A into D
			LD_B_E = 0x58, //Load B into E
			LD_C_E = 0x59, //Load C into E
			LD_D_E = 0x5A, //Load D into E
			LD_E_E = 0x5B, //Load E into E (Redundant)
			LD_H_E = 0x5C, //Load H into E
			LD_L_E = 0x5D, //Load L into E
			LD_adrHL_E = 0x5E, //Load contents of address pointed by HL into E
			LD_A_E = 0x5F, //Load A into E
			LD_B_H = 0x60, //Load B into H
			LD_C_H = 0x61, //Load C into H
			LD_D_H = 0x62, //Load D into H
			LD_E_H = 0x63, //Load E into H
			LD_H_H = 0x64, //Load H into H
			LD_L_H = 0x65, //Load L into H
			LD_adrHL_H = 0x66, //Load contents of address pointed by HL into H
			LD_A_H = 0x67, //Load A into H
			LD_B_L = 0x68, //Load B into L
			LD_C_L = 0x69, //Load C into L
			LD_D_L = 0x6A, //Load D into L
			LD_E_L = 0x6B, //Load E into L
			LD_H_L = 0x6C, //Load H into L
			LD_L_L = 0x6D, //Load L into L (Redundant)
			LD_adrHL_L = 0x6E, //Load contents of address pointed by HL into L
			LD_A_L = 0x6F, //Load A into L
			LD_B_adrHL = 0x70, //Load B into address pointed by HL
			LD_C_adrHL = 0x71, //Load C into address pointed by HL
			LD_D_adrHL = 0x72, //Load D into address pointed by HL
			LD_E_adrHL = 0x73, //Load E into address pointed by HL
			LD_H_adrHL = 0x74, //Load H into address pointed by HL
			LD_L_adrHL = 0x75, //Load L into address pointed by HL
			HALT = 0x76, //Halt processor
			LD_A_adrHL = 0x77, //Load A into address pointed by HL
			LD_B_A = 0x78, //Load B into A
			LD_C_A = 0x79, //Load C into A
			LD_D_A = 0x7A, //Load D into A
			LD_E_A = 0x7B, //Load E into A
			LD_H_A = 0x7C, //Load H into A
			LD_L_A = 0x7D, //Load L into A
			LD_adrHL_A = 0x7E, //Load contents of address pointed by HL into A
			LD_A_A = 0x7F, //Load A into A
			ADD_B_A = 0x80, //Add B to A
			ADD_C_A = 0x81, //Add C to A
			ADD_D_A = 0x82, //Add D to A
			ADD_E_A = 0x83, //Add E to A
			ADD_H_A = 0x84, //Add H to A
			ADD_L_A = 0x85, //Add L to A
			ADD_adrHL_A = 0x86, //Add contents of address pointed by HL to A
			ADD_A_A = 0x87, //Add A to A
			ADC_B_A = 0x88, //Add B and CARRY flag to A
			ADC_C_A = 0x89, //Add C and CARRY flag to A
			ADC_D_A = 0x8A, //add D and CARRY flag to A
			ADC_E_A = 0x8B, //Add E and CARRY flag to A
			ADC_H_A = 0x8C, //Add H and CARRY flag to A
			ADC_L_A = 0x8D, //Add L and CARRY flag to A
			ADC_adrHL_A = 0x8E, //Add contents of address pointed by HL and CARRY flag to A
			ADC_A_A = 0x8F, //Add A and CARRY flag to A
			SUB_B_A = 0x90, //Subtract B from A (Result in A)
			SUB_C_A = 0x91, //Subtract C from A
			SUB_D_A = 0x92, //Subtract D from A
			SUB_E_A = 0x93, //Subtract E from A
			SUB_H_A = 0x94, //Subtract H from A
			SUB_L_A = 0x95, //Subtract L from A
			SUB_adrHL_A = 0x96, //Subtract value pointed by HL from A
			SUB_A_A = 0x97, //Subtract A from A
			SBC_B_A = 0x98, //Subtract B and Carry flag from A
			SBC_C_A = 0x99, //Subtract C and CArry flag from A
			SBC_D_A = 0x9A, //Subtract D and Carry flag from A
			SBC_E_A = 0x9B, //Subtract E and CArry Flag from A
			SBC_H_A = 0x9C, //Subtract H and Carry Flag from A
			SBC_L_A = 0x9D, //Subtract L and Carry flag from A
			SBC_adrHL_A = 0x9E, //Subtract value pointed by HL and carry flag from A
			SBC_A_A = 0x9F, //Subtract A and carry flag from A
			AND_B_A = 0xA0, //Logical B AND A
			AND_C_A = 0xA1, //Logical C AND A
			AND_D_A = 0xA2, //Logical D AND A
			AND_E_A = 0xA3, //Logical E AND A
			AND_H_A = 0xA4, //Logical H AND A
			AND_L_A = 0xA5, //Logical L AND A
			AND_adrHL_A = 0xA6, //Logical value pointed by HL AND A
			AND_A_A = 0xA7, //Logical A AND A
			XOR_B_A = 0xA8, //Logical B XOR A
			XOR_C_A = 0xA9, //Logical C XOR A
			XOR_D_A = 0xAA, //Logical D XOR A
			XOR_E_A = 0xAB, //Logical E XOR A
			XOR_H_A = 0xAC, //Logical H XOR A
			XOR_L_A = 0xAD, //Logical L XOR A
			XOR_adrHL_A = 0xAE, //Logical value pointed by HL XOR A
			XOR_A_A = 0xAF, //Logical A XOR A
			OR_B_A = 0xB0, //Logical B OR A
			OR_C_A = 0xB1, //Logical C OR A
			OR_D_A = 0xB2, //Logical D OR A
			OR_E_A = 0xB3, //Logical E OR A
			OR_H_A = 0xB4, //Logical H OR A
			OR_L_A = 0xB5, //Logical L OR A
			OR_adrHL_A = 0xB6, //Logical value pointed by HL OR A
			OR_A_A = 0xB7, //Logical A OR A
			CMP_B_A = 0xB8, //Compare B to A (Performs A - B, sets zero flag if equals 0, or else does nothing)
			CMP_C_A = 0xB9, //Compare C to A
			CMP_D_A = 0xBA, //Compare D to A
			CMP_E_A = 0xBB, //Compare E to A
			CMP_H_A = 0xBC, //Compare H to A
			CMP_L_A = 0xBD, //Compare L to A
			CMP_adrHL_A = 0xBE, //Compare value pointed by HL to A
			CMP_A_A = 0xBF, //Compare A to A (Obsolete)
			RET_NOT_ZERO = 0xC0, //Return if last result was not zero
			POP_BC = 0xC1, //Pop stack into BC
			JMP_NOT_ZERO = 0xC2, //Jump to absolute location if last result was not zero
			JMP = 0xC3, //Jump to absolute value
			CALL_NOT_ZERO = 0xC4, //Call routine at location if last result was not zero
			PUSH_BC = 0xC5, //Push contents of BC into the stack
			ADD_IMM_A = 0xC6, //Add immediate value to A
			RST_0 = 0xC7, //Call routine at memory location 0x0000
			RET_ZERO = 0xC8, //return from routine if last result was zero
			RETURN = 0xC9, //Return from routine
			JMP_ZERO = 0xCA, //Jump to absolute location if last result was zero
			EXT_OP = 0xCB, //Extended operation (Requires more byte strings)
			CALL_ZERO = 0xCC, //Call routine at location if last result was zero
			CALL = 0xCD, //Call routine at location.
			ADC_8IMM_A = 0xCE, //Add 8 bit immediate value and CARRY flag to A
			RST_8 = 0xCF, //Call routine at memory location 0x0008
			RET_NOCARRY = 0xD0, //Return from routine if last result did not generate a carry
			POP_DE = 0xD1, //POP contents pointed by SP into DE
			JMP_NOCARRY = 0xD2, //Jump to absolute location if last result did not generate a carry
			//        XX = 0xD3, //No operation
			CALL_NOCARRY = 0xD4, //Call routine at location if last result did not generate a carry
			PUSH_DE = 0xD5, //Push contents of DE into the stack
			SUB_8IMM_A = 0xD6, //Subtract immediate 8 bit value from A
			RST_10 = 0xD7, //Call routine at memory location 0x0010
			RET_CARRY = 0xD8, //REturn from routine if last result generated a carry
			RET_INT = 0xD9, //Enable interrupts and return to calling routine
			JMP_CARRY = 0xDA, //Jump to absolute location if last result generated a carry
			//            XX = 0xDB, //No operation
			CALL_CARRY = 0xDC, //Call routine at location if last result generated a carry
			//        XX = 0xDD, //No operation
			SBC_8IMM_A = 0xDE, //Subtract 8 bit immediate value and CARRY flag from A
			RST_18 = 0xDF, //Call routine at memory location 0x0018
			LDH_A_IMMadr = 0xE0, //Load contents of A into memory location pointed to by (0xFF00 + 8 bit immediate value)
			POP_HL = 0xE1, //Pop stack into HL
			LDH_A_C = 0xE2, //Load contents of A into memory location pointed to by (0xFF00 + C)
			//        XX = 0xE3, //No operation
			//        XX = 0xE4, //No operation
			PUSH_HL = 0xE5, //Push contents of HL into the stack
			AND_8IMM_A = 0xE6, //Logical 8 Bit immediate AND A
			RST_20 = 0xE7, //Call routine at memory location 0x0020
			ADD_SIMM_SP = 0xE8, //Add signed 8 bit immediate value to SP
			JMP_adrHL = 0xE9, //Jump to address pointed by HL
			LD_A_adr = 0xEA, //Load A into specified 16 bit address
			//        XX = 0xEB, //No operation
			//        XX = 0xEC, //No operation
			//        XX = 0xED, //No operation
			XOR_8IMM_A = 0xEE, //Logical 8 bit immediate XOR A
			RST_28 = 0xEF, //Call routine at memory location 0x0028
			LDH_IMMadr_A = 0xF0, //Load Contents of memory at location (0xFF00 + 8bit immediate) into A
			POP_AF = 0xF1, //Pop stack into AF
			//    XX = 0xF2 , //No operation
			DISABLE_INT = 0xF3, //Disable interupts
			//        XX = 0xF4, //No operation
			PUSH_AF = 0xF5, //Push contents of AF into stack
			OR_8IMM_A = 0xF6, //Logical 8 bit immediate OR A
			RST_30 = 0xF7, //Call routine at memory location 0x0030
			LDHL_S_8IMM_SP_HL = 0xF8, // Add 8 bit immediate value to SP and then save result into HL
			LD_HL_SP = 0xF9, //Load HL into SP
			LD_16adr_A = 0xFA, //Load specified 16 bit address into A
			ENABLE_INT = 0xFB, //Enable interrupts
			//            XX = 0xFC, //No operation
			//        XX = 0xFD, //No operation
			CMP_8IMM_A = 0xFE, //Compare 8 bit immediate to A
			RST_38 = 0xFF //Call routine at memory location 0x0038
		};

		void clock();
		void executeNextOpCode();
		void increment16BitRegister(uWORD & reg);
		void increment8BitRegister(uBYTE & reg);
		void decrement8BitRegister(uBYTE & reg);
		void decrement16BitRegister(uWORD & reg);
		void add16BitRegister(uWORD & host, uWORD operand);
		bool TestBitInByte(uBYTE byte, int pos);
		bool TestBitInWord(uWORD word, int pos);
		bool checkCarryFromBit_Byte(int pos, uBYTE byte, uBYTE addedByte);
		bool checkBorrowFromBit_Byte(int pos, uBYTE byte, uBYTE subtractedByte);
		uBYTE twoComp_Byte(uBYTE byte);
	};
}

#endif /* CPU_h */
