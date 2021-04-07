#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using olc::Key;
using olc::vd2d;
using olc::Pixel;

using std::max;
using std::endl;
using std::cout;
using std::vector;

using std::chrono::seconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

#define gravity 100000
#define userFriction 0.1

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "universe";
	}

public:
	unsigned int m_z;
	unsigned int m_w;
	vector<vd2d> poss;
	vector<vd2d> posvs;
	vector<vd2d> poscs;
	vector<int> size;
	double zoom;
	vd2d posv;
	vd2d pos;
	vd2d halfScreen;

	unsigned int intRand()
	{
		m_z = 36969 * (m_z & 65535) + (m_z >> 16);
		m_w = 18000 * (m_w & 65535) + (m_w >> 16);

		return (m_z << 16) + m_w;
	}

	double doubleRand()
	{
		return (intRand() + 1.0) * 2.328306435454494e-10;
	}

	void applyForce(float fElapsedTime, bool on) {

		for (int i = 0; i < posvs.size() - 1; i++)
		{
			vd2d mpos = poss[i];
			vd2d mposv = posvs[i];
			int mSize = size[i];
			for (int j = i + 1; j < posvs.size(); j++)
			{
				int totalSize = mSize + size[j];
				vd2d dpos = poss[j] - mpos;
				double dis = dpos.mag2();

				/*DrawLine((mpos - pos) * zoom + halfScreen, (mpos + (dpos / dis * gravity) - pos) * zoom + halfScreen, olc::RED);
				DrawLine((poss[j] - pos) * zoom + halfScreen, (poss[j] - (dpos / dis * gravity) - pos) * zoom + halfScreen, olc::RED);*/

				if (on) {
					/*cout << "ball " << i << " and ball " << j << " is " << dpos << endl;
					cout << "ball " << i << " is " << mpos << endl;
					cout << "ball " << j << " is " << poss[j] << endl;
					cout << "dis " << dis << endl;
					cout << "dpos / dis " << (dpos / dis) << endl;
					cout << "gravity * dpos / dis " << (dpos / dis * gravity) << endl;
					cout << endl;*/
					mposv += dpos / dis * gravity * fElapsedTime;
					posvs[j] -= dpos / dis * gravity * fElapsedTime;

					if (dpos.x < totalSize && dpos.y < totalSize) {
						if (totalSize * totalSize > dis) {

							dis = sqrt(dis);
							double depth = dis - totalSize;
							vd2d dposv = posvs[j] - mposv;
							vd2d npos = dpos / dis;
							double mag = dposv.dot(npos);

							if (mag <= 0)
							{

								//cout << "mag before: " << mag << " and depth before: " << depth << endl;
								depth = max(mag, depth);
								mag -= depth;
								//cout << "mag after: " << mag << " and depth after: " << depth << endl;

								vd2d dapply = npos * mag * 0.9;
								mposv += dapply;
								posvs[j] -= dapply;

								dapply = npos * (depth / 2);
								poscs[i] += dapply;
								poscs[j] -= dapply;/**/
							}
						}
					}
				}
			}
			posvs[i] = mposv;
		}
	}

	void move(float fElapsedTime) {
		for (int i = 0; i < poss.size(); i++)
		{
			poss[i] = poss[i] + poscs[i] + (posvs[i] * fElapsedTime);
			poscs[i] = { 0,0 };
		}
	}

	void user(float fElapsedTime) {
		//fElapsedTime /= 10;
		if (GetKey(Key::Q).bHeld) { zoom /= pow(2, fElapsedTime); }
		if (GetKey(Key::E).bHeld) { zoom *= pow(2, fElapsedTime); }

		if (GetKey(Key::W).bHeld || GetKey(Key::UP).bHeld) { posv.y -= 1000 / zoom * fElapsedTime; }
		if (GetKey(Key::A).bHeld || GetKey(Key::LEFT).bHeld) { posv.x -= 1000 / zoom * fElapsedTime; }
		if (GetKey(Key::S).bHeld || GetKey(Key::DOWN).bHeld) { posv.y += 1000 / zoom * fElapsedTime; }
		if (GetKey(Key::D).bHeld || GetKey(Key::RIGHT).bHeld) { posv.x += 1000 / zoom * fElapsedTime; }

		posv *= pow(userFriction, fElapsedTime);
		pos += posv * fElapsedTime;
	}

	void drawScreen(float fElapsedTime) {
		Clear(Pixel(0, 0, 0));

		for (int i = 0; i < poss.size(); i++)
		{
			DrawCircle((poss[i] - pos) * zoom + halfScreen, size[i] * zoom);
			//DrawLine((poss[i] - pos) * zoom + halfScreen, (poss[i] + posvs[i] - pos) * zoom + halfScreen);
		}
	}

	bool OnUserCreate() override
	{
		halfScreen = { (double)ScreenWidth() / 2, (double)ScreenHeight() / 2 };
		m_z = (unsigned int)duration_cast<seconds>(high_resolution_clock::now().time_since_epoch()).count();
		m_w = (unsigned int)duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
		zoom = (double)1 / 1;
		for (int i = 0; i < 2000; i++)
		{
			poss.push_back((vd2d{ doubleRand() * ScreenWidth(), doubleRand() * ScreenHeight() } - halfScreen) * 10);
			posvs.push_back(vd2d{ doubleRand() * 200 - 100, doubleRand() * 200 - 100 });
			poscs.push_back(vd2d{ 0,0 });
			size.push_back(100);
		}/**/
		/*poss.push_back(vd2d{ 300, 0 });
		posvs.push_back(vd2d{ 0, 0 });
		poscs.push_back(vd2d{ 0,0 });
		size.push_back(100);
		poss.push_back(vd2d{ 0, 0 });
		posvs.push_back(vd2d{ 0, 0 });
		poscs.push_back(vd2d{ 0,0 });
		size.push_back(100);
		poss.push_back(vd2d{ 0, 100 });
		posvs.push_back(vd2d{ 0, 100 });
		poscs.push_back(vd2d{ 0,0 });
		size.push_back(100);*/
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		user(fElapsedTime);
		drawScreen(fElapsedTime);
		//GetKey(Key::SPACE).bHeld
		applyForce(fElapsedTime, GetKey(Key::SPACE).bHeld);
		if (GetKey(Key::SPACE).bHeld) {
			move(fElapsedTime);
		}

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(1000, 1000, 1, 1))
		demo.Start();
	return 0;
}