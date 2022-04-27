#pragma once

class Renderer
{
public:
	Renderer();
	~Renderer();

	void render(const Scene& scene, shared_ptr<FrameBuffer> frameBuffer);

private:
	
};

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
}