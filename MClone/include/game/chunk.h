#pragma once
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"yaml-cpp/yaml.h"
#include"perlinNoise/perlinNoise.hpp"

#include<iostream>
namespace game
{
	//typedef int(*IDEmitterFunc)(int, int, int);
	// to do: make IDemitter BlockEmitter
	typedef std::function<int(int, int, int)> IDEmitterFunc;

	struct internalBlock
	{
		int16_t id;
		int8_t lightLevel;
		int8_t rotation;
		int32_t padding;
	};
	
	
	static IDEmitterFunc defaultIDEmitter = [](int x, int y, int z) {
		return 1;	// grassBlock
	};



	class Chunk
	{
	public:
		glm::vec3 position;
		std::shared_ptr<graphics::VertexArray> vao;
		const int CHUNK_WIDTH = 16;
		const int CHUNK_HEIGHT = 384;
		const int CHUNK_BREADTH = 16;
		std::vector<internalBlock> internalBlocks;
	public:
		Chunk(const glm::vec3 pos);
		void createData();

		const internalBlock& getBlockAt(int x, int y, int z);
		inline int to1DArray(int x, int y, int z)
		{
			if (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_BREADTH || z < 0)
				return -1;
			return (x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT));
		}
		inline bool isBlockNull(int x, int y, int z)
		{
			int ret = to1DArray(x, y, z);
			return ret == -1 ?		//is it out of bounds if yes then it is null 
				true
				: (internalBlocks[ret].id == 0); //else if the id is 0 then its null else it is not
		}
	private:
		int IDEmitter(int x, int y, int z);
	private:
		bool isBlocksInit=false;
		//perlin
		const siv::PerlinNoise perlin{678};

	};

	
}