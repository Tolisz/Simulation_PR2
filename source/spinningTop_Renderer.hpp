#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "camera.hpp"
#include "shader.hpp"
#include "uniformBufferObject.hpp"
#include "obj_cube.hpp"
#include "drawParameters.hpp"
#include "simulationParameters.hpp"
#include "trajectoryBuffer.hpp"


class spinningTop_Renderer
{
public:

	spinningTop_Renderer();
	~spinningTop_Renderer();

	spinningTop_Renderer(const spinningTop_Renderer&) = delete;
	spinningTop_Renderer(spinningTop_Renderer&&) = delete;
	spinningTop_Renderer& operator=(const spinningTop_Renderer&) = delete;
	spinningTop_Renderer& operator=(spinningTop_Renderer&&) = delete;

	void Render(
		std::shared_ptr<const drawParameters> 	drawParams,
		const simulationDrawParameters& 		simResult);
	GLuint GetRenderTexture();

	void UpdateRenderArea(int width, int height);
	void UpdateCameraRotation(float rotX, float rotY);
	void UpdateCameraPosition(float delta);

	std::shared_ptr<trajectoryBuffer> GetTrajectoryBuffer();
	void UpdateGPUTrajectoryBuffer();
	
private:

	void SetUpFramebuffer(); 
	void SetUpScene();

	void PrepareShaders();
private:

	camera 		m_camera;
	glm::uvec2 	m_sceneSize;

	GLuint m_Framebuffer;
	GLuint m_Renderbuffer;
	GLuint m_colorTexture;

	uniformBufferObject m_UBO_Matrices; 

	// Cube
	shader m_shader_cube;
	std::unique_ptr<obj_cube> m_cube;
	glm::mat4 m_cubeInitModelMatrix;	// Model matrix for a cube with edge length 1.

	// Cube's diagonal
	shader m_shader_cubeDiagonal;
	
	// Trajectory
	shader m_shader_traj;
	GLuint m_trajVertexArray;
	GLuint m_trajArrayBuffer;

	std::shared_ptr<trajectoryBuffer> m_trajBuffer;
};