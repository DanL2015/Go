#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
using namespace std;
using namespace sf;

//black starts first
//encirclement no need to be diagonal
//encirclement when all neighboring squares captured by an opponent (including for sides and corner, for sides only need 3, corners only need 2)

int windowx = 855; // Each piece will have a size of 45x45
int windowy = 855;
// Go has a board of size 19 by 19
int board[19][19]; // 2 for white, 1 for black, 0 for unoccupied
RenderWindow window(VideoMode(windowx, windowy), "SFML Go", Style::Close | Style::Titlebar);
int turn = 1;
double m1x = -1;
double m1y = -1;
double m2x = -1;
double m2y = -1;
int i1x = -1;
int i1y = -1;
int i2x = -1;
int i2y = -1;

void initialize()
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			board[i][j] = 0;
		}
	}
}

void drawBoard() //draws the board with no pieces on it
{
	RectangleShape background;
	background.setSize(Vector2f(855, 855));
	background.setFillColor(Color::White);
	window.draw(background);

	for (int i = 0; i < 19; i++)
	{
		RectangleShape gridLine;
		gridLine.setSize(Vector2f(1, 810));
		gridLine.setFillColor(Color::Black);
		gridLine.setPosition(Vector2f(i * 45 + 22.5, 22.5));
		window.draw(gridLine);
	}

	for (int i = 0; i < 19; i++)
	{
		RectangleShape gridLine;
		gridLine.setSize(Vector2f(810, 1));
		gridLine.setFillColor(Color::Black);
		gridLine.setPosition(Vector2f(22.5, i * 45 + 22.5));
		window.draw(gridLine);
	}

	//put dots at each 3x3 square thing and in the middles
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			CircleShape dot;
			dot.setFillColor(Color::Black);
			dot.setRadius(5);
			dot.setPosition(Vector2f(157 - dot.getRadius()+1 + 6*45*i, 157 - dot.getRadius()+1 + 6 * 45 * j));
			window.draw(dot);
		}
	}
}

void drawPieces()
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (board[i][j] == 1)
			{
				//black piece
				Texture blackTexture;
				blackTexture.loadFromFile("Textures/black.png");
				Sprite black;
				black.setTexture(blackTexture);
				black.setPosition(i * 45, j * 45);
				window.draw(black);
			}
			else if (board[i][j] == 2)
			{
				//white piece
				Texture whiteTexture;
				whiteTexture.loadFromFile("Textures/white.png");
				Sprite white;
				white.setTexture(whiteTexture);
				white.setPosition(i * 45, j * 45);
				window.draw(white);
			}
		}
	}
}

//need to floodfill at every place
void updateBlack() //for some reason, always erasing a black (fixed)
{
	//floodfill all back locations
	//check if there is any Æø around it (count total)
	//if none, black is fully surrounded - erase
	int dx[4] = { 0, 1, -1, 0 };
	int dy[4] = { 1, 0, 0, -1 };
	int cboard[19][19];
	//black check
	int num = 1;
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			cboard[i][j] = 0;
		}
	}
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (board[i][j] == 1 && cboard[i][j] == 0)
			{
				cboard[i][j] = num;
				queue <pair <int, int> > q;
				q.push({ i, j });
				while (!q.empty())
				{
					int cx = q.front().first;
					int cy = q.front().second;
					q.pop();
					for (int k = 0; k < 4; k++)
					{
						int nx = cx + dx[k];
						int ny = cy + dy[k];
						if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19) continue;
						if (board[nx][ny] == 1 && cboard[nx][ny] == 0)
						{
							cboard[nx][ny] = num;
							q.push({nx, ny});
						}
					}
				}
				num++;
			}
		}
	}
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			cout << cboard[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	vector <int> qi;
	qi.assign(num, 0);
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (cboard[i][j] != 0)
			{
				int cqi = 0;
				for (int k = 0; k < 4; k++)
				{
					int nx = i + dx[k];
					int ny = j + dy[k];
					if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19) continue;
					if (board[nx][ny] == 0)
					{
						cqi++;
					}
				}
				qi[cboard[i][j]] += cqi;
			}
		}
	}
	for (int i = 1; i < qi.size(); i++)
	{
		cout << qi[i] << endl;
	}
	cout << endl;
	set<int> nerase;
	for (int i = 1; i < qi.size(); i++)
	{
		if (qi[i] == 0)
		{
			nerase.insert(i);
		}
	}
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (nerase.find(cboard[i][j]) != nerase.end() && cboard[i][j]!=0)
			{
				cout << "erased" << endl;
				board[i][j] = 0;
			}
		}
	}
	num = 1;
}

