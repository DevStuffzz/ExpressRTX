#pragma once

#include <Walnut/Image.h>
#include <Walnut/Timer.h>
#include <glm/glm.hpp>

#include <memory>

class Renderer {
public: 
	Renderer() = default;

	void Render();
	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	// Built In Pixel/Fragment Shader. Ran once for every pixel on the screen
	uint32_t PerPixel(glm::vec2 coord);
private:

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};