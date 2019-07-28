// Chip8Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Cpu.h"
#include "Graphics.h"

using namespace std;


sf::RenderWindow* m_window = NULL;
Graphics* m_graphics;
Cpu* m_cpu;



int main()
{
	m_window = new sf::RenderWindow(sf::VideoMode(Graphics::screenWidth * Graphics::scale, Graphics::screenHeight * Graphics::scale), "ZR Chip 8 Emulator");

	m_graphics = new Graphics(m_window);
	m_cpu = new Cpu(m_graphics);

	m_cpu->Reset();

	while (m_window->isOpen())
	{
		m_cpu->Run();


		sf::Event event;
		while (m_window->pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				m_window->close();
				break;
			case sf::Event::KeyPressed:
				m_cpu->OnKeyPressed(event.key);
				break;
			case sf::Event::KeyReleased:
				m_cpu->OnKeyReleased(event.key);
				break;
			}
		}

		m_graphics->Render();
	}

	delete m_cpu;
	m_cpu = nullptr;

	delete m_graphics;
	m_graphics = nullptr;

	delete m_window;
	m_window = nullptr;

	return 0;
}