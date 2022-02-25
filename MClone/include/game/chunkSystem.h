#pragma once
#include<unordered_map>
#include<vector>
#include"glm/glm.hpp"
#include"yaml-cpp/yaml.h"
#include"chunk.h"

namespace game
{
	
	

	struct textureFormat
	{
		uint32_t id;
		std::string name;
		glm::vec2 uvs[4];
	};

	//0 is NULL_BLOCK
	struct blockFormat
	{
		int id;
		std::string name, side, top, bottom;
		int sideID, topID, bottomID;
		bool isTransparent;
		blockFormat()
			:id(0), name("NULL_BLOCK"), side(""), top(""), bottom(""),sideID(0),topID(0),bottomID(0),isTransparent(true) {}
	};

	class ChunkSystem
	{
		//todo: add ID based textures to eliminate unorderded_map
		std::unordered_map<std::string, int> nameToIdMap;	//for blocks
		std::vector<blockFormat> blockFormats;
		std::vector<textureFormat> textureFormats;		
	public:
		std::vector<std::shared_ptr<Chunk>> chunks;
	public:
		ChunkSystem();
		void createDataForChunk(std::shared_ptr<Chunk> chunk);
	private:
		void loadFormats();
		void loadTextureFormat(YAML::Node mainNode, blockFormat& blockFmt);
	};
}