#pragma once
#include<string>
#include<unordered_map>
#include"glm\glm.hpp"

namespace graphics
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSourcePath, const std::string& fragmentSourcePath);
		~Shader();

		void bind();
		void unbind();

		void setUniformInt(const std::string& name, int val);
		void setUniformInt2(const std::string& name, int val1, int val2);
		void setUniformInt2(const std::string& name, const glm::ivec2& val);
		void setUniformInt3(const std::string& name, int val1, int val2, int val3);
		void setUniformInt3(const std::string& name, const glm::ivec3& val);
		void setUniformInt4(const std::string& name, int val1, int val2, int val3, int val4);
		void setUniformInt4(const std::string& name, const glm::ivec4& val);
		/////////////////////////////////////////
		void setUniformFloat(const std::string& name, float val);
		void setUniformFloat2(const std::string& name, float val1, float val2);
		void setUniformFloat2(const std::string& name, const glm::vec2& val);
		void setUniformFloat3(const std::string& name, float val1, float val2, float val3);
		void setUniformFloat3(const std::string& name, const glm::vec3& val);
		void setUniformFloat4(const std::string& name, float val1, float val2, float val3, float val4);
		void setUniformFloat4(const std::string& name, const glm::vec4& val);
		void setUniformMat3(const std::string& name, const glm::mat3& mat);
		void setUniformMat4(const std::string& name, const glm::mat4& mat);
	private:
		int getUniformLocation(const std::string& name);
	private:
		uint32_t m_ProgramId;
		std::unordered_map<std::string, int> m_uniformLocations;
	};
}