#pragma once

#include <SFML/Graphics.hpp>

class Graphics
{
public:
	static const word screenWidth = 64;
	static const word screenHeight = 32;
	static const word scale = 10;
	byte m_ScreenData[screenWidth][screenHeight];

private:
	sf::RenderWindow* m_window = nullptr;
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	sf::Uint8* m_pixels;

public:
	Graphics(sf::RenderWindow* window);
	~Graphics();

	void Clear();
	void Render();
};

