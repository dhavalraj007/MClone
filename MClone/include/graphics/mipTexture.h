#pragma once

#include<string>
#include"log.h"

namespace graphics
{
	class MipTexture
	{
	public:
		enum class TextureFiltering
		{
			Nearest,
			Linear
		};
		//the SAME name must be refrenced in shader code to be able to use this texture
		//paths should be set as increasing mip levels
		MipTexture(const std::string& name, const std::vector<std::string>& paths, int texUnit);
		~MipTexture();

		inline std::string getName() const { return m_Name; }
		inline std::string getPath(int mip) const { return m_Paths[mip]; }
		inline uint32_t getId() const { return m_Id; }
		inline uint32_t getWidth(int mip) const { return m_Widths[mip]; }
		inline uint32_t getHeight(int mip) const { return m_Heights[mip]; }
		inline uint32_t getNumOfChannels(int mip) const { return m_NumOfChannels[mip]; }
		inline TextureFiltering getFilter(bool mag) const { return mag ? m_MagFilter : m_MinFilter; }
		inline int getTexUnit() const { return m_TexUnit; }


		void setMaxMipLevel(int mip) {if(mip<=maxMipLevel) maxMipLevel = mip; }
		void setName(const std::string& name) { m_Name = name; }
		void setTexUnit(int texUnit)
		{
			MCLONE_ASSERT(m_TexUnit > 0, "Texture Unit 0 is not available for User acess. - please use texture units >= 1");
			m_TexUnit = texUnit;
		}
		void setFilter(bool mag, TextureFiltering filter);

		void bind();
		void unbind();
	private:
		void LoadTexture();
	private:
		int maxMipLevel;
		int mipsProvided;
		std::string m_Name;
		std::vector<std::string> m_Paths;
		uint32_t m_Id;
		std::vector<uint32_t> m_Widths;
		std::vector<uint32_t> m_Heights;
		std::vector<uint32_t> m_NumOfChannels;
		std::vector<unsigned char*> m_Pixels;
		TextureFiltering m_MagFilter;
		TextureFiltering m_MinFilter;
		int m_TexUnit;
	};
}