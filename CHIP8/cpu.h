/*
Bryon Kucharski
September 5th, 2017

CHIP8 Intrepreter

cpu.h

-CHIP8 cpu

*/

#include <vector>
#include <stack>    
typedef unsigned short int word; // two bytes
typedef unsigned char byte; // 8 bits
using namespace std;
class CPU {
public:
	void initCPU();
	void loadROM(std::string romFile);
	void printROM();
	void printRegisters();
	void cycle();
	word fetch();
	void decodeExecute(word inst);
	word getPC();
	word getStackTop();
	byte getSoundTimer();
	byte getDelayTimer();
	byte ** getScreen();
	bool getScreenFlag();

	byte* getRegisters();
	word getRegisterI();
	void createScreen();
	void updateKey(int key, bool val);

	
private:
	byte memory[0xFFF];
	byte registers[16];
	word registerI;
	word PC; // program counter
	std::stack<word> stack;

	byte ** screen;

	byte delayTimer, soundTimer;

	bool debug, screenFlag, CPUhalted;
	bool keyboardInputs[16];

	//taken from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
	byte chip8_fontset[80]=
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};



};