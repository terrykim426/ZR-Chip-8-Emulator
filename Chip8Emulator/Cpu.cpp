#include "stdafx.h"
#include "Cpu.h"
#include <iostream>
#include <Windows.h>


Cpu::Cpu(Graphics* graphics)
{
	m_graphics = graphics;
}


Cpu::~Cpu()
{
}

void Cpu::Reset()
{
	m_AddressI = 0;
	m_ProgramCounter = 0x200;

	memset(m_Registers, 0, sizeof(m_Registers));

	// load in the game
	FILE *in;
	fopen_s(&in, "E:/VS Projects/Chip8Emulator/Roms/INVADERS", "rb");
	fread_s(&m_GameMemory[0x200], sizeof(m_GameMemory), 0xfff, 1, in);
	fclose(in);

	m_Stack.clear();

	m_CpuClock.restart();
	m_timer = 0;
	m_delayTimer = 0;
	m_soundTimer = 0;

	memset(m_inputs, 0, sizeof(m_inputs));
}

void Cpu::Run()
{
	int time = m_CpuClock.getElapsedTime().asMilliseconds();

	if (m_timer < time)
	{
		m_timer += interval;

		for (int i = 0; i < 10; ++i)
		{
			word opcode = GetNextOpCode();
			Decode(opcode);
		}

		if (m_delayTimer > 0)
		{
			m_delayTimer--;
		}

		if (m_soundTimer > 0)
		{
			m_soundTimer--;
			// Note: this only worked on window OS
			Beep(440, 100);
		}
	}
}

void Cpu::OnKeyPressed(sf::Event::KeyEvent key)
{
	//cout << "OnKeyPressed: " << key.code << endl;
	switch (key.code)
	{
	case sf::Keyboard::Up: m_inputs[0x2] = 1; break;
	case sf::Keyboard::Down: m_inputs[0x8] = 1; break;
	case sf::Keyboard::Left: m_inputs[0x4] = 1; break;
	case sf::Keyboard::Right: m_inputs[0x6] = 1; break;
	case sf::Keyboard::Space: m_inputs[0x5] = 1; break;

	case sf::Keyboard::A: m_inputs[0xA] = 1; break;
	case sf::Keyboard::B: m_inputs[0xB] = 1; break;
	case sf::Keyboard::C: m_inputs[0xC] = 1; break;
	case sf::Keyboard::D: m_inputs[0xD] = 1; break;
	case sf::Keyboard::E: m_inputs[0xE] = 1; break;
	case sf::Keyboard::F: m_inputs[0xF] = 1; break;
	case sf::Keyboard::Num0: m_inputs[0x0] = 1; break;
	case sf::Keyboard::Num1: m_inputs[0x1] = 1; break;
	case sf::Keyboard::Num3: m_inputs[0x3] = 1; break;
	case sf::Keyboard::Num7: m_inputs[0x7] = 1; break;
	case sf::Keyboard::Num9: m_inputs[0x9] = 1; break;
	}
}

void Cpu::OnKeyReleased(sf::Event::KeyEvent key)
{
	//cout << "OnKeyReleased: " << key.code << endl;
	switch (key.code)
	{
	case sf::Keyboard::Up: m_inputs[0x2] = 0; break;
	case sf::Keyboard::Down: m_inputs[0x8] = 0; break;
	case sf::Keyboard::Left: m_inputs[0x4] = 0; break;
	case sf::Keyboard::Right: m_inputs[0x6] = 0; break;
	case sf::Keyboard::Space: m_inputs[0x5] = 0; break;

	case sf::Keyboard::A: m_inputs[0xA] = 0; break;
	case sf::Keyboard::B: m_inputs[0xB] = 0; break;
	case sf::Keyboard::C: m_inputs[0xC] = 0; break;
	case sf::Keyboard::D: m_inputs[0xD] = 0; break;
	case sf::Keyboard::E: m_inputs[0xE] = 0; break;
	case sf::Keyboard::F: m_inputs[0xF] = 0; break;
	case sf::Keyboard::Num0: m_inputs[0x0] = 0; break;
	case sf::Keyboard::Num1: m_inputs[0x1] = 0; break;
	case sf::Keyboard::Num3: m_inputs[0x3] = 0; break;
	case sf::Keyboard::Num7: m_inputs[0x7] = 0; break;
	case sf::Keyboard::Num9: m_inputs[0x9] = 0; break;
	}
}


