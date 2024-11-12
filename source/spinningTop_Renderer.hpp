#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "shader.hpp"
#include "uniformBufferObject.hpp"

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
	void UpdateCameraRotation(float rotX, float rotY);
	void UpdateCameraPosition(float delta);
private:

	void SetUpFramebuffer(); 
	void SetUpScene();

private:

	camera 		m_camera;
	glm::uvec2 	m_sceneSize;

	GLuint m_Framebuffer;
	GLuint m_Renderbuffer;
	GLuint m_colorTexture;

	uniformBufferObject m_UBO_Matrices; 

	shader		m_gridShader;
	GLuint 		m_VAO;
	GLuint 		m_VBO;
};