#pragma once
#include"glm\glm.hpp"

namespace graphics
{
	class Camera
	{
	public:
		Camera(const glm::vec3& pos,float aspectRatio);

		inline glm::vec3 getPos() const { return m_position; }
		inline glm::vec3 getTarget() const { return m_target; }
		inline glm::mat4 getViewMatrix() const { return m_viewMatrix; }
		inline glm::mat4 getProjMatrix() const { return m_projMatrix; }
		inline float getZNear() const { return m_zNear; }
		inline float getZFar() const { return m_zFar; }
		inline float getFov() const { return m_fov; }
		inline glm::vec3 getDirUp() const { return m_dirUp; }

		inline void setPos(const glm::vec3& pos) { m_position = pos; calculateViewMatrix();  };
		inline void setTarget(const glm::vec3& target) { m_target = target; calculateViewMatrix(); };

		inline void setFov(float fov) { m_fov = fov;  calculateProjMatrix(); }
		inline void setAspectRatio(float ar) { m_aspectRatio = ar; calculateProjMatrix();}
		inline void setZNear(float zNear) { m_zNear = zNear;  calculateProjMatrix();}
		inline void setZFar(float zFar) { m_zFar= zFar;  calculateProjMatrix();}
	protected:
		void calculateViewMatrix();
		void calculateProjMatrix();
	protected:
		glm::vec3 m_position;
		glm::vec3 m_target;
		glm::vec3 m_dirUp;

		float m_aspectRatio;
		float m_fov;
		float m_zNear;
		float m_zFar;

		glm::mat4 m_projMatrix;
		glm::mat4 m_viewMatrix;
	};


	//requires mouse to be centered at a window for the first call to handleInput
	class FlyCamera :public Camera
	{
	public:
		FlyCamera(const glm::vec3& pos, float aspectRatio);
		FlyCamera(const glm::vec3& pos, float aspectRatio,const glm::vec3& orientation,float _speed);
		void handleInput();	//todo: add delta time
		inline void setOrientation(const glm::vec3& orientation);
	public:
		float speed;
	private:
		glm::vec3 m_orientation;//camera front
		float yaw, pitch;
		bool t_firstMouse=true;
	};
}