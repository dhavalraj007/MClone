#pragma once

#include<vector>
#include"log.h"

#include<type_traits>

#define MCLONE_CREATE_VERTEX_BUFFER(name,type) std::unique_ptr<graphics::VertexBuffer<type>> name = std::make_unique<graphics::VertexBuffer<type>>();

namespace graphics
{
	class RawVertexBuffer
	{
	public:
		static const uint32_t GLtypeByte;
		static const uint32_t GLtypeUByte;
		static const uint32_t GLtypeShort;
		static const uint32_t GLtypeUShort;
		static const uint32_t GLtypeInt;
		static const uint32_t GLtypeUInt;
		static const uint32_t GLtypeFloat;
		static const uint32_t GLtypeDouble;
	public:
		RawVertexBuffer();
		virtual ~RawVertexBuffer();

		virtual uint32_t getTypeSize() = 0;

		inline bool IsUploaded() { return m_IsUploaded; }
		inline uint32_t getId() const { return m_Vbo; }
		inline uint32_t getVertexCount() const { return m_VertexCount; }
		inline uint32_t getStride()const { return m_Stride; }
		inline uint32_t getGLtype()const { return GLtype; }
		inline std::vector<uint32_t> getLayout() const { return m_Layout; }

		void setLayout(const std::vector<uint32_t>& layout);

		virtual void upload(bool dynamic);

		void bind();
		void unbind();

	protected:
		bool m_IsUploaded = false;
		bool m_IsLayoutSet = false;
		uint32_t m_Vbo = 0;
		uint32_t m_VertexCount = 0;
		uint32_t m_Stride = 0;
		std::vector<uint32_t> m_Layout;
		void* m_Data = nullptr;
		uint32_t m_Size = 0;
		uint32_t GLtype = 0;
	};

	template<typename T>
	class VertexBuffer :public RawVertexBuffer
	{
		static_assert(
			std::is_same<T, char>() ||
			std::is_same<T, unsigned char>() ||
			std::is_same<T, short>() ||
			std::is_same<T, unsigned short>() ||
			std::is_same<T, int>() ||
			std::is_same<T, unsigned int>() ||
			std::is_same<T, float>() ||
			std::is_same<T, double>()
			, "This type is not supported!"
			);
	public:
		VertexBuffer()
			:m_ValueCount(0)
		{
			if constexpr (std::is_same<T, char>()) { GLtype = GLtypeByte; }
			if constexpr (std::is_same<T, unsigned char>()) { GLtype = GLtypeUByte; }
			if constexpr (std::is_same<T, short>()) { GLtype = GLtypeShort; }
			if constexpr (std::is_same<T, unsigned short>()) { GLtype = GLtypeUShort; }
			if constexpr (std::is_same<T, int>()) { GLtype = GLtypeInt; }
			if constexpr (std::is_same<T, unsigned int>()) { GLtype = GLtypeUInt; }
			if constexpr (std::is_same<T, float>()) { GLtype = GLtypeFloat; }
			if constexpr (std::is_same<T, double>()) { GLtype = GLtypeDouble; }
		}
		~VertexBuffer() {}

		void pushVertices(const std::vector<T>& verts)
		{
			MCLONE_ASSERT(verts.size() > 0, "No value passed in for vertex");
			MCLONE_ASSERT(m_IsLayoutSet, "Layout for vertexBuffer not set.");
			MCLONE_ASSERT(verts.size() % m_Stride == 0, "Vertex data is inconsistent with layout!");
			m_VertexCount = (uint32_t)verts.size() / m_Stride;
			m_DataVec.insert(m_DataVec.end(), verts.begin(), verts.end());
		}

		void pushVertex(const std::vector<T>& vec)
		{
			MCLONE_ASSERT(vec.size() > 0, "No value passed in for vertex");
			if (m_DataVec.size() == 0)
			{
				m_ValueCount = (uint32_t)vec.size();
			}
			MCLONE_ASSERT(vec.size() == m_ValueCount, "Attempting to add a Vertex with unexpexted number of values.");
			if (vec.size() == m_ValueCount)
			{
				m_VertexCount++;
				m_DataVec.insert(m_DataVec.end(), vec.begin(), vec.end());
			}
		}

		// must setLayout before this call or this wont work
		void upload(bool dynamic) override
		{
			MCLONE_ASSERT(m_IsLayoutSet, "Layout for vertexBuffer not set.");
			m_Stride *= sizeof(T);
			m_Size = sizeof(T) * (uint32_t)m_DataVec.size();
			MCLONE_TRACE("m_Strid:{} , m_Size:{}", m_Stride, m_Size);
			MCLONE_ASSERT(m_Size > 0, "VertexBuffer::Upload() calling with m_Size = 0");
			m_Data = &m_DataVec[0];
			RawVertexBuffer::upload(dynamic);
		}

		uint32_t getTypeSize() override { return sizeof(T); }

	private:
		std::vector<T> m_DataVec;
		uint32_t m_ValueCount;
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		inline bool isValid() const { return m_IsValid; }
		inline uint32_t getVertexCount() { return m_VertexCount; }
		inline uint32_t getElementCount() { return m_ElementCount; }

		void pushBuffer(std::unique_ptr<RawVertexBuffer> vbo);
		void setElements(const std::vector<uint32_t>& elements);

		void upload();

		void bind();
		void unbind();

	private:
		bool m_IsValid;
		uint32_t m_VertexCount, m_ElementCount;
		uint32_t m_Vao, m_Ebo;
		uint32_t m_AttributeCount;
		std::vector<std::unique_ptr<RawVertexBuffer>> m_Vbos;
	};
}