// ref: https://en.wikipedia.org/wiki/CHIP-8
void Cpu::Decode(word opcode)
{
	//cout << "Opcode: " << std::hex << opcode << endl;

	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x0F00)
		{
		case 0x0000:
			switch (opcode & 0x000F)
			{
			case 0x0000: Opcode00E0(); break;
			default: Opcode00EE(); break;
			}
			break;
		default: Opcode0NNN(); break;
		}
		break;
	case 0x1000: Opcode1NNN(opcode); break;
	case 0x2000: Opcode2NNN(opcode); break;
	case 0x3000: Opcode3XNN(opcode); break;
	case 0x4000: Opcode4XNN(opcode); break;
	case 0x5000: Opcode5XY0(opcode); break;
	case 0x6000: Opcode6XNN(opcode); break;
	case 0x7000: Opcode7XNN(opcode); break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: Opcode8XY0(opcode); break;
		case 0x0001: Opcode8XY1(opcode); break;
		case 0x0002: Opcode8XY2(opcode); break;
		case 0x0003: Opcode8XY3(opcode); break;
		case 0x0004: Opcode8XY4(opcode); break;
		case 0x0005: Opcode8XY5(opcode); break;
		case 0x0006: Opcode8XY6(opcode); break;
		case 0x0007: Opcode8XY7(opcode); break;
		case 0x000E: Opcode8XYE(opcode); break;
		}
		break;
	case 0x9000: Opcode9XY0(opcode); break;
	case 0xA000: OpcodeANNN(opcode); break;
	case 0xB000: OpcodeBNNN(opcode); break;
	case 0xC000: OpcodeCXNN(opcode); break;
	case 0xD000: OpcodeDXYN(opcode); break;
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: OpcodeEX9E(opcode); break;
		case 0x00A1: OpcodeEXA1(opcode); break;
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: OpcodeFX07(opcode); break;
		case 0x000A: OpcodeFX0A(opcode); break;
		case 0x0015: OpcodeFX15(opcode); break;
		case 0x0018: OpcodeFX18(opcode); break;
		case 0x001E: OpcodeFX1E(opcode); break;
		case 0x0029: OpcodeFX29(opcode); break;
		case 0x0033: OpcodeFX33(opcode); break;
		case 0x0055: OpcodeFX55(opcode); break;
		case 0x0065: OpcodeFX65(opcode); break;
		}
		break;
	}
}

// opcode is 2 byte
word Cpu::GetNextOpCode()
{
	word opCode = 0;
	opCode = m_GameMemory[m_ProgramCounter] << 8 | m_GameMemory[m_ProgramCounter + 1];
	m_ProgramCounter += 2;

	return opCode;
}

// Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
void Cpu::Opcode0NNN()
{
	// do nothing
}

// Clears the screen.
void Cpu::Opcode00E0()
{
	m_graphics->Clear();
}

// Returns from a subroutine.
void Cpu::Opcode00EE()
{
	m_ProgramCounter = m_Stack.back();
	m_Stack.pop_back();
}

// Jumps to address NNN.
void Cpu::Opcode1NNN(word opCode)
{
	m_ProgramCounter = opCode & 0x0FFF;
}

// Calls subroutine at NNN.
void Cpu::Opcode2NNN(word opCode)
{
	m_Stack.push_back(m_ProgramCounter);
	m_ProgramCounter = opCode & 0x0FFF;
}

// Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
void Cpu::Opcode3XNN(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte value = opCode & 0x00FF;
	if (m_Registers[rx] == value)
	{
		m_ProgramCounter += 2;
	}
}

// Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
void Cpu::Opcode4XNN(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte value = opCode & 0x00FF;
	if (m_Registers[rx] != value)
	{
		m_ProgramCounter += 2;
	}
}

// Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
void Cpu::Opcode5XY0(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	if (m_Registers[rx] == m_Registers[ry])
	{
		m_ProgramCounter += 2;
	}
}

// Sets VX to NN.
void Cpu::Opcode6XNN(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte value = opCode & 0x00FF;
	m_Registers[rx] = value;
}

// Adds NN to VX. (Carry flag is not changed)
void Cpu::Opcode7XNN(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte value = opCode & 0x00FF;
	m_Registers[rx] += value;
}


// 8XY0	Assign	Vx = Vy	Sets VX to the value of VY.
void Cpu::Opcode8XY0(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	m_Registers[rx] = m_Registers[ry];
}

// Sets VX to VX or VY. (Bitwise OR operation)
void Cpu::Opcode8XY1(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	m_Registers[rx] |= m_Registers[ry];
}

// Sets VX to VX and VY. (Bitwise AND operation)
void Cpu::Opcode8XY2(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	m_Registers[rx] &= m_Registers[ry];
}

// Sets VX to VX xor VY.
void Cpu::Opcode8XY3(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	m_Registers[rx] ^= m_Registers[ry];
}

// Adds VY to VX.VF is set to 1 when there's a carry, and to 0 when there isn't.
void Cpu::Opcode8XY4(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	word value = m_Registers[rx] + m_Registers[ry];
	m_Registers[0xF] = (value > 0xFF) ? 1 : 0;
	m_Registers[rx] = value & 0xFF;
}

// VY is subtracted from VX.VF is set to 0 when there's a borrow, and 1 when there isn't.
void Cpu::Opcode8XY5(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	m_Registers[0xF] = (m_Registers[rx] < m_Registers[ry]) ? 0 : 1;
	m_Registers[rx] -= m_Registers[ry];
}

// Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
void Cpu::Opcode8XY6(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	m_Registers[0xF] = m_Registers[rx] & 1;
	m_Registers[rx] >>= 1;
}

// Sets VX to VY minus VX.VF is set to 0 when there's a borrow, and 1 when there isn't.
void Cpu::Opcode8XY7(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	m_Registers[0xF] = (m_Registers[rx] > m_Registers[ry]) ? 0 : 1;
	m_Registers[rx] = m_Registers[ry] - m_Registers[rx];
}

// Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
void Cpu::Opcode8XYE(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	m_Registers[0xF] = m_Registers[rx] >> 7;
	m_Registers[rx] <<= 1;
}

// Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
void Cpu::Opcode9XY0(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	if (m_Registers[rx] != m_Registers[ry])
	{
		m_ProgramCounter += 2;
	}
}

// Sets I to the address NNN.
void Cpu::OpcodeANNN(word opCode)
{
	m_AddressI = opCode & 0x0FFF;
}

// Jumps to the address NNN plus V0.
void Cpu::OpcodeBNNN(word opCode)
{
	word value = opCode & 0x0FFF;
	m_ProgramCounter = value + m_Registers[0x0];
}

// Sets VX to the result of a bitwise and operation on a random number(Typically: 0 to 255) and NN.
void Cpu::OpcodeCXNN(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte value = opCode & 0x00FF;
	m_Registers[rx] = (rand() % 0x100) & value;
}

