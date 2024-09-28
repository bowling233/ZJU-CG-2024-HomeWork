#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cmath>
#include <cassert>

class Color
{
public:
	constexpr Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
	const unsigned char r, g, b;
};

constexpr Color GRAY = Color(128, 128, 128);
constexpr Color LIGHT_GRAY = Color(192, 192, 192);
constexpr Color BLACK = Color(0, 0, 0);
constexpr Color WHITE = Color(255, 255, 255);
constexpr Color RED = Color(255, 0, 0);
constexpr Color GREEN = Color(0, 255, 0);
constexpr Color BLUE = Color(0, 0, 255);

class PPM_Image
{
public:
	PPM_Image(const int w, const int h) : width(w), height(h), data(new unsigned char[w * h * 3])
	{
		this->init();
	}
	~PPM_Image()
	{
		delete[] data;
	}
	void setPixel(const int w, const int h, const Color c = BLACK)
	{
		data[(h * width + w) * 3 + 0] = c.r;
		data[(h * width + w) * 3 + 1] = c.g;
		data[(h * width + w) * 3 + 2] = c.b;
	}
	void setPixelBias(const int w, const int h, const int x0, const int y0, const Color c = BLACK)
	{
		data[((h + y0) * width + (w + x0)) * 3 + 0] = c.r;
		data[((h + y0) * width + (w + x0)) * 3 + 1] = c.g;
		data[((h + y0) * width + (w + x0)) * 3 + 2] = c.b;
	}
	void init()
	{
		for (int i = 0; i < width * height * 3; ++i)
		{
			data[i] = 255;
		}
	}
	void write(const char *filename)
	{
		FILE *fp;
		fp = fopen(filename, "wb");

		fprintf(fp, "P6\n%d %d\n255\n", width, height);
		fwrite(data, width * height * 3, 1, fp);

		fclose(fp);
	}
	void plotXLine(const int y, const Color c)
	{
		for (int i = 0; i < width; ++i)
		{
			setPixel(i, y, c);
		}
	}
	void plotYLine(const int x, const Color c)
	{
		for (int i = 0; i < height; ++i)
		{
			setPixel(x, i, c);
		}
	}
	void plotGrid(const int xInterval, const int yInterval)
	{
		for (int i = 0; i < width; i += xInterval)
		{
			plotYLine(i, LIGHT_GRAY);
		}
		for (int i = 0; i < height; i += yInterval)
		{
			plotXLine(i, LIGHT_GRAY);
		}
	}
	void plotEllipse(const int x0, const int y0, const int a, const int b, const Color color = BLACK)
	{
		long x = -a, y = 0;
		long e2 = b, dx = (1 + 2 * x) * e2 * e2;
		long dy = x * x, err = dx + dy;

		do
		{
			setPixel(x0 - x, y0 + y, color);
			setPixel(x0 + x, y0 + y, color);
			setPixel(x0 + x, y0 - y, color);
			setPixel(x0 - x, y0 - y, color);
			e2 = 2 * err;
			if (e2 >= dx)
			{
				x++;
				err += dx += 2 * (long)b * b;
			}
			if (e2 <= dy)
			{
				y++;
				err += dy += 2 * (long)a * a;
			}
		} while (x <= 0);

		while(y++ < b)
		{
			setPixel(x0, y0 + y, color);
			setPixel(x0, y0 - y, color);
		}
	}
	void plotEllipseRotated(const int x0, const int y0, const double a, const double b, const double theta, const Color color = BLACK)
	{
		int x, y, XV, YV, YR, XH, XL;
		double aSq, Xf, Yf, XfSq, YfSq, A, B, C, D, A2, B2, C2,
			B_2, k1, k2, k3, k4,
			Fn, Fnw, Fw, Fsw, Fs,
			Fn_n, Fn_nw, Fnw_n, Fnw_nw,
			Fw_w, Fw_nw, Fnw_w, Fw_sw, Fsw_w, Fsw_sw,
			Fs_s, Fs_sw, Fsw_s,
			d1, d2, d3, d4,
			Xinit, Yinit, Xv, Yv, Xr, Yr, Xh, Yh, Xl, Yl,
			cross1, cross2, cross3, cross4;

		aSq = a * a;
		double c = sqrt(aSq - b * b);
		Xf = c * cos(theta);
		Yf = c * sin(theta);
		XfSq = Xf * Xf;
		YfSq = Yf * Yf;

		A = aSq - XfSq;
		B = -2 * Xf * Yf;
		C = aSq - YfSq;
		D = aSq * (YfSq - A);

		A2 = A + A;
		B2 = B + B;
		C2 = C + C;
		B_2 = B / 2;

		k1 = -B / C2;
		Xv = sqrt(-D / (A + B * k1 + C * k1 * k1));
		if (Xv < 0)
			Xv = -Xv;
		Yv = k1 * Xv;

		k2 = -B / A2;
		Yh = sqrt(-D / (A * k2 * k2 + B * k2 + C));
		if (Yh < 0)
			Yh = -Yh;
		Xh = k2 * Yh;

		k3 = (A2 - B) / (C2 - B);
		Xr = sqrt(-D / (A + B * k3 + C * k3 * k3));
		Yr = k3 * Xr;
		if (Xr < Yr * k1)
			Yr = -Yr;

		k4 = (-A2 - B) / (C2 + B);
		Xl = sqrt(-D / (A + B * k4 + C * k4 * k4));
		Yl = k4 * Xl;
		if (Xl > Yl * k1)
			Xl = -Xl;

		XV = round(Xv);
		YV = round(Yv);
		YR = round(Yr);
		XH = round(Xh);
		XL = round(Xl);

		x = XV;
		y = YV;
		Xinit = x - 0.5;
		Yinit = y + 1;
		Fn = C2 * Yinit + B * Xinit + C;
		Fnw = Fn - A2 * Xinit - B * Yinit + A - B;
		d1 = (A * Xinit * Xinit) + (B * Xinit * Yinit) + (C * Yinit * Yinit) + D;
		Fn_n = C2;
		Fn_nw = Fnw_n = C2 - B;
		Fnw_nw = A2 - B2 + C2;
		Fw_w = A2;
		Fw_nw = Fnw_w = A2 - B;
		Fsw_sw = A2 + B2 + C2;
		Fs_s = C2;
		Fw_sw = Fsw_w = A2 + B;
		Fs_sw = Fsw_s = C2 + B;

		cross1 = B - A;
		cross2 = A - B + C;
		cross3 = A + B + C;
		cross4 = A + B;
		while (y < YR)
		{
			setPixelBias(x, y, x0, y0, color);
			setPixelBias(-x, -y, x0, y0, color);
			y = y + 1;
			if ((d1 < 0) || (Fn - Fnw < cross1))
			{
				d1 = d1 + Fn;
				Fn = Fn + Fn_n;
				Fnw = Fnw + Fnw_n;
			}
			else
			{
				x = x - 1;
				d1 = d1 + Fnw;
				Fn = Fn + Fn_nw;
				Fnw = Fnw + Fnw_nw;
			}
		}

		Fw = Fnw - Fn + A + B + B_2;
		Fnw = Fnw + A - C;
		d2 = d1 + (Fw - Fn + C) / 2 + (A + C) / 4 - A;
		while (x > XH)
		{
			setPixelBias(x, y, x0, y0, color);
			setPixelBias(-x, -y, x0, y0, color);
			x = x - 1;
			if ((d2 < 0) || (Fnw - Fw < cross2))
			{
				y = y + 1;
				d2 = d2 + Fnw;
				Fw = Fw + Fw_nw;
				Fnw = Fnw + Fnw_nw;
			}
			else
			{
				d2 = d2 + Fw;
				Fw = Fw + Fw_w;
				Fnw = Fnw + Fnw_w;
			}
		}

		d3 = d2 + Fw - Fnw + C2 - B;
		Fw = Fw + B;
		Fsw = Fw - Fnw + Fw + C2 + C2 - B;
		while (x > XL)
		{
			setPixelBias(x, y, x0, y0, color);
			setPixelBias(-x, -y, x0, y0, color);
			x = x - 1;
			if ((d3 < 0) || (Fsw - Fw > cross3))
			{
				d3 = d3 + Fw;
				Fw = Fw + Fw_w;
				Fsw = Fsw + Fsw_w;
			}
			else
			{
				y = y - 1;
				d3 = d3 + Fsw;
				Fw = Fw + Fw_sw;
				Fsw = Fsw + Fsw_sw;
			}
		}

		Fs = Fsw - Fw - B;
		d4 = d3 - Fsw / 2 + Fs + A - (A + C - B) / 4;
		Fsw = Fsw + C - A;
		Fs = Fs + C - B_2;
		YV = -YV;
		while (y > YV)
		{
			setPixelBias(x, y, x0, y0, color);
			setPixelBias(-x, -y, x0, y0, color);
			y = y - 1;
			if ((d4 < 0) || (Fsw - Fs < cross4))
			{
				x = x - 1;
				d4 = d4 + Fsw;
				Fs = Fs + Fs_sw;
				Fsw = Fsw + Fsw_sw;
			}
			else
			{
				d4 = d4 + Fs;
				Fs = Fs + Fs_s;
				Fsw = Fsw + Fsw_s;
			}
		}
		setPixelBias(x, y, x0, y0, color);
		setPixelBias(-x, -y, x0, y0, color);
	}

private:
	const int width;
	const int height;
	unsigned char *const data;
};

int main()
{
	PPM_Image img(400, 300);
	// set range of pixels to black
	img.plotGrid(20, 20);
	img.plotEllipse(200, 150, 100, 50, BLUE);
	img.plotEllipseRotated(200, 150, 100, 50, 3.14/4, RED);
	img.write("test.ppm");
	return 0;
}