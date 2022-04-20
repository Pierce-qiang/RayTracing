#pragma once
#include "vec3.h"
#include <fstream>
#include <algorithm>

class FrameBuffer
{
public:


	FrameBuffer(int width, int height) :_width(width), _height(height) {
		_colorBuffer = new color[_width * _height];
	}
	~FrameBuffer() {
		delete[] _colorBuffer;
	}

	//left down corner is startPos
	void SetColor(const int x, const int y, const color& input) {
		_colorBuffer[y * _width + x]= input;
	}

	const int GetWidth()const { return _width; };
	const int GetHeight()const { return _height; };


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