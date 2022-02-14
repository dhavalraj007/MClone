#include "graphics/mipTexture.h"
#include "graphics/helpers.h"


#include"stb/stb_image.h"

#include"stb/stb_image_write.h"

#include"glad/glad.h"


namespace graphics
{
	

	MipTexture::MipTexture(const std::string& name, const std::vector<std::string>& paths, int texUnit)
		:m_Name(name)
		, m_Paths(paths)
		, mipsProvided((int)paths.size())
		, m_Id(0)
		, m_Widths(std::vector<uint32_t>(mipsProvided, 0))
		, m_Heights(std::vector<uint32_t>(mipsProvided, 0))
		, m_NumOfChannels(std::vector<uint32_t>(mipsProvided, 0))
		, m_Pixels(std::vector<unsigned char*>(mipsProvided,nullptr))
		, m_MagFilter(TextureFiltering::Nearest)
		, m_MinFilter(TextureFiltering::Nearest)
		, m_TexUnit(texUnit)
	{
		maxMipLevel = mipsProvided;
		MCLONE_ASSERT(m_TexUnit > 0, "mipTexture Unit 0 is not available for User acess. - please use texture units >= 1");
		int width, height, numOfChannels;
		stbi_set_flip_vertically_on_load(1);
		for (int i = 0; i < mipsProvided; i++)
		{
			m_Pixels[i] = stbi_load(paths[i].c_str(), &width, &height, &numOfChannels, 0);

			if (m_Pixels[i])
			{
				m_Widths[i] = width;
				m_Heights[i] = height;
				m_NumOfChannels[i] = numOfChannels;
			}
			else
			{
				MCLONE_ERROR("Error loading texture from path :{}", paths[i]);
			}
		}
		LoadTexture();
	}

	MipTexture::~MipTexture()
	{
		for (auto& pixels : m_Pixels)
		{
			stbi_image_free(pixels);
			pixels = nullptr;
		}
	}

	void MipTexture::setFilter(bool mag, TextureFiltering filter)
	{
		glActiveTexture(GL_TEXTURE0 + m_TexUnit);
		glBindTexture(GL_TEXTURE_2D, m_Id); MCLONE_CHECK_GL_ERROR;
		if (mag)
		{
			m_MagFilter = filter;
			switch (filter)
			{
			case TextureFiltering::Nearest:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); MCLONE_CHECK_GL_ERROR
					break;
			case TextureFiltering::Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); MCLONE_CHECK_GL_ERROR
					break;
			default:
				break;
			}
		}
		else
		{
			m_MinFilter = filter;
			switch (filter)
			{
			case TextureFiltering::Nearest:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); MCLONE_CHECK_GL_ERROR
					break;
			case TextureFiltering::Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); MCLONE_CHECK_GL_ERROR
					break;
			default:
				break;
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0); MCLONE_CHECK_GL_ERROR;
	}

	void MipTexture::bind()
	{
		glActiveTexture(GL_TEXTURE0 + m_TexUnit);
		glBindTexture(GL_TEXTURE_2D, m_Id); MCLONE_CHECK_GL_ERROR;
	}

	void MipTexture::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void MipTexture::LoadTexture()
	{
		glGenTextures(1, &m_Id); MCLONE_CHECK_GL_ERROR;
		glActiveTexture(GL_TEXTURE0 + m_TexUnit);
		glBindTexture(GL_TEXTURE_2D, m_Id); MCLONE_CHECK_GL_ERROR;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxMipLevel);
		for (int i = 0; i < mipsProvided; i++)
		{
			GLenum channelFormat = 0;
			if (m_NumOfChannels[i] == 3)
			{
				channelFormat = GL_RGB;
			}
			else if (m_NumOfChannels[i] == 4)
			{
				channelFormat = GL_RGBA;
			}
			if (m_Pixels[i] && channelFormat == 0)
			{
				MCLONE_ERROR("Error not supported texture channel :{} ", m_Paths[i]);
			}

			if (m_Pixels[i] && channelFormat != 0)
			{
				glTexImage2D(GL_TEXTURE_2D, i, channelFormat, m_Widths[i], m_Heights[i], 0, channelFormat, GL_UNSIGNED_BYTE, m_Pixels[i]); MCLONE_CHECK_GL_ERROR;

				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
				glGenerateMipmap(GL_TEXTURE_2D);
				setFilter(true, m_MagFilter);
				setFilter(false, m_MinFilter);
				MCLONE_TRACE("Loaded {}-Channel mipTexture :{}", m_NumOfChannels[i], m_Paths[i]);
			}
			else
			{
				float pixels[] = {
					1.0f,0.f,1.0f,		1.0f,1.f,1.0f,		1.0f,0.f,1.0f,		1.0f,1.f,1.0f,
					1.0f,1.0f,1.0f,		1.0f,0.f,1.0f,		1.0f,1.0f,1.0f,		1.0f,0.f,1.0f,
					1.0f,0.f,1.0f,		1.0f,1.f,1.0f,		1.0f,0.f,1.0f,		1.0f,1.f,1.0f,
					1.0f,1.0f,1.0f,		1.0f,0.f,1.0f,		1.0f,1.0f,1.0f,		1.0f,0.f,1.0f,
				};
				m_Widths[i] = 4;
				m_Heights[i] = 4;
				m_NumOfChannels[i] = 3;
				glTexImage2D(GL_TEXTURE_2D, i, GL_RGB, m_Widths[i], m_Heights[i], 0, GL_RGB, GL_FLOAT, pixels); MCLONE_CHECK_GL_ERROR;
				setFilter(true, TextureFiltering::Nearest);
				setFilter(false, TextureFiltering::Nearest);
				MCLONE_WARN("Unable to Load mipTexture : {} - loading checkerboard pattern for mipLevel: {}", m_Paths[i],i);
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}