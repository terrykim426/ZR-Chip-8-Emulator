#pragma once
#include <vector>

#include "Graphics.h"

using namespace std;

class Cpu
{
private:
	byte m_GameMemory[0xFFF]; // 0xFFF bytes of memory
	byte m_Registers[16]; // 16 registers, 1 byte each
	word m_AddressI; // the 16-bit address register I
	word m_ProgramCounter; // the 16-bit program counter
	vector<word> m_Stack; // the 16-bit stack

	Graphics* m_graphics;

	static const int interval = 17; // 1000 ms / 60 hertz
	int m_timer;
	sf::Clock m_CpuClock;
	byte m_delayTimer;
	byte m_soundTimer;

	byte m_inputs[16];

public:
	Cpu(Graphics* graphics);
	~Cpu();

	void Reset();
	void Run();
	void OnKeyPressed(sf::Event::KeyEvent key);
	void OnKeyReleased(sf::Event::KeyEvent key);

private:
	void Decode(word opcode);
	word GetNextOpCode();
	void Opcode0NNN();
	void Opcode00E0();
	void Opcode00EE();
	void Opcode1NNN(word opcode);
	void Opcode2NNN(word opcode);
	void Opcode3XNN(word opcode);
	void Opcode4XNN(word opcode);
	void Opcode5XY0(word opcode);
	void Opcode6XNN(word opcode);
	void Opcode7XNN(word opcode);
	void Opcode8XY0(word opcode);
	void Opcode8XY1(word opcode);
	void Opcode8XY2(word opcode);
	void Opcode8XY3(word opcode);
	void Opcode8XY4(word opcode);
	void Opcode8XY5(word opcode);
	void Opcode8XY6(word opcode);
	void Opcode8XY7(word opcode);
	void Opcode8XYE(word opcode);
	void Opcode9XY0(word opcode);
	void OpcodeANNN(word opcode);
	void OpcodeBNNN(word opcode);
	void OpcodeCXNN(word opcode);
	void OpcodeDXYN(word opcode);
	void OpcodeEX9E(word opcode);
	void OpcodeEXA1(word opcode);
	void OpcodeFX07(word opcode);
	void OpcodeFX0A(word opcode);
	void OpcodeFX15(word opcode);
	void OpcodeFX18(word opcode);
	void OpcodeFX1E(word opcode);
	void OpcodeFX29(word opcode);
	void OpcodeFX33(word opcode);
	void OpcodeFX55(word opcode);
	void OpcodeFX65(word opcode);
};

/*
*/