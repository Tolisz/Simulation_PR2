#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <map>
#include "camera.hpp"
#include "shader.hpp"
#include "uniformBufferObject.hpp"
#include "obj_cube.hpp"
#include "drawParameters.hpp"
#include "simulationParameters.hpp"
#include "trajectoryBuffer.hpp"
#include "light.hpp"
#include "material.hpp"

class spinningTop_Renderer
{
public:

	spinningTop_Renderer(std::size_t initialTrajLength);
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

private:

	void UpdateUBOs(float a);

	void DrawCube(
		const glm::mat4& model, 
		std::shared_ptr<const drawParameters> 	drawParams,
		const simulationDrawParameters& 		simResult);

	void DrawGravitation(
		const glm::mat4& model, 
		std::shared_ptr<const drawParameters> 	drawParams,
		const simulationDrawParameters& 		simResult);

	void SetUpFramebuffer(); 
	void SetUpScene();

	void PrepareShaders();

	void UpdateLightPositions(float a);

private:

	camera 		m_camera;
	glm::uvec2 	m_sceneSize;

	GLuint m_Framebuffer;
	GLuint m_Renderbuffer;
	GLuint m_colorTexture;

	uniformBufferObject m_matricesUBO; 

	const int c_maxLights = 5;
	uniformBufferObject m_lightsUBO;

	glm::vec4 m_ambientColor = glm::vec4(1.0f);
	std::vector<light> m_lights;

	std::map<std::string, material> m_materials;

	// Cube
	shader m_shader_cube;
	std::unique_ptr<obj_cube> m_cube;
	glm::mat4 m_cubeInitModelMatrix;	// Model matrix for a cube with edge length 1.

	// Cube's diagonal
	shader m_shader_cubeDiagonal;
	GLuint m_diagonalVertexArray;

	// Trajectory
	shader m_shader_traj;
	std::shared_ptr<trajectoryBuffer> m_trajBuffer;

	// Gravity
	shader m_shader_gravityQuad;
	GLuint m_quadVertexArray;

	shader m_shader_gravityForce;
	GLuint m_forceVertexArray;
};