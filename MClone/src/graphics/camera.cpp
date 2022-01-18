#include "graphics\camera.h"
#include "glm\gtc\matrix_transform.hpp"

namespace graphics
{
	Camera::Camera(const glm::vec3& pos,float aspectRatio)
		:m_position(pos)
		,m_aspectRatio(aspectRatio)
		,m_fov(45.0f)
		,m_zNear(0.1f)
		,m_zFar(100.f)
		, m_orientation(0.f)
	{
		calculateViewMatrix();
		calculateProjMatrix();
	}

	void Camera::calculateViewMatrix()
	{
		m_viewMatrix = glm::lookAt(m_position, m_orientation, glm::vec3(0.f, 1.f, 0.f));
	}

	void Camera::calculateProjMatrix()
	{
		m_projMatrix = glm::perspective(glm::radians(m_fov),m_aspectRatio ,m_zNear, m_zFar);
	}
}
