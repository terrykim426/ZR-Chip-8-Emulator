#include "stdafx.h"
#include "Graphics.h"
#include <iostream>

using namespace std;

Graphics::Graphics(sf::RenderWindow* window)
{
	m_window = window;

	if (!m_texture.create(screenWidth, screenHeight))
	{
		// error...
		std::cout << "Unable to create texture";
	}

	m_pixels = new sf::Uint8[screenWidth * screenHeight * 4];

	m_sprite = sf::Sprite(m_texture);
	m_sprite.setScale(scale, scale);
}


Graphics::~Graphics()
{
	delete[] m_pixels;
	m_pixels = nullptr;

	m_window = nullptr;
}

void Graphics::Clear()
{
	for (int dy = 0; dy < screenHeight; ++dy)
	{
		for (int dx = 0; dx < screenWidth; ++dx)
		{
			m_ScreenData[dx][dy] = 0;
		}
	}

	m_window->clear();
}

void Graphics::Render()
{
	for (int dy = 0; dy < screenHeight; ++dy)
	{
		for (int dx = 0; dx < screenWidth; ++dx)
		{
			int index = (dy * screenWidth + dx) * 4;
			m_pixels[index] = m_ScreenData[dx][dy] == 1 ? 0xFF : 0;
			m_pixels[index + 1] = m_ScreenData[dx][dy] == 1 ? 0xFF : 0;
			m_pixels[index + 2] = m_ScreenData[dx][dy] == 1 ? 0xFF : 0;
			m_pixels[index + 3] = 0xFF;
		}
	}
	m_texture.update(m_pixels);

	// draw it to the window
	//window.clear();
	m_window->draw(m_sprite);
	m_window->display();
}
