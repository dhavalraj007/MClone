#include<filesystem>
#include<fstream>
#include "utils/pngPacker.h"
#include "log.h"
#include"Yaml-cpp/yaml.h"
#include"stb/stb_image.h"
#include"stb/stb_image_write.h"

namespace pngPacker
{
	struct texLocation
	{
		int currX, currY, width, height;
		std::string name;
	};

	void packPngs(const char* inputPath, const char* outputPath)
	{
		std::vector<texLocation> texLocations;
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

			//save uvs to later save them in yaml file
			texLocations.push_back({ currX,currY,currWidth,currHeight,image.path().stem().string() });

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

		YAML::Node texFormat;
		texFormat["BLOCKS"];
		for (auto location : texLocations)
		{
			YAML::Node uvs;
			uvs["uvs"]["0"].push_back((float)location.currX / outMaxWidth);
			uvs["uvs"]["0"].push_back((float)location.currY / outMaxHeight);

			uvs["uvs"]["1"].push_back((float)(location.currX + location.width) / outMaxWidth);
			uvs["uvs"]["1"].push_back((float)location.currY / outMaxHeight);

			uvs["uvs"]["2"].push_back((float)(location.currX + location.width) / outMaxWidth);
			uvs["uvs"]["2"].push_back((float)(location.currY + location.height) / outMaxHeight);
									  
			uvs["uvs"]["3"].push_back((float)location.currX / outMaxWidth);
			uvs["uvs"]["3"].push_back((float)(location.currY + location.height) / outMaxHeight);

			texFormat["BLOCKS"][location.name] = uvs;
		}

		std::ofstream fout("textureFormat.yaml");
		fout << texFormat;
		fout.close();
	}
}