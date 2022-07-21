#pragma once
#include "vec3.h"
#include "rtweekend.h"
#include <fstream>
#include <algorithm>

class frameBuffer
{
public:


	frameBuffer(int width, int height) :_width(width), _height(height) {
		_colorBuffer = new color[_width * _height];
	}
	~frameBuffer() {
		delete[] _colorBuffer;
	}

	//left down corner is startPos
	void setColor(const int x, const int y, const color& input) {
		_colorBuffer[y * _width + x]= input;
	}

	const int getWidth()const { return _width; };
	const int getHeight()const { return _height; };


	void saveAsPPM(const char* name, float gamma)const {
		// save framebuffer to file
		FILE* fp = fopen(name, "wb");
		(void)fprintf(fp, "P6\n%d %d\n255\n", _width, _height);
		float deGamma = 1.0 / gamma;
		for (auto i = 0; i < _width * _height; ++i) {
			static unsigned char color888[3];
			color888[0] = (unsigned char)(255 * std::pow(clamp(_colorBuffer[i].x(), 0, 1), deGamma));
			color888[1] = (unsigned char)(255 * std::pow(clamp(_colorBuffer[i].y(), 0, 1), deGamma));
			color888[2] = (unsigned char)(255 * std::pow(clamp(_colorBuffer[i].z(), 0, 1), deGamma));
			fwrite(color888, 1, 3, fp);
		}
		fclose(fp);
	}


private:
	const int _width, _height;
	color* _colorBuffer;
};