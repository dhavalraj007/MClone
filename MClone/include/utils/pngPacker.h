#pragma once

namespace pngPacker
{
	struct Pixel
	{
		uint8_t r, g, b, a;
	};
	void packPngs(const char* inputPath, const char* outputPath);
}