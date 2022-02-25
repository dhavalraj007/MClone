#pragma once
#include<memory>
#include<string>
#include"glad/glad.h"

namespace graphics
{
	class TextureArray
	{
	public:
		TextureArray(const std::string& imagePath, int _width, int _height,int unit);
		void bind();
		void unbind();
		int texUnit;
		uint32_t ID;
	};
}