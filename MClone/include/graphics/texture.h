#pragma once

#include<string>
#include"log.h"

namespace graphics
{

	class Texture
	{
	public:
		enum class TextureFiltering
		{
			Nearest,
			Linear
		};
		//the SAME name must be refrenced in shader code to be able to use this texture
		Texture(const std::string& name, const std::string& path, int texUnit);
		~Texture();

		inline std::string getName() const { return m_Name; }
		inline std::string getPath() const { return m_Path; }
		inline uint32_t getId() const { return m_Id; }
		inline uint32_t getWidth() const { return m_Width; }
		inline uint32_t getHeight() const { return m_Height; }
		inline uint32_t getNumOfChannels() const { return m_NumOfChannels; }
		inline TextureFiltering getFilter(bool mag) const { return mag?m_MagFilter:m_MinFilter; }
		inline int getTexUnit() const { return m_TexUnit; }

		void setName(const std::string& name) { m_Name = name; }
		void setTexUnit(int texUnit)
		{
			MCLONE_ASSERT(m_TexUnit > 0, "Texture Unit 0 is not available for User acess. - please use texture units >= 1");
			m_TexUnit = texUnit;
		}
		void setFilter(bool mag,TextureFiltering filter);

		void bind();
		void unbind();
	private:
		void LoadTexture();
	private:
		std::string m_Name;
		std::string m_Path;
		uint32_t m_Id;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_NumOfChannels;
		unsigned char* m_Pixels;
		TextureFiltering m_MagFilter;
		TextureFiltering m_MinFilter;
		int m_TexUnit;
	};
}