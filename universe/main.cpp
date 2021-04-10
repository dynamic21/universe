#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using olc::Key;
using olc::vd2d;
using olc::Pixel;

using std::map;
using std::max;
using std::min;
using std::endl;
using std::cout;
using std::vector;

using std::chrono::seconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

#define G 1
#define controlFriction 0.1

class ball
{
public:
	vd2d pos;
	vd2d posv;
	Pixel color;

	ball(vd2d Pos, vd2d Posv, Pixel Color)
	{
		pos = Pos;
		posv = Posv;
		color = Color;
	}
};

class Example : public olc::PixelGameEngine
{
public:
	vd2d pos;
	vd2d posv;
	double zoom;

	vd2d halfScreen;
	unsigned int m_z;
	unsigned int m_w;

	vector<ball> balls;

	map<int, map<int, vector<int>>> grid;

	unsigned int intRand()
	{
		m_z = 36969 * (m_z & 65535) + (m_z >> 16);
		m_w = 18000 * (m_w & 65535) + (m_w >> 16);

		return (m_z << 16) + m_w;
	}

	double doubleRand() { return (intRand() + 1.0) * 2.328306435454494e-10; }

	Pixel mapToRainbow(double d)
	{
		d = d - 6.0 * int(d / 6);

		double r = (d > 3) ? max(0.0, min(1.0, d - 4)) : max(0.0, min(1.0, 2 - d));
		double g = (d > 2) ? max(0.0, min(1.0, 4 - d)) : max(0.0, min(1.0, d));
		double b = (d > 4) ? max(0.0, min(1.0, 6 - d)) : max(0.0, min(1.0, d - 2));

		return Pixel(r * 0xff, g * 0xff, b * 0xff);
	}

	void control(double fElapsedTime)
	{
		if (GetKey(Key::Q).bHeld) { zoom /= pow(2, fElapsedTime); }
		if (GetKey(Key::E).bHeld) { zoom *= pow(2, fElapsedTime); }

		if (GetKey(Key::W).bHeld || GetKey(Key::UP).bHeld) { posv.y -= 1000 / zoom * fElapsedTime; }
		if (GetKey(Key::A).bHeld || GetKey(Key::LEFT).bHeld) { posv.x -= 1000 / zoom * fElapsedTime; }
		if (GetKey(Key::S).bHeld || GetKey(Key::DOWN).bHeld) { posv.y += 1000 / zoom * fElapsedTime; }
		if (GetKey(Key::D).bHeld || GetKey(Key::RIGHT).bHeld) { posv.x += 1000 / zoom * fElapsedTime; }

		posv *= pow(controlFriction, fElapsedTime);
		pos += posv * fElapsedTime;
	}

	void gravity(double fElapsedTime)
	{
		for (int i = 0; i < balls.size() - 1; i++)
		{
			for (int j = i + 1; j < balls.size(); j++)
			{
				vd2d dpos = balls[j].pos - balls[i].pos;
				dpos *= G * fElapsedTime / dpos.mag2();

				balls[i].posv += dpos;
				balls[j].posv -= dpos;
			}
		}
	}

	void ballToBall(int i, int j)
	{
		vd2d dpos = balls[j].pos - balls[i].pos;
		double dis = dpos.mag2();

		if (dis < 1)
		{
			dpos /= sqrt(dis);
			dis = (balls[j].posv - balls[i].posv).dot(dpos);

			if (dis < 0)
			{
				dpos *= dis;
				balls[i].posv += dpos;
				balls[j].posv -= dpos;
			}
		}
	}

