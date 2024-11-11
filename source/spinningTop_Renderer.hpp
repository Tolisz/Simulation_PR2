#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class spinningTop_Renderer
{
public:

	spinningTop_Renderer();
	~spinningTop_Renderer();

	spinningTop_Renderer(const spinningTop_Renderer&) = delete;
	spinningTop_Renderer(spinningTop_Renderer&&) = delete;
	spinningTop_Renderer& operator=(const spinningTop_Renderer&) = delete;
	spinningTop_Renderer& operator=(spinningTop_Renderer&&) = delete;

	void Render();
	GLuint GetRenderTexture();
	
	void UpdateRenderArea(int width, int height);

private:

	void SetUpFramebuffer(); 

private:

	GLuint m_Framebuffer;
	GLuint m_Renderbuffer;
	GLuint m_colorTexture;

	glm::uvec2 m_sceneSize;
};