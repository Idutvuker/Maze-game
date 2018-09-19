#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <conio.h>
#include <vector>

using namespace sf;

const int H = 20, W = 40;

float p_size = 40.f;

Vector2f multVector(Vector2f vect, float c)
{
	return Vector2f(vect.x * c, vect.y * c);
}

Vector2f devVector(Vector2f vect, float c)
{
	return Vector2f(vect.x / c, vect.y / c);
}

Vector2f normVector(Vector2f vect)
{
	float dist = sqrt(vect.x*vect.x + vect.y*vect.y);
	return Vector2f(vect.x / dist, vect.y / dist);
}

std::string map[H] = {
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"W                                      W",
	"WWWWWWWWWWWWWWWWwwwwwwwwwwwwwwwwwwwwWWWW" };

int collisionS(Vector2f speed, Vector2f pos, char ch)
{
	Vector2i pos32(devVector(pos, 32.0));

	if (pos.x<0 || pos.x>32 * W || pos.y<0 || pos.y>32 * H)
		return 0;

	for (int x = pos32.x; x < pos32.x + 2; x++)
	{
		for (int y = pos32.y; y < pos32.y + 2; y++)
		{
			if (map[y][x] == ch && FloatRect(pos.x, pos.y, p_size, p_size).intersects(FloatRect(x * 32, y * 32, 32, 32)))
			{
				return 1;
			}
		}
	}
	return 0;
}

int random(int f, int l)
{
	return f + rand() % (l - f + 1);
}

void printMap()
{
	for (int y = 0; y < H; y++)
	{
		std::cout << map[y] << std::endl;
	}
}

void tunnel(int pos[2])
{
	int i, n, r = 0, b = 0, dir[2] = { 0, 1 }, m_size[2] = { W, H };

	for (n = 0; n < 20; n++)
	{
		map[pos[1]][pos[0]] = 'W';


		// Near border
		for (i = 0; i < 2; i++)
		{
			//std::cout <<pos[i] <<"____"<< m_size[i] << std::endl;
			if (pos[i] == m_size[i] - 2 || pos[i] == 1)
			{
				//r = !i;
				dir[i] = 0;
				dir[!i] = random(0, 1) * 2 - 1;
				b += 1;
			}
		}
		// If 2 borders
		if (b == 2) break;

		// If no border
		if (b == 0)
		{
			r = random(0, 1);

			// Changing direction
			if (dir[r] == 0)
			{
				dir[!r] = 0;
				dir[r] = random(0, 1) * 2 - 1;
			}
		}

		b = 0;
		pos[0] += dir[0];
		pos[1] += dir[1];

		// Debug
		//printMap();
		//_getch();
		//system("cls");		
	}
}

int genMap()
{
	int pos[2] = { 10, 5 }, r1, r2;

	for (int i = 0; i < 10; i++)
	{
		tunnel(pos);
		pos[0] = random(1, W - 2);
		pos[1] = random(1, H - 2);
	}

	r1 = random(1, H - 2);
	map[r1][0] = 'D';
	map[r1][1] = ' ';
	map[r1][2] = ' ';

	r2 = random(1, H - 2);
	map[r2][W - 1] = 'D';
	map[r2][W - 2] = ' ';
	map[r2][W - 3] = ' ';

	return r1;
}

struct Bullet
{
	Vector2f pos;
	Vector2f dir;

	bool bullCollision()
	{
		Vector2i pos32(devVector(pos, 32.0));

		if (pos32.x > 0 && pos32.x < W - 1 && pos32.y > 0 && pos32.y < H - 1)
		{
			if (map[pos32.y][pos32.x] == 'W')
			{
				map[pos32.y][pos32.x] = ' ';
				return 1;
			}
		}
		else return 1;

		return 0;
	}

	bool update()
	{
		pos += multVector(dir, 5.0);

		if (bullCollision())	return 1;

		return 0;
	}

};

std::vector <Bullet> vec_bullets;

int main()
{
	int x, y;
	srand(time(0));
	y = genMap();

	RenderWindow window(VideoMode(1280, 640), "Great game");
	window.setFramerateLimit(120);

	RectangleShape player(Vector2f(p_size, p_size));
	player.setFillColor(Color(255, 255, 0));
	player.setPosition(10, y * 32 + 12);

	player.setOutlineThickness(-2);
	player.setOutlineColor(Color(250, 150, 100));

	int bun = 5;
	bool l_mouse = false;

	Vector2f speed;
	speed = Vector2f(0.0, 0.0);
	float sp = 2.5;

	Event event;

	Font font;
	font.loadFromFile("fonts/arial.ttf");

	Text text;
	text.setFont(font);
	text.setCharacterSize(30);
	text.setColor(Color(255, 0, 0));
	text.setPosition(10, 0);
	text.setStyle(Text::Bold);

	Texture wall, door;
	wall.loadFromFile("textures/texture_o_icon.jpg");
	door.loadFromFile("textures/door.png");

	Sprite sprite;
	sprite.setTextureRect(IntRect(0, 0, 32, 32));

	while (window.isOpen())
	{
		Vector2f p_pos = player.getPosition();

		if (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		speed = multVector(speed, 0.93);

		//Keyboard
		if (Keyboard::isKeyPressed(Keyboard::Escape))
			window.close();

		if (Keyboard::isKeyPressed(Keyboard::W))
			speed.y = -sp;

		if (Keyboard::isKeyPressed(Keyboard::S))
			speed.y = sp;

		if (Keyboard::isKeyPressed(Keyboard::D))
			speed.x = sp;

		if (Keyboard::isKeyPressed(Keyboard::A))
			speed.x = -sp;

		//Mouse
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (!l_mouse && bun > 0)
			{
				Bullet bullet;
				bullet.pos = p_pos + Vector2f(20.0, 20.0);
				bullet.dir = normVector(Vector2f(Mouse::getPosition(window)) - player.getPosition());
				vec_bullets.push_back(bullet);

				bun--;
				l_mouse = true;
			}
		}
		else l_mouse = false;


		//Background
		window.clear(Color(107, 140, 255));

		//Collision
		Vector2f npos(p_pos + Vector2f(speed.x, 0));
		if (collisionS(speed, npos, 'W'))
			speed.x = 0;

		npos = Vector2f(p_pos + Vector2f(0, speed.y));
		if (collisionS(speed, npos, 'W'))
			speed.y = 0;

		//Move
		player.move(speed);

		//Bullets

		RectangleShape rect(Vector2f(20.f, 20.f));
		rect.setOrigin(10, 10);

		for (int i = 0; i < vec_bullets.size(); i++)
		{
			Bullet &bullet = vec_bullets[i];

			if (bullet.update())
			{
				vec_bullets.erase(vec_bullets.begin() + i);
				i--;
			}

			rect.setPosition(bullet.pos);
			window.draw(rect);
		}

		//MAP
		for (y = 0; y < H; y++)
		{
			for (x = 0; x < W; x++)
			{
				if (map[y][x] == 'W')
				{
					sprite.setTexture(wall);
				}

				if (map[y][x] == 'D')
				{
					sprite.setTexture(door);
				}

				if (map[y][x] == ' ')
					continue;

				sprite.setPosition(x * 32, y * 32);
				window.draw(sprite);
			}
		}

		text.setString("Ammo: " + std::to_string(bun));
		window.draw(text);

		window.draw(player);
		window.display();
	}

	return 0;

}