void updateWhite()
{
	int dx[4] = { 0, 1, -1, 0 };
	int dy[4] = { 1, 0, 0, -1 };
	int cboard[19][19];
	//black check
	int num = 1;
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			cboard[i][j] = 0;
		}
	}
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (board[i][j] == 2 && cboard[i][j] == 0)
			{
				cboard[i][j] = num;
				queue <pair <int, int> > q;
				q.push({ i, j });
				while (!q.empty())
				{
					int cx = q.front().first;
					int cy = q.front().second;
					q.pop();
					for (int k = 0; k < 4; k++)
					{
						int nx = cx + dx[k];
						int ny = cy + dy[k];
						if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19) continue;
						if (board[nx][ny] == 2 && cboard[nx][ny] == 0)
						{
							cboard[nx][ny] = num;
							q.push({ nx, ny });
						}
					}
				}
				num++;
			}
		}
	}
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			cout << cboard[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	vector <int> qi;
	qi.assign(num, 0);
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (cboard[i][j] != 0)
			{
				int cqi = 0;
				for (int k = 0; k < 4; k++)
				{
					int nx = i + dx[k];
					int ny = j + dy[k];
					if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19) continue;
					if (board[nx][ny] == 0)
					{
						cqi++;
					}
				}
				qi[cboard[i][j]] += cqi;
			}
		}
	}
	for (int i = 1; i < qi.size(); i++)
	{
		cout << qi[i] << endl;
	}
	cout << endl;
	set<int> nerase;
	for (int i = 1; i < qi.size(); i++)
	{
		if (qi[i] == 0)
		{
			nerase.insert(i);
		}
	}
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (nerase.find(cboard[i][j]) != nerase.end() && cboard[i][j] != 0)
			{
				cout << "erased" << endl;
				board[i][j] = 0;
			}
		}
	}
}

int main()
{
	initialize();
	while (window.isOpen())
	{
		Event evnt;
		while (window.pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
				window.close();
		}
		//input
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (m1x == -1 || m1y == -1)
			{
				m1x = Mouse::getPosition(window).x;
				m1y = Mouse::getPosition(window).y;
				i1x = (m1x) / 45;
				i1y = (m1y) / 45;
			}
			else if (m2x == -1 || m2y == -1) //basically need to double click to place
			{
				m2x = Mouse::getPosition(window).x;
				m2y = Mouse::getPosition(window).y;
				i2x = (m2x) / 45;
				i2y = (m2y) / 45;
				if (i1x == i2x && i1y == i2y && board[i1x][i1y] == 0)
				{
					//want to place piece at i1x, i1y
					board[i1x][i1y] = turn;
					turn = turn % 2 + 1;
					//update the board to check for any cutoffs
					updateBlack();
					updateWhite();
				}
				m1x = -1;
				m1y = -1;
				m2x = -1;
				m2y = -1;
				i1x = -1;
				i1y = -1;
				i2x = -1;
				i2y = -1;
			}
		}

		if (Mouse::isButtonPressed(Mouse::Right))
		{
			m1x = -1;
			m1y = -1;
			m2x = -1;
			m2y = -1;
			i1x = -1;
			i1y = -1;
			i2x = -1;
			i2y = -1;
		}

		//display
		window.clear();
		drawBoard();
		drawPieces();
		if (i1x != -1 && i1y != -1 && m1x != -1 && m1y != -1)
		{
			Texture selectTexture;
			selectTexture.loadFromFile("Textures/select.png");
			Sprite select;
			select.setTexture(selectTexture);
			select.setPosition(Vector2f((float)i1x * 45, (float)i1y * 45));
			window.draw(select);
		}
		window.display();
		if (turn == 1)
		{
			window.setTitle("SFML Go | Black's Turn");
		}
		else window.setTitle("SFML Go | White's Turn");
	}
	return 0;
}