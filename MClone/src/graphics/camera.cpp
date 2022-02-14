#include "graphics\camera.h"
#include "glm\gtc\matrix_transform.hpp"
#include"input\mouse.h"
#include"input\keyboard.h"
#include"log.h"

namespace graphics
{
	Camera::Camera(const glm::vec3& pos, float aspectRatio)
		:m_position(pos)
		, m_aspectRatio(aspectRatio)
		, m_fov(45.0f)
		, m_zNear(0.1f)
		, m_zFar(100.f)
		, m_target(0.f)
		, m_dirUp(0.0f, 1.f, 0.0f)
	{
		calculateViewMatrix();
		calculateProjMatrix();
	}

	void Camera::calculateViewMatrix()
	{
		m_viewMatrix = glm::lookAt(m_position, m_target, m_dirUp);
	}

	void Camera::calculateProjMatrix()
	{
		m_projMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_zNear, m_zFar);
	}





	FlyCamera::FlyCamera(const glm::vec3& pos, float aspectRatio, const glm::vec3& orientation, float _speed)
		: Camera(pos, aspectRatio)
		, m_orientation(orientation)
		, speed(_speed)
		, yaw(-90.f)
		, pitch(0.0f)
	{
		setOrientation(
			{
				cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
				sin(glm::radians(pitch)),
				cos(glm::radians(pitch)) * sin(glm::radians(yaw))
			});
	}

	FlyCamera::FlyCamera(const glm::vec3& pos, float aspectRatio)
		: FlyCamera(pos, aspectRatio, {0.f, 0.f, -1.f}, 0.0001f)
	{}

	void FlyCamera::handleInput(float dt)
	{
		if (input::Keyboard::Key(input::Keys::MKEY_LSHIFT))
		{
			speed =  dt*fastspeed;
		}
		else
		{
			speed = dt*midspeed;
		}
		bool changed = false;
		if (input::Keyboard::Key(input::Keys::MKEY_W))
		{
			m_position += speed * m_orientation;
			changed = true;
		}
		if (input::Keyboard::Key(input::Keys::MKEY_S))
		{
			m_position -= speed * m_orientation;
			changed = true;
		}
		if (input::Keyboard::Key(input::Keys::MKEY_A))
		{
			m_position -= speed * (glm::normalize(glm::cross(m_orientation, m_dirUp)));
			changed = true;
		}
		if (input::Keyboard::Key(input::Keys::MKEY_D))
		{
			m_position += speed * (glm::normalize(glm::cross(m_orientation, m_dirUp)));
			changed = true;
		}

		
		float xOffset;
		float yOffset;
		if (t_firstMouse)
		{
			xOffset = 0;
			yOffset = 0;
			t_firstMouse = false;
		}
		else
		{
			xOffset = (input::Mouse::x - input::Mouse::xLast) * sensitivity;
			yOffset = (input::Mouse::yLast - input::Mouse::y) * sensitivity;
		}
		
		yaw += (float)xOffset;
		pitch += (float)yOffset;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		setOrientation(
			{
				cos(glm::radians(pitch))*cos(glm::radians(yaw)),
				sin(glm::radians(pitch)),
				cos(glm::radians(pitch))*sin(glm::radians(yaw))
			}
		);

		if (changed)
		{
			Camera::setPos(m_position);
			Camera::setTarget(m_position + m_orientation);
		}
	}
	inline void FlyCamera::setOrientation(const glm::vec3& orientation)
	{
		m_orientation = glm::normalize(orientation); 
		Camera::setTarget(m_position + m_orientation);
	}
}