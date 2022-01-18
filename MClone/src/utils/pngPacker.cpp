#include<filesystem>
#include "utils\pngPacker.h"
#include "log.h"

#include"stb/stb_image.h"
#include"stb/stb_image_write.h"

namespace pngPacker
{
	void packPngs(const char* inputPath, const char* outputPath)
	{

		int numFiles = 0;
		for (auto image : std::filesystem::directory_iterator(inputPath))
		{
			numFiles++;
		}
		MCLONE_INFO("{}", numFiles);


		int outMaxWidth = 32 * (int)sqrt(numFiles);
		int currX = 0, currY = 0;
		int currLineHeight = 0;
		std::vector<Pixel> outPxData(outMaxWidth);	// 1 row maxwidth cols
		for (auto image : std::filesystem::directory_iterator(inputPath))
		{
			int currWidth, currHeight, currNChannels;
			if (image.path().extension().string() != ".png")
				continue;
			uint8_t* currImageData = stbi_load(image.path().string().c_str(), &currWidth, &currHeight, &currNChannels, 4);
			if (!currImageData)
				continue;
			if (currX + currWidth > outMaxWidth)
			{
				currX = 0;
				currY += currLineHeight;
				currLineHeight = 0;
				continue;
			}
			if (currHeight > currLineHeight)
			{
				currLineHeight = currHeight;
				outPxData.resize((currY + currLineHeight) * outMaxWidth);
			}

			for (int y = 0; y < currHeight; y++)
			{
				for (int x = 0; x < currWidth; x++)
				{
					int localX = currX + x;
					int localY = currY + y;
					Pixel& px = outPxData[localY * outMaxWidth + localX];
					uint8_t* inPx = &currImageData[(y * currWidth + x) * 4];
					px.r = inPx[0];
					px.g = inPx[1];
					px.b = inPx[2];
					px.a = inPx[3];
				}
			}

			stbi_image_free(currImageData);

			currX += currWidth;
			if (currX >= outMaxWidth)
			{
				currX = 0;
				currY += currLineHeight;
				currLineHeight = 0;
				outPxData.resize((currY + currLineHeight) * outMaxWidth);
			}
		}

		int outMaxHeight = currY + currLineHeight;

		stbi_write_png(outputPath, outMaxWidth, outMaxHeight, 4, &outPxData[0], outMaxWidth * 4);


	}
}