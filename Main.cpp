#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "main.hpp"

const int map_size_x{ 30 };
const int map_size_y{ 30 };
const int window_size_x{ 720 };
const int window_size_y{ 720 };

const int square_size = (int)(std::min(window_size_x / map_size_x, window_size_y / map_size_y));

int map[map_size_y][map_size_x];
int map_view[map_size_y][map_size_x];
sf::RenderWindow app(sf::VideoMode(window_size_x, window_size_y), "Minesweeper!", sf::Style::Close);

sf::Texture mine;
sf::Texture flag;
sf::Texture square;
sf::Font font;

// Stores all the directions from one cell

int ways[][2] = { { -1, -1 },
	{ -1, 0 },
	{ -1, 1 },
	{ 0, -1 },
	{ 0, 1 },
	{ 1, -1 },
	{ 1, 0 },
	{ 1, 1 } };

void ShowMine(int x, int y) {
	sf::Sprite sprite;
	sprite.setTexture(mine);
	sprite.scale(sf::Vector2f((float)square_size / (float)mine.getSize().x, (float)square_size / (float)mine.getSize().y));
	sprite.setPosition(sf::Vector2f(x * square_size, y * square_size));
	app.draw(sprite);
}

void ShowSquareSprite(int x, int y) {
	sf::Sprite sprite;
	sprite.setTexture(square);
	sprite.scale(sf::Vector2f((float)square_size / (float)square.getSize().x, (float)square_size / (float)square.getSize().y));
	sprite.setPosition(sf::Vector2f(x * square_size, y * square_size));
	app.draw(sprite);
}

void ShowColorText(int x, int y) {
	sf::Color colors[] = {
						sf::Color::Blue,
						sf::Color::Green,
						sf::Color::Red,
						sf::Color::Magenta,
						sf::Color::Cyan,
						sf::Color::Blue,
						sf::Color::Green,
						sf::Color::Red,
	};

	sf::Text text;
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
	text.setString(std::to_string(map[y][x]));
	text.setCharacterSize(square_size);
	text.setFillColor(colors[map[y][x] - 1]);
	text.setPosition(sf::Vector2f(x * square_size + square_size / 4, y * square_size - square_size / 6));
	app.draw(text);
}

// To set the squares on the Window

void SetSquareOnScreen(int x, int y) {
	sf::RectangleShape rectangle(sf::Vector2f(x * square_size, y * square_size));
	rectangle.setSize(sf::Vector2f(square_size, square_size));
	rectangle.setPosition(sf::Vector2f(x * square_size, y * square_size));
	rectangle.setFillColor(sf::Color::White);
	rectangle.setOutlineColor(sf::Color::Black);
	rectangle.setOutlineThickness(1);
	app.draw(rectangle);
}

void ShowFlagONScreen(int x, int y) {
	sf::Sprite sprite;
	sprite.setTexture(flag);
	sprite.scale(sf::Vector2f((float)square_size / (float)flag.getSize().x, (float)square_size / (float)flag.getSize().y));
	sprite.setPosition(sf::Vector2f(x * square_size, y * square_size));
	app.draw(sprite);
}

void GameOver(std::string t)
{
	sf::Text text;
	text.setFont(font);
	text.setString(t);
	text.setFillColor(sf::Color::Red);
	text.setCharacterSize(std::min(window_size_x, window_size_y) / 6);
	app.draw(text);
	app.display();
	sf::Event e;
	while (1)
	{
		while (app.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			{
				app.close();
				return;
			}
		}
	}
}

void CheckWin()
{
	for (int y = 0; y < map_size_y; y++)
	{
		for (int x = 0; x < map_size_x; x++)
		{
			if (!(map[y][x] == map_view[y][x] || (map[y][x] == -1 && map_view[y][x] == 9) || (map[y][x] == 0 && map_view[y][x] == 10)))
			{
				return;
			}
		}
	}
	std::string t = "You won!";
	GameOver(t);
	return;
}
void OpenAllZeroAround(int x, int y)
{
	for (auto i : ways)
	{

		if (x - i[0] >= 0 && x - i[0] < map_size_x && y - i[1] >= 0 && y - i[1] < map_size_y && (map_view[y - i[1]][x - i[0]] == 10))
		{
			map_view[y - i[1]][x - i[0]] = map[y - i[1]][x - i[0]];
			if (map[y - i[1]][x - i[0]] == 0)
			{
				OpenAllZeroAround(x - i[0], y - i[1]);
			}
		}
	}
}

void SetNumbers()
{
	for (int y = 0; y < map_size_y; y++)
	{
		for (int x = 0; x < map_size_x; x++)
		{
			if (map[y][x] != -1)
			{
				int n = 0;
				for (auto i : ways)
				{
					if (x - i[0] >= 0 && x - i[0] < map_size_x && y - i[1] >= 0 && y - i[1] < map_size_y && map[y - i[1]][x - i[0]] == -1)
					{
						n++;
					}
				}
				map[y][x] = n;
			}
		}
	}
}

void GenerateMap()
{
	for (int y = 0; y < map_size_y; y++)
	{
		for (int x = 0; x < map_size_x; x++)
		{
			map_view[y][x] = 10;
			if (random(0, 5) == 1)
			{
				map[y][x] = -1; //mine
			}
			else
			{
				map[y][x] = 0;
			}
		}
	}
	SetNumbers();
}

void Loop()
{
	sf::Event e;
	while (app.isOpen())
	{
		while (app.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			{
				app.close();
			}

			if (e.type == sf::Event::MouseButtonPressed)
			{

				int x = (int)(e.mouseButton.x / square_size);
				int y = (int)(e.mouseButton.y / square_size);

				if (x < map_size_x && x >= 0 && y < map_size_y && y >= 0)
				{
					if (e.mouseButton.button == sf::Mouse::Right)
					{
						if (map_view[y][x] != 9 && map_view[y][x] == 10) // 9 - flag
						{
							map_view[y][x] = 9;
						}
						else if (map_view[y][x] == 9)
						{
							map_view[y][x] = 10;
						}
					}

					if (e.mouseButton.button == sf::Mouse::Left)
					{
						map_view[y][x] = map[y][x];
						if (map[y][x] == 0)
						{
							OpenAllZeroAround(x, y);
						}
						if (map[y][x] == -1)
						{
							std::string t = "Game over!";
							ShowMine(x, y);
							GameOver(t);
							//return;
						}
						CheckWin();
					}
				}
			}
		}

		//draw
		app.clear(sf::Color::Black);
		for (int y = 0; y < map_size_y; y++)
		{
			for (int x = 0; x < map_size_x; x++)
			{
				SetSquareOnScreen(x, y);
				if (map_view[y][x] == 10) 
				{
					ShowSquareSprite(x, y);
				}
				
				if (map_view[y][x] == -1)
				{ //mine
					ShowMine(x, y);
				}
				else if (map_view[y][x] == 9)
				{
					ShowFlagONScreen(x, y);
				}

				else if (map_view[y][x] != 10)
				{
					ShowColorText(x, y);
				}
			}
		}
		app.display();
	}
}

int main()
{
	//import Textures and font

	flag.loadFromFile("Textures/flag.png");
	mine.loadFromFile("Textures/mine.png");
	square.loadFromFile("Textures/Square.png");

	font.loadFromFile("Textures/FutureLight-BW15w.ttf");

	GenerateMap();
	Loop();
}