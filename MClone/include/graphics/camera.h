#pragma once
#include"glm\glm.hpp"

namespace graphics
{
	class Camera
	{
	public:
		Camera(const glm::vec3& pos,float aspectRatio);

		inline glm::vec3 getPos() const { return m_position; }
		inline glm::vec3 getOrientation() const { return m_orientation; }
		inline glm::mat4 getViewMatrix() const { return m_viewMatrix; }
		inline glm::mat4 getProjMatrix() const { return m_projMatrix; }
		inline float getZNear() const { return m_zNear; }
		inline float getZFar() const { return m_zFar; }
		inline float getFov() const { return m_fov; }

		inline void setPos(const glm::vec3& pos) { m_position = pos; calculateViewMatrix();  };
		inline void setOrientation(const glm::vec3& orientation) { m_orientation = orientation; calculateViewMatrix(); };

		inline void setFov(float fov) { m_fov = fov;  calculateProjMatrix(); }
		inline void setAspectRatio(float ar) { m_aspectRatio = ar; calculateProjMatrix();}
		inline void setZNear(float zNear) { m_zNear = zNear;  calculateProjMatrix();}
		inline void setZFar(float zFar) { m_zFar= zFar;  calculateProjMatrix();}
		
	private:
		void calculateViewMatrix();
		void calculateProjMatrix();
	private:
		glm::vec3 m_position;
		glm::vec3 m_orientation;

		float m_aspectRatio;
		float m_fov;
		float m_zNear;
		float m_zFar;

		glm::mat4 m_projMatrix;
		glm::mat4 m_viewMatrix;
	};
}