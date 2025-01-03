#include "spinningTop_Renderer.hpp"

#include <assert.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#ifdef EMBEDDED_SHADERS
	#include "shaders/all_shaders.hpp"
#endif

spinningTop_Renderer::spinningTop_Renderer(std::size_t initialTrajLength)
{
	SetUpFramebuffer();
	SetUpScene();
	
	m_trajBuffer = std::make_shared<trajectoryBuffer>(initialTrajLength);

	m_camera.m_worldPos = glm::vec3(0.0f, 0.0f, 6.0f);
	m_camera.UpdateRotation(glm::radians(40.0f), -glm::radians(40.0f));
}

spinningTop_Renderer::~spinningTop_Renderer()
{
	glDeleteFramebuffers(1, &m_Framebuffer);
	glDeleteTextures(1, &m_colorTexture);
	glDeleteRenderbuffers(1, &m_Renderbuffer);
}

void spinningTop_Renderer::Render(
	std::shared_ptr<const drawParameters> 	drawParams, 
	const simulationDrawParameters& 		simResult)
{
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	
	glViewport(0, 0, m_sceneSize.x, m_sceneSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	UpdateUBOs(simResult.m_cubeEdgeLength);

	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 model =
		glm::mat4_cast(simResult.m_Q) * 
		glm::scale(glm::mat4(1.0f), glm::vec3(simResult.m_cubeEdgeLength)) *
		m_cubeInitModelMatrix;

	if (glm::dot(m_camera.m_front, glm::vec3(0.0f, 1.0f, 0.0f)) > 0)
	{
		DrawCube(model, drawParams);	
		DrawGravitation(model, drawParams, simResult);
	}
	else 
	{
		DrawGravitation(model, drawParams, simResult);
		DrawCube(model, drawParams);
	}

	if (drawParams->b_drawDiagonal)
	{
		m_shader_cubeDiagonal.Use();
		m_shader_cubeDiagonal.set4fv("lineColor", drawParams->m_colorDiagonal);
		m_shader_cubeDiagonal.setM4fv("model", false, model);
		glBindVertexArray(m_diagonalVertexArray);
		glDrawArrays(GL_LINES, 0, 2);
	}

	if (drawParams->b_drawTrajectory)
	{
		m_shader_traj.Use();
		m_shader_traj.set4fv("trajectoryColor", drawParams->m_colorTrajectory);

		m_trajBuffer->Draw(drawParams->b_drawTrajectoryAsLine);		
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint spinningTop_Renderer::GetRenderTexture()
{
	return m_colorTexture;
}

void spinningTop_Renderer::UpdateRenderArea(int width, int height)
{
	m_sceneSize.x = width;
	m_sceneSize.y = height;

	glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_sceneSize.x, m_sceneSize.y);

	glBindTexture(GL_TEXTURE_2D, m_colorTexture);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_sceneSize.x, m_sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void spinningTop_Renderer::UpdateCameraRotation(float rotX, float rotY)
{
	m_camera.UpdateRotation(rotX, rotY);
}

void spinningTop_Renderer::UpdateCameraPosition(float delta)
{
	m_camera.UpdatePosition(delta);
}

std::shared_ptr<trajectoryBuffer> spinningTop_Renderer::GetTrajectoryBuffer()
{
	return m_trajBuffer;
}

void spinningTop_Renderer::UpdateUBOs(float a)
{
	// Matrices
    float aspect = static_cast<float>(m_sceneSize.x)/m_sceneSize.y;
    glm::mat4 viewProj[2] = {m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(aspect)};

	m_matricesUBO.BindUBO();
	m_matricesUBO.SetBufferData(0, viewProj, 2 * sizeof(glm::mat4));

	// Lights
	UpdateLightPositions(a);

	m_lightsUBO.BindUBO();
	m_lightsUBO.SetBufferData(0, &m_ambientColor, sizeof(glm::vec4));
    m_lightsUBO.SetBufferData(sizeof(glm::vec4), m_lights.data(), 3 * sizeof(glm::vec4) * m_lights.size());
}

void spinningTop_Renderer::DrawCube(
	const glm::mat4& model, 
	std::shared_ptr<const drawParameters> 	drawParams)
{
	glEnable(GL_CULL_FACE);
	if (drawParams->b_drawCube)
	{
		m_shader_cube.Use();
		m_shader_cube.set4fv("cubeColor", drawParams->m_colorCube);

		const material& mat = m_materials["cube"]; 
		m_shader_cube.set3fv("material.ka", mat.ka);
		m_shader_cube.set3fv("material.kd", mat.kd);
		m_shader_cube.set3fv("material.ks", mat.ks);
		m_shader_cube.set1f("material.shininess", mat.shininess);

		m_shader_cube.set3fv("cameraPos", m_camera.m_worldPos);

		m_shader_cube.setM4fv("model", false, model);
		m_cube->Draw();
	}
	glDisable(GL_CULL_FACE);
}

void spinningTop_Renderer::DrawGravitation(
	const glm::mat4& model, 
	std::shared_ptr<const drawParameters> 	drawParams,
	const simulationDrawParameters& 		simResult)
{
	if (drawParams->b_drawGravitation)
	{
		// Quad
		m_shader_gravityQuad.Use();
		m_shader_gravityQuad.set4fv("GravitationColor", drawParams->m_colorGravitation);
		
		const material& mat = m_materials["quad"]; 
		m_shader_gravityQuad.set3fv("material.ka", mat.ka);
		m_shader_gravityQuad.set3fv("material.kd", mat.kd);
		m_shader_gravityQuad.set3fv("material.ks", mat.ks);
		m_shader_gravityQuad.set1f("material.shininess", mat.shininess);

		m_shader_gravityQuad.set3fv("cameraPos", m_camera.m_worldPos);

		glBindVertexArray(m_quadVertexArray);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Gravitation Vector
		if (simResult.b_forceApplied) 
		{
			m_shader_gravityForce.Use();
			m_shader_gravityForce.setM4fv("model", false, model);
			m_shader_gravityForce.set4fv("GravitationColor", drawParams->m_colorGravitation);
			m_shader_gravityForce.set1f("edgeLength", simResult.m_cubeEdgeLength);
			glBindVertexArray(m_forceVertexArray);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}	
}

void spinningTop_Renderer::SetUpFramebuffer()
{
	glGenFramebuffers(1, &m_Framebuffer);
	glGenTextures(1, &m_colorTexture);
	glGenRenderbuffers(1, &m_Renderbuffer);
	
	UpdateRenderArea(1, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);

	// DEBUG ONLY !!!!!!
	assert(("Framebuffer is not complete", glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void spinningTop_Renderer::SetUpScene()
{
	PrepareShaders();

	// CUBE
	m_cube = std::make_unique<obj_cube>();

	float xRot = -glm::acos(glm::sqrt(3.0f) / 3.0f);
	m_cubeInitModelMatrix =
		glm::rotate(glm::mat4(1.0f), xRot, {1.0f, 0.0f, 0.0f}) * 
		glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), {0.0f, 1.0f, 0.0f}) * 
		glm::translate(glm::mat4(1.0f), {0.5f, 0.5f, 0.5f}) * 
		glm::scale(glm::mat4(1.0f), {0.5f, 0.5f, 0.5});
	
	// DIAGONAL
	glGenVertexArrays(1, &m_diagonalVertexArray);

	// Gravity
	glGenVertexArrays(1, &m_quadVertexArray);
	glGenVertexArrays(1, &m_forceVertexArray);

	// Light and material
	light l;
    l.m_diffuseColor = glm::vec4(1.0f);
    l.m_specularColor = glm::vec4(1.0f);
	m_lights.push_back(l);
	
	l.m_diffuseColor = glm::vec4(0.7f);
    l.m_specularColor = glm::vec4(0.7f);
	for (int i = 0; i < 3; i++)
	{
		m_lights.push_back(l);
	}

	// Shader Update
	m_shader_gravityQuad.Use();
	m_shader_gravityQuad.set1i("numberOfLights", static_cast<GLint>(m_lights.size()));

	m_shader_cube.Use();
	m_shader_cube.set1i("numberOfLights", static_cast<GLint>(m_lights.size()));

	material m; 
	m.ka = glm::vec3(0.2f);
    m.kd = glm::vec3(1.0f);
    m.ks = glm::vec3(0.5f);
    m.shininess = 256.0f;
	m_materials.insert(std::make_pair("quad", m));

	m.ka = glm::vec3(0.2f);
    m.kd = glm::vec3(1.0f);
    m.ks = glm::vec3(0.0f);
    m.shininess = 128.0f;
	m_materials.insert(std::make_pair("cube", m));
}

void spinningTop_Renderer::PrepareShaders()
{
#ifndef EMBEDDED_SHADERS
	m_shader_cube.Init();
	m_shader_cube.AttachShader("./shaders/cube.vert", GL_VERTEX_SHADER, false);
	m_shader_cube.AttachShader("./shaders/cube.frag", GL_FRAGMENT_SHADER, false);
	m_shader_cube.Link();

	m_shader_cubeDiagonal.Init();
	m_shader_cubeDiagonal.AttachShader("./shaders/diagonal.vert", GL_VERTEX_SHADER, false);
	m_shader_cubeDiagonal.AttachShader("./shaders/diagonal.frag", GL_FRAGMENT_SHADER, false);
	m_shader_cubeDiagonal.Link();

	m_shader_traj.Init();
	m_shader_traj.AttachShader("./shaders/trajectory.vert", GL_VERTEX_SHADER, false);
	m_shader_traj.AttachShader("./shaders/trajectory.frag", GL_FRAGMENT_SHADER, false);
	m_shader_traj.Link();

	m_shader_gravityQuad.Init();
	m_shader_gravityQuad.AttachShader("./shaders/quad.vert", GL_VERTEX_SHADER, false);
	m_shader_gravityQuad.AttachShader("./shaders/quad.frag", GL_FRAGMENT_SHADER, false);
	m_shader_gravityQuad.Link();

	m_shader_gravityForce.Init();
	m_shader_gravityForce.AttachShader("./shaders/force.vert", GL_VERTEX_SHADER, false);
	m_shader_gravityForce.AttachShader("./shaders/force.frag", GL_FRAGMENT_SHADER, false);
	m_shader_gravityForce.Link();
#else 
	m_shader_cube.Init();
	m_shader_cube.AttachShader(shader_cube_vert, GL_VERTEX_SHADER, true);
	m_shader_cube.AttachShader(shader_cube_frag, GL_FRAGMENT_SHADER, true);
	m_shader_cube.Link();

	m_shader_cubeDiagonal.Init();
	m_shader_cubeDiagonal.AttachShader(shader_diagonal_vert, GL_VERTEX_SHADER, true);
	m_shader_cubeDiagonal.AttachShader(shader_diagonal_frag, GL_FRAGMENT_SHADER, true);
	m_shader_cubeDiagonal.Link();

	m_shader_traj.Init();
	m_shader_traj.AttachShader(shader_trajectory_vert, GL_VERTEX_SHADER, true);
	m_shader_traj.AttachShader(shader_trajectory_frag, GL_FRAGMENT_SHADER, true);
	m_shader_traj.Link();

	m_shader_gravityQuad.Init();
	m_shader_gravityQuad.AttachShader(shader_quad_vert, GL_VERTEX_SHADER, true);
	m_shader_gravityQuad.AttachShader(shader_quad_frag, GL_FRAGMENT_SHADER, true);
	m_shader_gravityQuad.Link();

	m_shader_gravityForce.Init();
	m_shader_gravityForce.AttachShader(shader_force_vert, GL_VERTEX_SHADER, true);
	m_shader_gravityForce.AttachShader(shader_force_frag, GL_FRAGMENT_SHADER, true);
	m_shader_gravityForce.Link();	
#endif

	// Matrices
	m_matricesUBO.CreateUBO(2 * sizeof(glm::mat4));
    m_matricesUBO.BindBufferBaseToBindingPoint(0);

	// Lights
	m_lightsUBO.CreateUBO((1 + 3 * c_maxLights) * sizeof(glm::vec4));
	m_lightsUBO.BindBufferBaseToBindingPoint(1);
}

void spinningTop_Renderer::UpdateLightPositions(float a)
{
	float d = glm::sqrt(3.0f) * a;

	m_lights[0].m_position = glm::vec4(0.0f, + d * 1.5f, 0.0f, 1.0f);
	m_lights[1].m_position = glm::vec4(
		d * glm::cos(glm::radians(0.0f)), 
		- d * 1.5f, 
		d * glm::sin(glm::radians(0.0f)), 1.0f);
	m_lights[2].m_position = glm::vec4(
		d * glm::cos(glm::radians(120.0f)), 
		- d * 1.5f, 
		d * glm::sin(glm::radians(120.0f)), 1.0f);
	m_lights[3].m_position = glm::vec4(
		d * glm::cos(glm::radians(240.0f)), 
		- d * 1.5f, 
		d * glm::sin(glm::radians(240.0f)), 1.0f);
}