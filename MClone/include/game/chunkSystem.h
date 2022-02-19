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
		std::string name;
		glm::vec2 uvs[4];
	};

	//0 is NULL_BLOCK
	struct blockFormat
	{
		int id;
		std::string name, side, top, bottom;
		bool isTransparent;
		blockFormat()
			:id(0), name("NULL_BLOCK"), side(""), top(""), bottom(""), isTransparent(true) {}
	};

	class ChunkSystem
	{
		//todo: add ID based textures to eliminate unorderded_map
		std::unordered_map<std::string, int> nameToIdMap;
		std::vector<blockFormat> blockFormats;
		std::unordered_map<std::string, textureFormat> textureFormats;		
	public:
		std::vector<Chunk> chunks;
	public:
		ChunkSystem();
		void addChunk(int posX, int posZ);
		void render();
	private:
		void loadFormats();
		void loadTextureFormat(YAML::Node mainNode, std::string name);
	};
}