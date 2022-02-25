#include"log.h"

#include"graphics/helpers.h"
#include"graphics/misc.h"
#include"stb/stb_image.h"

namespace graphics
{
	struct pixel
	{
		uint8_t r, g, b, a;
	};
	TextureArray::TextureArray(const std::string& imagePath, int _width, int _height,int unit)
	{
		texUnit = unit;
		int imageWidth, imageHeight, imageChannels;
		//stbi_set_flip_vertically_on_load(true);
		pixel* ImageData = reinterpret_cast<pixel*>( stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &imageChannels, 4));
		MCLONE_ASSERT(ImageData, "Image not loaded!");
		int numOfImages =int((imageWidth / _width) * (imageHeight/_height));

		glGenTextures(1, &ID); MCLONE_CHECK_GL_ERROR;
		glActiveTexture(GL_TEXTURE0 + texUnit); MCLONE_CHECK_GL_ERROR;
		glBindTexture(GL_TEXTURE_2D_ARRAY, ID); MCLONE_CHECK_GL_ERROR;
		// Allocate the storage.
		glTexImage3D(GL_TEXTURE_2D_ARRAY,
			0,                 // mipmap level
			GL_RGBA8,          // gpu texel format
			_width,             // width
			_height,             // height
			numOfImages,             // depth
			0,                 // border
			GL_RGBA,      // cpu pixel format
			GL_UNSIGNED_BYTE,  // cpu pixel coord type
			ImageData           // pixel data  
		); MCLONE_CHECK_GL_ERROR;

		std::vector<pixel> data(_width*_height);
		int currX = 0;
		int currY = 0;
		for (int img_i = 0; img_i < numOfImages; img_i++)
		{
			for (int y = 0; y < _height; y++)
			{
				for (int x = 0; x < _width; x++)
				{
					pixel& imagePix = ImageData[x + currX + imageWidth * (y + currY)];
					data[x + _width * (_height-y -1)] = imagePix;
				}
			}
			currX += _width;
			if (currX >= imageWidth)
			{
				currX = 0;
				currY += _height;
			}
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
				0, 0, img_i,
				_width, _height, 1,
				GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		}

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY); MCLONE_CHECK_GL_ERROR;
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); MCLONE_CHECK_GL_ERROR;
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); MCLONE_CHECK_GL_ERROR;

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); MCLONE_CHECK_GL_ERROR;
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); MCLONE_CHECK_GL_ERROR;
	}
	void TextureArray::bind()
	{
		glActiveTexture(GL_TEXTURE0 + texUnit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
	}
	void TextureArray::unbind()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}