// Draws a sprite at coordinate(VX, VY) that has a width of 8 pixels and a height of N pixels.
// Each row of 8 pixels is read as bit - coded starting from memory location I; 
// I value doesn’t change after the execution of this instruction.As described above, 
// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
// and to 0 if that doesn’t happen
void Cpu::OpcodeDXYN(word opCode)
{
	// screen x & y
	byte rx = (opCode & 0x0F00) >> 8;
	byte ry = (opCode & 0x00F0) >> 4;
	byte sx = m_Registers[rx];
	byte sy = m_Registers[ry];

	// sprite height
	byte height = opCode & 0x000F;

	m_Registers[0xF] = 0;
	for (int dy = 0; dy < height; ++dy)
	{
		byte line = m_GameMemory[m_AddressI + dy];
		if (line == 0) continue;

		for (int dx = 0; dx < 8; ++dx)
		{
			// inverse due to the left most pixel is the most significant bit
			int mask = 1 << (7 - dx);
			int x = sx + dx;
			int y = sy + dy;

			int isOn = (line & mask) > 0 ? 1 : 0;
			if (isOn == 0) continue;

			if (m_graphics->m_ScreenData[x][y] == 1)
			{
				m_Registers[0xF] = 1;
			}

			m_graphics->m_ScreenData[x][y] ^= 1;
		}
	}
}

// Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
void Cpu::OpcodeEX9E(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte inputKey = m_Registers[rx];
	if (m_inputs[inputKey] == 1)
	{
		m_ProgramCounter += 2;
	}
}

// Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
void Cpu::OpcodeEXA1(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte inputKey = m_Registers[rx];
	if (m_inputs[inputKey] == 0)
	{
		m_ProgramCounter += 2;
	}
}

// Sets VX to the value of the delay timer.
void Cpu::OpcodeFX07(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	m_Registers[rx] = m_delayTimer;
}

// A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
void Cpu::OpcodeFX0A(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	int keyPressed = -1;
	for (int i = 0; i < 16; ++i)
	{
		if (m_inputs[i] == 1)
		{
			keyPressed = i;
		}
	}

	if (keyPressed == -1)
	{
		m_ProgramCounter -= 2;
	}
	else
	{
		m_Registers[rx] = keyPressed;
	}
}

// Sets the delay timer to VX.
void Cpu::OpcodeFX15(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	m_delayTimer = m_Registers[rx];
}

// Sets the sound timer to VX.
void Cpu::OpcodeFX18(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	m_soundTimer = m_Registers[rx];
}

// Adds VX to I.
// VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.
// This is an undocumented feature of the CHIP-8 and used by the Spacefight 2091! game.
void Cpu::OpcodeFX1E(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	unsigned long value = m_AddressI + m_Registers[rx];
	// m_Registers[0xF] = (value >> 16) > 0 ? 1 : 0; // is this needed?
	m_AddressI = value & 0xFFFF;
}

// Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
void Cpu::OpcodeFX29(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;

	m_AddressI = m_Registers[rx];
}

// Stores the binary-coded decimal representation of VX,
// with the most significant of three digits at the address in I, 
// the middle digit at I plus 1, and the least significant digit at I plus 2.
// (In other words, take the decimal representation of VX, 
// place the hundreds digit in memory at location in I,
// the tens digit at location I+1,
// and the ones digit at location I+2.)
void Cpu::OpcodeFX33(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;
	byte value = m_Registers[rx];
	m_GameMemory[m_AddressI] = value / 100;
	m_GameMemory[m_AddressI + 1] = (value / 10) % 10;
	m_GameMemory[m_AddressI + 2] = value % 10;
}

// Stores V0 to VX (including VX) in memory starting at address I.
// The offset from I is increased by 1 for each value written,
// but I itself is left unmodified.
void Cpu::OpcodeFX55(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;

	for (int i = 0; i <= rx; ++i)
	{
		m_GameMemory[m_AddressI + i] = m_Registers[i];
	}
	m_AddressI += rx + 1;
}

// Fills V0 to VX (including VX) with values from memory starting at address I.
// The offset from I is increased by 1 for each value written,
// but I itself is left unmodified.
void Cpu::OpcodeFX65(word opCode)
{
	byte rx = (opCode & 0x0F00) >> 8;

	for (int i = 0; i <= rx; ++i)
	{
		m_Registers[i] = m_GameMemory[m_AddressI + i];
	}
	m_AddressI += rx + 1;
}