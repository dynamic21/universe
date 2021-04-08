#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using olc::Key;
using olc::vd2d;
using olc::Pixel;

using std::max;
using std::min;
using std::endl;
using std::cout;
using std::vector;

using std::chrono::seconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

#define gravity 1
#define controlFriction 0.1

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "universe";
	}

public:
	vd2d pos;
	vd2d posv;
	double zoom;

	vd2d halfScreen;
	unsigned int m_z;
	unsigned int m_w;

	vector<vd2d> poss;
	vector<vd2d> posvs;
	vector<Pixel> colors;

	unsigned int intRand()
	{
		m_z = 36969 * (m_z & 65535) + (m_z >> 16);
		m_w = 18000 * (m_w & 65535) + (m_w >> 16);

		return (m_z << 16) + m_w;
	}

	double doubleRand() { return (intRand() + 1.0) * 2.328306435454494e-10; }

	Pixel mapToRainbow(double d) {
		double r = (d > 3) ? max(0.0, min(1.0, d - 4)) : max(0.0, min(1.0, 2 - d));
		double g = (d > 2) ? max(0.0, min(1.0, 4 - d)) : max(0.0, min(1.0, d));
		double b = (d > 4) ? max(0.0, min(1.0, 6 - d)) : max(0.0, min(1.0, d - 2));

		return Pixel(r * 0xff, g * 0xff, b * 0xff);
	}

	void control(float fElapsedTime)
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

	void collision(float fElapsedTime)
	{
		for (int i = 0; i < posvs.size() - 1; i++)
		{
			vd2d mposv = posvs[i];

			for (int j = i + 1; j < posvs.size(); j++)
			{
				vd2d dpos = poss[j] - poss[i];
				double dis = dpos.mag2();

				mposv += dpos / dis * gravity * fElapsedTime;
				posvs[j] -= dpos / dis * gravity * fElapsedTime;

				if (dis < 4) {

					dis = sqrt(dis);
					vd2d dposv = posvs[j] - mposv;
					vd2d npos = dpos / dis;
					double mag = dposv.dot(npos);

					if (mag < 0) {
						vd2d dapply = npos * mag;
						mposv += dapply;
						posvs[j] -= dapply;
					}
				}
			}
			posvs[i] = mposv;
		}
	}

	void drawScreen(float fElapsedTime)
	{
		Clear(Pixel(0, 0, 0));

		for (int i = 0; i < poss.size(); i++)
		{
			poss[i] = poss[i] + posvs[i] * fElapsedTime;
			FillCircle((poss[i] - pos) * zoom + halfScreen, zoom, colors[i]);
		}
	}

	bool OnUserCreate() override
	{
		zoom = 16;
		halfScreen = { (double)ScreenWidth() / 2, (double)ScreenHeight() / 2 };
		m_z = (unsigned int)duration_cast<seconds>(high_resolution_clock::now().time_since_epoch()).count();
		m_w = (unsigned int)duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

		for (int i = 0; i < 2000; i++)
		{
			poss.push_back((vd2d{ doubleRand() * ScreenWidth(), doubleRand() * ScreenHeight() } - halfScreen));
			posvs.push_back(vd2d{ 0, 0 });
			colors.push_back(mapToRainbow(doubleRand() * 6));
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		control(fElapsedTime);
		collision(0.01); // defined until stable fps
		drawScreen(0.01); // defined until stable fps

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