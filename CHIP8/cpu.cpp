#include "cpu.h"
#include <iostream>
#include <bitset>
#include <stdlib.h> 
#include <time.h>      
#include <ctime>

void CPU::initCPU() {
	debug = false;
	screenFlag = false;
	CPUhalted = false;

	PC = 0x200; // 0x200 is the beginning of the ROM - 0 - 1FFF is for the intreperter

	for (int i = 0; i < 16; i++) {
		registers[i] = 0;
	}

	for (int i = 0; i < 0x50; i++) {
		memory[i] = chip8_fontset[i];
	}

	createScreen();

	registerI = 0;
	delayTimer = 0;
	soundTimer = 0;
}

//fetches instruction
//decodes/executes instruction
//updates timers
//prints to console status of cpu if debug is on
void CPU::cycle() {
	word inst = 0;
	screenFlag = false;

	inst = this->fetch();
	this->decodeExecute(inst);

	if (soundTimer > 0) { soundTimer--; }
	if (delayTimer > 0) { delayTimer--; }


	if (debug) {
		cout << "PC: " << "0x" << std::hex << this->getPC() << " " << endl;
		std::cout<< "instruction: "<< std::hex << "0x" << inst << std::endl;
		cout << "delay timer: " << (int)delayTimer << endl;
		cout << "sound timer: " << (int)soundTimer << endl;
		this->printRegisters();
		for (int i = 0; i < 16; i++) {
			cout << "key[" << i << "] - " << (bool)keyboardInputs[i] << endl;
		}
	}
}

void CPU:: updateKey(int key, bool val) {

	this->keyboardInputs[key] = val;

}
word CPU::getPC() {
	return this->PC;
}

word CPU::getStackTop()
{
	if (stack.size() > 0) {
		return stack.top();
	}
	else {
		return 0;
	}
}

byte CPU::getSoundTimer()
{
	return this->soundTimer;
}

byte CPU::getDelayTimer()
{
	return delayTimer;
}

byte ** CPU::getScreen()
{
	return screen;
}

bool CPU::getScreenFlag()
{
	return screenFlag;
}

byte* CPU::getRegisters()
{
	
	 return registers;
}

word CPU::getRegisterI()
{
	return registerI;
}

//creates a 63x32 array - used to draw the screen
void CPU::createScreen()
{
	screen = new byte*[64];
	for (int i = 0; i < 32; i++) {
		screen[i] = new byte[64];
		for (int j = 0; j < 64; j++) {
			screen[i][j] = 0;
		}
	}
}

// load in the game 
void CPU::loadROM(std::string romFile) {

	FILE *in;
	in = fopen(romFile.c_str(), "rb");
	fread(&memory[0x200], 0xfff, 1, in);
	fclose(in);
}

void CPU::printROM() {

	for (int i = 0x0; i <= sizeof(memory); i++) {
		std::bitset<8> x(memory[i]);
		std::cout <<"0x"<<std::hex << i << ": "<< x << std::endl;
	}
}

void CPU::printRegisters()
{
	for (int i = 0; i < 16; i++) {
		std::bitset<8> x(registers[i]);
		std::cout << "R" << i << ": "<<x << std::endl;
	}
	std::cout << "I: 0x" << std::hex << registerI << std::endl;
	cout << endl;
}

//gets the data at PC and PC+1. 
//	One full opcode is a word in length (16 bits).
//	Each memory address is a byte (8 bits).
//	Opcodes are stored in PC and PC+1
word CPU::fetch() {

	word inst = 0;

	word inst_half1 = memory[PC];
	word inst_half2 = memory[PC + 1];

	inst = inst_half1;
	inst = inst << 8; // make room for 8 bits
	inst = inst | inst_half2; // "concatanate" by birwise or
	
		PC = PC + 2;

	
	return inst;



}

