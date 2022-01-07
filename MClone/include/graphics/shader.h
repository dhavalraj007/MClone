#pragma once
#include<string>
#include<unordered_map>


namespace graphics
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		~Shader();

		void bind();
		void unbind();

		void setUniformInt(const std::string& name, int val);
		void setUniformFloat(const std::string& name, float val);
		void setUniformFloat2(const std::string& name, float val1, float val2);
		void setUniformFloat3(const std::string& name, float val1, float val2, float val3);
		void setUniformFloat4(const std::string& name, float val1, float val2, float val3, float val4);
	private:
		int getUniformLocation(const std::string& name);
	private:
		uint32_t m_ProgramId;
		std::unordered_map<std::string, int> m_uniformLocations;
	};
}