	void collision()
	{
		map<int, map<int, vector<int>>>::iterator find1;
		map<int, vector<int>>::iterator find2;

		for (map<int, map<int, vector<int>>>::iterator i = grid.begin(); i != grid.end(); i++)
		{
			for (map<int, vector<int>>::iterator j = i->second.begin(); j != i->second.end(); j++)
			{
				for (int k = 0; k < j->second.size(); k++)
				{
					for (int l = k + 1; l < j->second.size(); l++)
					{
						//DrawLine((balls[j->second[k]].pos - pos) * zoom + halfScreen, (balls[j->second[l]].pos - pos) * zoom + halfScreen);
						ballToBall(j->second[k], j->second[l]);
					}

					find1 = grid.find(i->first);

					if (find1 != grid.end())
					{
						find2 = find1->second.find(j->first + 1);

						if (find2 != find1->second.end())
						{
							for (int l = 0; l < find2->second.size(); l++)
							{
								//DrawLine((balls[j->second[k]].pos - pos) * zoom + halfScreen, (balls[find2->second[l]].pos - pos) * zoom + halfScreen);
								ballToBall(j->second[k], find2->second[l]);
							}
						}
					}

					find1 = grid.find(i->first + 1);

					if (find1 != grid.end())
					{
						find2 = find1->second.find(j->first - 1);

						if (find2 != find1->second.end())
						{
							for (int l = 0; l < find2->second.size(); l++)
							{
								//DrawLine((balls[j->second[k]].pos - pos) * zoom + halfScreen, (balls[find2->second[l]].pos - pos) * zoom + halfScreen);
								ballToBall(j->second[k], find2->second[l]);
							}
						}

						find2 = find1->second.find(j->first);

						if (find2 != find1->second.end())
						{
							for (int l = 0; l < find2->second.size(); l++)
							{
								//DrawLine((balls[j->second[k]].pos - pos) * zoom + halfScreen, (balls[find2->second[l]].pos - pos) * zoom + halfScreen);
								ballToBall(j->second[k], find2->second[l]);
							}
						}

						find2 = find1->second.find(j->first + 1);

						if (find2 != find1->second.end())
						{
							for (int l = 0; l < find2->second.size(); l++)
							{
								//DrawLine((balls[j->second[k]].pos - pos) * zoom + halfScreen, (balls[find2->second[l]].pos - pos) * zoom + halfScreen);
								ballToBall(j->second[k], find2->second[l]);
							}
						}
					}
				}
			}
		}
	}

	void drawScreen(double fElapsedTime)
	{
		Clear(Pixel(0, 0, 0));
		grid.clear();

		for (int i = 0; i < balls.size(); i++)
		{
			balls[i].pos += balls[i].posv * fElapsedTime;
			grid[int(balls[i].pos.x)][int(balls[i].pos.y)].push_back(i);
			vd2d bPos = (balls[i].pos - pos) * zoom;
			//if (vd2d{ abs(bPos.x), abs(bPos.x) } < halfScreen)
			FillCircle(bPos + halfScreen, zoom * 0.5, balls[i].color);
		}
	}

	bool OnUserCreate() override
	{
		zoom = 16;
		halfScreen = { (double)ScreenWidth() / 2, (double)ScreenHeight() / 2 };
		m_z = (unsigned int)duration_cast<seconds>(high_resolution_clock::now().time_since_epoch()).count();
		m_w = (unsigned int)duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

		for (int i = 0; i < 100000; i++)
		{
			double randNum1 = doubleRand() * 6.28318530718;
			double randNum2 = doubleRand() * 6.28318530718;
			vd2d bPos = (vd2d{ cos(randNum1), sin(randNum1) }) * ScreenHeight() * (1 - doubleRand() * doubleRand());
			vd2d bPosv = (vd2d{ cos(randNum2), sin(randNum2) }) * 10;
			Pixel bColor = mapToRainbow(doubleRand() * 2 + 0.1 * sqrt(bPos.x * bPos.x + bPos.y * bPos.y));
			ball newBall(bPos, bPosv, bColor);
			balls.push_back(newBall);
			grid[int(bPos.x)][int(bPos.y)].push_back(i);
		}

		return true;
	}

	bool OnUserUpdate(double fElapsedTime) override
	{
		drawScreen(0.01);
		control(fElapsedTime);
		//gravity(fElapsedTime);
		collision();

		return true;
	}
};

int main()
{
	Example demo;

	if (demo.Construct(500, 500, 2, 2))
		demo.Start();

	return 0;
}