#include "spinningTop_Renderer.hpp"

#include <assert.h>
#include <glm/gtc/matrix_transform.hpp>

spinningTop_Renderer::spinningTop_Renderer()
{
	SetUpFramebuffer();
	SetUpScene();

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glViewport(0, 0, m_sceneSize.x, m_sceneSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    float aspect = static_cast<float>(m_sceneSize.x)/m_sceneSize.y;
    glm::mat4 viewProj[2] = {m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(aspect)};

	m_UBO_Matrices.BindUBO();
	m_UBO_Matrices.SetBufferData(0, viewProj, 2 * sizeof(glm::mat4));

	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawParams->b_drawCube)
	{
		m_shader_cube.Use();
		m_shader_cube.set4fv("objectColor", drawParams->m_colorCube);

		glm::mat4 model =
			glm::mat4_cast(simResult.m_Q) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(simResult.m_cubeEdgeLength)) *
			m_cubeInitModelMatrix;
		
		m_shader_cube.setM4fv("model", false, model);
		m_cube->Draw();
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

	m_cube = std::make_unique<obj_cube>();

	float xRot = -glm::acos(glm::sqrt(3) / 3);
	m_cubeInitModelMatrix =
		glm::rotate(glm::mat4(1.0f), xRot, {1.0f, 0.0f, 0.0f}) * 
		glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), {0.0f, 1.0f, 0.0f}) * 
		glm::translate(glm::mat4(1.0f), {0.5f, 0.5f, 0.5f}) * 
		glm::scale(glm::mat4(1.0f), {0.5f, 0.5f, 0.5});
}

void spinningTop_Renderer::PrepareShaders()
{
	m_shader_cube.Init();
	m_shader_cube.AttachShader("./shaders/cube.vert", GL_VERTEX_SHADER);
	m_shader_cube.AttachShader("./shaders/cube.frag", GL_FRAGMENT_SHADER);
	m_shader_cube.Link();

	m_UBO_Matrices.CreateUBO(2 * sizeof(glm::mat4));
    m_UBO_Matrices.BindBufferBaseToBindingPoint(0);
}