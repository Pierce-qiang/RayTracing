#pragma once
#include "frameBuffer.h"
#include "scene.h"
#include<chrono>
#include <mutex>
#include <thread>

class Renderer
{
public:

	void render(const scene& scene, shared_ptr<frameBuffer> m_frameBuffer, int samples_per_pixel, int max_depth);
private:
	std::mutex print_mutex;
};