#pragma once
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include"glm/glm.hpp"
#include"yaml-cpp/yaml.h"
#include"chunk.h"
#include<future>

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

		struct IntPairHashVec3 {
			std::size_t operator()(const glm::ivec3& p) const {
				//Shift first integer over to make room for the second integer. The two are
				//then packed side by side.
				return (((uint64_t)p.x) << 32) | ((uint64_t)p.z);
			}
		};
		struct IntPairHashChunk {
			std::size_t operator()(const std::shared_ptr<game::Chunk>& p) const {
				//Shift first integer over to make room for the second integer. The two are
				//then packed side by side.
				return (((uint64_t)p->position.x) << 32) | ((uint64_t)p->position.z);
			}
		};

		//todo: add ID based textures to eliminate unorderded_map
		std::unordered_map<std::string, int> nameToIdMap;	//for blocks
		std::vector<blockFormat> blockFormats;	
		int chunkRadius = 5;
		glm::ivec3 currChunkAreaCenter;
		std::unordered_set<glm::ivec3, IntPairHashVec3> chunkPosSet;
		std::vector<std::future<void>> f_dataFutures;
		std::unordered_set<std::shared_ptr<Chunk>, IntPairHashChunk> newChunks;	//stores chunks for which data is to be created
	public:
		std::unordered_set<std::shared_ptr<Chunk>, IntPairHashChunk> chunks;
	public:
		ChunkSystem();
		void createVertexDataForChunk(std::shared_ptr<Chunk> chunk);
		void createChunkAreaAround(const glm::vec3& camPos);
		inline int getChunkRadius()const { return chunkRadius; }
		inline void setChunkRadius(int radii) { chunkRadius = radii; }
		void uploadReadyChunksToGpu();
	private:
		void loadFormats();
		void uploadDataToChunkVao(std::shared_ptr<Chunk> chunk);
	};
}