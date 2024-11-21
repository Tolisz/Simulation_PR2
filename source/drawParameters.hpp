#pragma once

#include <glm/glm.hpp>

struct drawParameters
{
	glm::vec4 m_colorCube			= {0.4	, 1.0	, 0.4	, 0.5};
	glm::vec4 m_colorDiagonal		= {0.953, 0.373	, 0.318	, 1.0};

	glm::vec4 m_colorTrajectory		= {1.0	, 1.0	, 1.0	, 1.0};
	glm::vec4 m_colorGravitation	= {0.0	, 1.0	, 1.0	, 0.3};

	int m_trajectoryPointsNum 	= 200; 

	bool b_drawCube 		= true;
	bool b_drawDiagonal 	= true;
	
	bool b_drawTrajectory 	= true;
	bool b_drawGravitation  = true;	

	bool b_drawTrajectoryAsLine = true;
};