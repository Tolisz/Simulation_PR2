#include "spinningTop_Renderer.hpp"

#include <assert.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

spinningTop_Renderer::spinningTop_Renderer()
{
	SetUpFramebuffer();
	SetUpScene();
	
	m_trajBuffer = std::make_shared<trajectoryBuffer>();

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
	glEnable(GL_DEPTH_TEST);
	glEnable( GL_BLEND );
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glViewport(0, 0, m_sceneSize.x, m_sceneSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    float aspect = static_cast<float>(m_sceneSize.x)/m_sceneSize.y;
    glm::mat4 viewProj[2] = {m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(aspect)};

	m_UBO_Matrices.BindUBO();
	m_UBO_Matrices.SetBufferData(0, viewProj, 2 * sizeof(glm::mat4));

	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 model =
		glm::mat4_cast(simResult.m_Q) * 
		glm::scale(glm::mat4(1.0f), glm::vec3(simResult.m_cubeEdgeLength)) *
		m_cubeInitModelMatrix;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (drawParams->b_drawCube)
	{
		m_shader_cube.Use();
		m_shader_cube.set4fv("objectColor", drawParams->m_colorCube);

		m_shader_cube.setM4fv("model", false, model);
		m_cube->Draw();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
		SyncGPUTrajectoryBuffer();

		m_shader_traj.Use();
		m_shader_traj.set4fv("trajectoryColor", drawParams->m_colorTrajectory);

		if (!b_trajDrawDifferently)
		{
			glBindVertexArray(m_trajVertexArray);
			glDrawArrays(GL_LINE_STRIP, 0, m_trajDrawSize - 1);
		}
		else 
		{
			glBindVertexArray(m_trajVertexArray);
			glDrawArrays(GL_LINE_STRIP, m_trajGPUPos, m_trajDrawSize - m_trajGPUPos);
			glDrawArrays(GL_LINE_STRIP, 0, m_trajGPUPos);

			// glBindVertexArray(m_LastLineVertexArray);
			// glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		}
		
	}

	if (drawParams->b_drawGravitation)
	{
		// Quad
		m_shader_gravityQuad.Use();
		m_shader_gravityQuad.set4fv("GravitationColor", drawParams->m_colorGravitation);
		
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

void spinningTop_Renderer::ReallocateGPUTrajectoryBuffer()
{
	size_t gpuBuffSize = sizeof(glm::vec3) * m_trajBuffer->Capacity();
	size_t writeSize = sizeof(glm::vec3) * m_trajBuffer->Size();
	m_trajDrawSize =  m_trajBuffer->Size();

	glBindBuffer(GL_ARRAY_BUFFER, m_trajArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, gpuBuffSize, nullptr, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, writeSize, m_trajBuffer->GetDataFrom(0));

	glBindBuffer(GL_ARRAY_BUFFER, m_LastLineElements);
	unsigned int indices[] = {m_trajBuffer->Size() - 2, m_trajBuffer->Size() - 1}; 
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	b_trajGPUbufferAllocated = true;
}

void spinningTop_Renderer::FreeGPUTrajectoryBuffer()
{
	m_trajDrawSize = 0;
	m_trajGPUPos = 0;
	b_trajDrawDifferently = false;

	glBindBuffer(GL_ARRAY_BUFFER, m_trajArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	b_trajGPUbufferAllocated = false;

	m_trajBuffer->Reset();
}

bool spinningTop_Renderer::IsGPUTrajectoryBufferAllocated()
{
	return b_trajGPUbufferAllocated;
}

void spinningTop_Renderer::SyncGPUTrajectoryBuffer()
{
	m_trajBuffer->Lock();

	ReallocateGPUTrajectoryBuffer();

	// int Pos = m_trajBuffer->WritePos();
	// if (Pos > m_trajGPUPos) 
	// {
	// 	size_t writeSize = sizeof(glm::vec3) * (Pos - m_trajGPUPos);
	// 	size_t offset = sizeof(glm::vec3) * m_trajGPUPos;

	// 	glBindBuffer(GL_ARRAY_BUFFER, m_trajArrayBuffer);
	// 	glBufferSubData(GL_ARRAY_BUFFER, offset, writeSize, m_trajBuffer->GetDataFrom(m_trajGPUPos));
	// }
	// else if (Pos < m_trajGPUPos)
	// {
	// 	b_trajDrawDifferently = true;

	// 	size_t writeSize = sizeof(glm::vec3) * (m_trajGPUPos - m_trajBuffer->Size());
	// 	size_t offset = sizeof(glm::vec3) * m_trajGPUPos;

	// 	glBindBuffer(GL_ARRAY_BUFFER, m_trajArrayBuffer);
	// 	glBufferSubData(GL_ARRAY_BUFFER, offset, writeSize, m_trajBuffer->GetDataFrom(m_trajGPUPos));

	// 	writeSize = sizeof(glm::vec3) * Pos;
	// 	offset = 0;
	// 	glBufferSubData(GL_ARRAY_BUFFER, offset, writeSize, m_trajBuffer->GetDataFrom(0));

		
	// 	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LastLineElements);
	// 	// unsigned int indices[] = {0, m_trajBuffer->Size() - 1}; 
	// 	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// }

	// m_trajGPUPos = m_trajBuffer->WritePos();

	// m_trajDrawSize = m_trajBuffer->Size();
	m_trajBuffer->Unlock();
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

	float xRot = -glm::acos(glm::sqrt(3) / 3);
	m_cubeInitModelMatrix =
		glm::rotate(glm::mat4(1.0f), xRot, {1.0f, 0.0f, 0.0f}) * 
		glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), {0.0f, 1.0f, 0.0f}) * 
		glm::translate(glm::mat4(1.0f), {0.5f, 0.5f, 0.5f}) * 
		glm::scale(glm::mat4(1.0f), {0.5f, 0.5f, 0.5});
	
	// DIAGONAL
	glGenVertexArrays(1, &m_diagonalVertexArray);

	// TRAJECTORY
	glGenVertexArrays(1, &m_trajVertexArray);
	glGenVertexArrays(1, &m_LastLineVertexArray);
    glGenBuffers(1, &m_trajArrayBuffer);
	glGenBuffers(1, &m_LastLineElements);

    glBindVertexArray(m_trajVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_trajArrayBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	glBindVertexArray(m_LastLineVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_trajArrayBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LastLineElements);
	glBindVertexArray(0);

	// Gravity
	glGenVertexArrays(1, &m_quadVertexArray);
	glGenVertexArrays(1, &m_forceVertexArray);
}

void spinningTop_Renderer::PrepareShaders()
{
	m_shader_cube.Init();
	m_shader_cube.AttachShader("./shaders/cube.vert", GL_VERTEX_SHADER);
	m_shader_cube.AttachShader("./shaders/cube.frag", GL_FRAGMENT_SHADER);
	m_shader_cube.Link();

	m_shader_cubeDiagonal.Init();
	m_shader_cubeDiagonal.AttachShader("./shaders/diagonal.vert", GL_VERTEX_SHADER);
	m_shader_cubeDiagonal.AttachShader("./shaders/diagonal.frag", GL_FRAGMENT_SHADER);
	m_shader_cubeDiagonal.Link();

	m_shader_traj.Init();
	m_shader_traj.AttachShader("./shaders/trajectory.vert", GL_VERTEX_SHADER);
	m_shader_traj.AttachShader("./shaders/trajectory.frag", GL_FRAGMENT_SHADER);
	m_shader_traj.Link();

	m_shader_gravityQuad.Init();
	m_shader_gravityQuad.AttachShader("./shaders/quad.vert", GL_VERTEX_SHADER);
	m_shader_gravityQuad.AttachShader("./shaders/quad.frag", GL_FRAGMENT_SHADER);
	m_shader_gravityQuad.Link();

	m_shader_gravityForce.Init();
	m_shader_gravityForce.AttachShader("./shaders/force.vert", GL_VERTEX_SHADER);
	m_shader_gravityForce.AttachShader("./shaders/force.frag", GL_FRAGMENT_SHADER);
	m_shader_gravityForce.Link();

	m_UBO_Matrices.CreateUBO(2 * sizeof(glm::mat4));
    m_UBO_Matrices.BindBufferBaseToBindingPoint(0);
}