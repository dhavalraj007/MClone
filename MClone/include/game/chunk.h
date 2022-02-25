#pragma once
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"yaml-cpp/yaml.h"

#include<iostream>
namespace game
{
	struct internalBlock
	{
		int16_t id;
		int8_t lightLevel;
		int8_t rotation;
		int32_t padding;
		internalBlock():id(0),lightLevel(0),rotation(0),padding(0){}
		internalBlock(std::istream& is)
		{
			is.read((char*)this, sizeof(internalBlock));
			//is >> id >> lightLevel >> rotation >> padding;
		}
		/*friend std::istream& operator>>(std::istream& input, internalBlock& rhs)
		{
			input >> rhs.id >> rhs.lightLevel >> rhs.rotation >> rhs.padding;
			return input;
		}*/
	};



	class Chunk
	{
	public:
		glm::ivec3 position;
		std::shared_ptr<graphics::VertexArray> vao;
		static const int CHUNK_WIDTH = 16;	//x
		static const int CHUNK_HEIGHT = 255;	//y (dont fukin increase this shit than 255)
		static const int CHUNK_BREADTH = 16; //z
		std::vector<internalBlock> internalBlocks;
	public:
		Chunk(const glm::ivec3 pos);
		void createData(uint32_t seed);
		//worldPath is the path where every bin file for every pos of chunk exists
		void serialize(const std::string& worldPath);
		void deserialize(const std::string& worldPath);

		const internalBlock& getBlockAt(int x, int y, int z);
		inline int to1DArray(int x, int y, int z)
		{
			if(isOutOfBounds(x,y,z))
				return -1;
			return (x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT));
		}
		inline bool isOutOfBounds(int x, int y, int z)
		{
			return (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_BREADTH || z < 0);
		}
		inline bool isBlockNull(int x, int y, int z)
		{
			int ret = to1DArray(x, y, z);
			return ret == -1 ?		//is it out of bounds if yes then it is null 
				true
				: (internalBlocks[ret].id == 0); //else if the id is 0 then its null else it is not
		}

	private:
	private:
		bool isBlocksInit=false;
	};

	
}