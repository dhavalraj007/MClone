#pragma once
#include<memory>
#include<string>
#include"glad/glad.h"

namespace graphics
{
	//template<typename T=float>
	//class TextureBuffer 
	//{
	//public:
	//	TextureBuffer(const std::vector<T=float>& data,size_t size, GLenum _internalFormat, int _texUnit)
	//		:texUnit(_texUnit)
	//		:internalFormat(_internalFormat)
	//	{
	//		//create tex buffer
	//		glGenBuffers(1, &tboID);
	//		glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	//		glBufferData(GL_TEXTURE_BUFFER,size, &data[0], GL_STATIC_DRAW);
	//		glBindBuffer(GL_TEXTURE_BUFFER, 0);

	//		glGenTextures(1, &texID);
	//	}

	//	void bind()
	//	{
	//		glActiveTexture(GL_TEXTURE0 + texUnit);
	//		glBindTexture(GL_TEXTURE_BUFFER, texID);
	//		glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, tbo);
	//	}
	//	void unbind()
	//	{
	//		glBindTexture(GL_TEXTURE_BUFFER, 0);
	//	}
	//private:
	//	uint32_t tboID;
	//	uint32_t texID;
	//	int texUnit;
	//	GLenum internalFormat;
	//};

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