void CPU::decodeExecute(word inst)
{
	word opcode = inst & 0xf000; // anding with 0xf000 will give the just first byte and the rest 0
	std::string instructionType = "";
	switch (opcode)
	{
		case 0x0000: {
			
			if ((inst & 0x00ff) == 0x00E0) 
			{

				//Clears the screen.
				instructionType = "CLEAR SCREEN";
				this->createScreen();
				//screen = 0;
				break;

			}
			if ((inst & 0x00ff) == 0x00EE) 
			{
				//Returns from a subroutine.
				instructionType = "Sub Routine Return";
				PC = stack.top();
				stack.pop();
				break;

			}
			break;
		}
		case 0x1000: {
			// Jumps to address NNN.
			instructionType = "JUMP";
			PC = inst & 0x0fff;
			break;
		}
		case 0x2000: {
			instructionType = "sub routine call";
			stack.push(PC);
			PC = inst & 0x0fff;
			break;
		}
		case 0x3000: {
			//Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
			instructionType = "skip if equal immediate";
			word x = inst & 0x0f00;
			x = x >> 8;
			if (registers[x] == (inst & 0x00ff)) {
				if (debug) { cout << "skipping next instruction" << endl; }
				PC += 2;
			}
			break;
		}
		case 0x4000: {
			//Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
			instructionType = "skip if not equal immediate";
			word x = inst & 0x0f00;
			x = x >> 8;
			if (registers[x] != (inst & 0x00ff)) {
				if (debug) { cout << "skipping next instruction" << endl; }
				PC += 2;
			}
			break;
		}
		case 0x5000: {
			//Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
			instructionType = "skip if equal";
			word x = inst & 0x0f00;
			x = x >> 8;
			word y = inst & 0x00f0;
			y = y >> 4;
			if (registers[x] == registers[y]) {
				PC += 2;
				if (debug) { cout << "skipping next instruction" << endl; }
			}
			break;
		}
		case 0x6000: {
			instructionType = "set to an immediate";
			word x = inst & 0x0f00;
			x = x >> 8;
			registers[x] = inst & 0x00ff;
			break;
		}
		case 0x7000: {
			instructionType = "add immediate";
			word x = inst & 0x0f00;
			x = x >> 8;
			registers[x] += inst & 0x00ff;
			break;
		}
		case 0x8000: 
		{
			word x = inst & 0x0f00;
			x = x >> 8;
			word y = inst & 0x00f0;
			y = y >> 4;

			switch (inst & 0x000f)
			{
				case 0x0000: {
					//Sets VX to the value of VY.
					instructionType = "set VX to VY";
					registers[x] = registers[y];
					break;
				}
				case 0x0001: {
					//	Sets VX to VX or VY. (Bitwise OR operation)
					instructionType = "set Vx = to Vx bitwise or vy";
					registers[x] = registers[x] | registers[y];
					break;
				}
				case 0x0002: {
					//Sets VX to VX and VY. (Bitwise AND operation)
					instructionType = "bitwise AND";
					registers[x] = registers[x] & registers[y];
					break;
				}
				case 0x0003: {
					//Sets VX to VX xor VY.
					instructionType = "bitwise xor";
					registers[x] = registers[x] ^ registers[y];
					break;
				}
				case 0x0004: {
					//Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
					instructionType = "Add and check for carry";
					if (registers[y] > (0xFF - registers[x])) {
						registers[0xf] = 1;
					}
					else {
						registers[0xf] = 0;

					}
					registers[x] = registers[x] + registers[y];

					break;
				}
				case 0x0005: {
					//	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
					instructionType = "subtract and check for borrow";
					if (registers[y] > registers[x]) {
						registers[0xf] = 0;
					}
					else {
						registers[0xf] = 1;
					}

					registers[x] = registers[x] - registers[y];

					break;
				}
				case 0x0006: {
					//Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
					instructionType = "shift Vx by one";
					int LSB = registers[x] & 1; // this will get the LSB
					registers[0xf] = LSB;
					registers[x] = registers[x] >> 1;
					break;
				}
				case 0x0007: {
					//Vx=Vy-Vx Sets VX to VY minus VX.VF is set to 0 when there's a borrow, and 1 when there isn't.
					instructionType = "subtract Vx from Vy";
					if (registers[x] > registers[y]) {
						registers[0xf] = 0;
					}
					else {
						registers[0xf] = 1;
					}

					registers[x] = registers[y] - registers[x];
					break;
				}
				case 0x000E: {
					//Shifts VX left by one.VF is set to the value of the most significant bit of VX before the shift.
					instructionType = "Shift left by one";
					int MSB = registers[x] >> 7;
					registers[0xf] = MSB;
					registers[x] = registers[x] << 1;
					break;
				}
				default: {
					std::cout << "Instruction: 0x" << inst << "is not supported" << std::endl;
					break;
				}
			}
			
			break;

		}
		case 0x9000: {
			instructionType = "skip if not equal";
			word x = inst & 0x0f00;
			x = x >> 8;
			word y = inst & 0x00f0;
			y = y >> 4;
			if (registers[x] != registers[y]) {
				PC += 2;
			}
			break;

		}
		case 0xA000: {
			instructionType = "set register I";
			registerI = inst & 0x0fff;
			break;

		}
		case 0xB000: {
			instructionType = "jump plus immediate";
			PC = (inst & 0x0fff) + registers[0];
			break;

		}
		case 0xC000: {
			instructionType = "random and";

			srand(time(NULL));

			int num = rand() % 256;
			word x = inst & 0x0f00;
			x = x >> 8;
			registers[x] = num & (inst & 0x00ff);
			break;

		}
		case 0xD000: {
			instructionType = "display";
			word Vx = inst & 0x0f00;
			Vx = Vx >> 8;
			word Vy = inst & 0x00f0;
			Vy = Vy >> 4;
			word height = inst & 0x000f; // no need to shift
			word pixel;

			registers[0xf] = 0;

			for (int y = 0; y < height; y++) {

				pixel = memory[registerI + y]; //gets the current 8 bit pixel to draw 

				for (int x = 0; x < 8; x++) { // iterate through the pixel 

					if ((pixel & (0x80 >> x)) != 0) { // scan throught byte one bit at a time
						int reg1 = registers[Vx] + x;
						int reg2 = registers[Vy] + y;					
						if (screen[reg2][reg1] == 1) { // check for collision (pixel is already 1)
							registers[0xf] = 1;
						}

						//set new screen to draw
						screen[reg2][reg1] ^= 1;

					}
				}

				screenFlag = true; //set flag to redraw screen
				
			}

			break;

		}
		case 0xE000: {
			

			switch (inst & 0x0ff) {
				case 0x009E: {
					//Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
					instructionType = "skip if key pressed";
					word x = inst & 0x0f00;
					x = x >> 8;

					if (keyboardInputs[registers[x]]) {
						if (debug) { cout << "skipping next instruction" << endl; }
						PC += 2;
					}

					break;
				}
				case 0x00A1: {
					//Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
					instructionType = "skip if key not pressed";

					word x = inst & 0x0f00;
					x = x >> 8;

					if (!keyboardInputs[registers[x]]) {
						if (debug) { cout << "skipping next instruction" << endl; }
						PC += 2;
					}

					break;
				}

			}

			break;

		}
		case 0xF000: {
			word x = inst & 0x0f00;
			x = x >> 8;

			switch (inst & 0x00ff) {
				case 0x0007: {
					//Sets VX to the value of the delay timer.
					instructionType = "set vx to delay timer";
				
					registers[x] = delayTimer;
					break;
				}
				case 0x000A: {
					// A key press is awaited, and then stored in VX. (Blocking Operation.All instruction halted until next key event)
					instructionType = "get key";
					CPUhalted = true;
					PC -= 2; // run instruction again unless keyboard is pressed
					for (int i = 0; i < 16; i++) {
						if (keyboardInputs[i]) {
							CPUhalted = false;
							PC += 2;
							registers[x] = i;
							break;
						}
					}

					break;
				}
				case 0x0015: {
					//Sets the delay timer to VX.
					instructionType = "set delay timer";
					
					delayTimer = registers[x];
					break;
				}
				case 0x0018: {
					//Sets the sound timer to VX.
					instructionType = "set sound timer";
					
					soundTimer = registers[x];
					break;
				}
				case 0x001E: {
					//Adds VX to I
					instructionType = "add vx to I ";

					registerI += registers[x];
					break;
				}
				case 0x0029: {
					//Sets I to the location of the sprite for the character in VX.Characters 0 - F(in hexadecimal) are represented by a 4x5 font.
					instructionType = "set I to sprite";

					registerI = registers[x] * 0x5; //fonts are 5 bytes, move the front of the font

					break;
				}
				case 0x0033: {
					//Stores the binary-coded decimal representation of VX,
					instructionType = "BCD";

					memory[registerI] = registers[x] / 100; //gets the hundreds place
					memory[registerI + 1] = (registers[x] / 10) % 10; // gets the tenths place
					memory[registerI + 2] = registers[x] % 10; // gets the ones place

					break;
				}
				case 0x0055: {
					//	Stores V0 to VX (including VX) in memory starting at address I
					instructionType = "reg dump";

					for (int i = 0; i <= x; i++) {
						memory[registerI + i] = registers[i];
					}
					registerI = registerI + x + 1;
					break;
				}
				case 0x0065: {
					//Fills V0 to VX (including VX) with values from memory starting at address I.
					instructionType = "reg load";
					for (int i = 0; i <= x; i++) {
						registers[i] = memory[registerI + i];
					}
					registerI = registerI + x + 1;
					break;
				}
				
			}

			break;

		}
		default: {
			std::cout << "Opcode: 0x" << opcode << "is not supported" << std::endl;
			break;
			}
		}

		if (debug) {
			std::cout << "This is a " << instructionType << " instruction" << std::endl;
		}
	}

