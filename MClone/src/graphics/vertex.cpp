#include "graphics/vertex.h"
#include "graphics/helpers.h"
#include"glad/glad.h"

namespace graphics
{

	const uint32_t RawVertexBuffer::GLtypeByte = GL_BYTE;
	const uint32_t RawVertexBuffer::GLtypeUByte = GL_UNSIGNED_BYTE;
	const uint32_t RawVertexBuffer::GLtypeShort = GL_SHORT;
	const uint32_t RawVertexBuffer::GLtypeUShort = GL_UNSIGNED_SHORT;
	const uint32_t RawVertexBuffer::GLtypeInt = GL_INT;
	const uint32_t RawVertexBuffer::GLtypeUInt = GL_UNSIGNED_INT;
	const uint32_t RawVertexBuffer::GLtypeFloat = GL_FLOAT;
	const uint32_t RawVertexBuffer::GLtypeDouble = GL_DOUBLE;

	RawVertexBuffer::RawVertexBuffer()
	{
		glGenBuffers(1, &m_Vbo);	MCLONE_CHECK_GL_ERROR;
	}

	RawVertexBuffer::~RawVertexBuffer()
	{
		glDeleteBuffers(1, &m_Vbo);	//MCLONE_CHECK_GL_ERROR;
	}

	void RawVertexBuffer::setLayout(const std::vector<uint32_t>& layout)
	{
		m_Layout = layout;
		for (auto& count : m_Layout)
		{
			m_Stride += count;
		}
		m_IsLayoutSet = true;
	}

	void RawVertexBuffer::upload(bool dynamic)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);	MCLONE_CHECK_GL_ERROR;
		glBufferData(GL_ARRAY_BUFFER, m_Size, m_Data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	MCLONE_CHECK_GL_ERROR;
		glBindBuffer(GL_ARRAY_BUFFER, 0);	MCLONE_CHECK_GL_ERROR;
		m_IsUploaded = true;
	}

	void RawVertexBuffer::bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);	MCLONE_CHECK_GL_ERROR;
	}

	void RawVertexBuffer::unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);	MCLONE_CHECK_GL_ERROR;
	}

	VertexArray::VertexArray()
		:m_Vao(0)
		, m_Ebo(0)
		, m_VertexCount(0)
		, m_ElementCount(0)
		, m_AttributeCount(0)
		, m_IsValid(false)
	{
		glGenVertexArrays(1, &m_Vao);	MCLONE_CHECK_GL_ERROR;
	}


	VertexArray::~VertexArray()
	{
		m_Vbos.clear();
		glDeleteVertexArrays(1, &m_Vao);
	}

	void VertexArray::pushBuffer(std::unique_ptr<RawVertexBuffer> vbo)
	{
		if (m_Vbos.size() > 0)
		{
			MCLONE_ASSERT(m_Vbos[0]->getVertexCount() == vbo->getVertexCount(), "Attempting to add a vertexbuffer of different size!");
		}
		MCLONE_ASSERT(vbo->getLayout().size() > 0, "Layout not set on the vbo");
		if (vbo->getLayout().size() > 0)
		{
			m_Vbos.push_back(std::move(vbo));
			m_VertexCount = m_Vbos[0]->getVertexCount();
		}
	}

	void VertexArray::setElements(const std::vector<uint32_t>& elements)
	{
		m_ElementCount = (uint32_t)elements.size();
		glBindVertexArray(m_Vao);	MCLONE_CHECK_GL_ERROR;
		glGenBuffers(1, &m_Ebo);	MCLONE_CHECK_GL_ERROR;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);	MCLONE_CHECK_GL_ERROR;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(uint32_t), &elements[0], GL_STATIC_DRAW);	MCLONE_CHECK_GL_ERROR;
		glBindVertexArray(0);	MCLONE_CHECK_GL_ERROR;
	}

	void VertexArray::upload()
	{
		bind();
		uint32_t attributeCount = 0;
		for (auto& vbo : m_Vbos)
		{
			if (!vbo->IsUploaded())
			{
				vbo->upload(false);
			}
			vbo->bind();
			uint32_t offset = 0;
			for (auto& count : vbo->getLayout())
			{
				glEnableVertexAttribArray(attributeCount);
				glVertexAttribPointer(attributeCount, count, static_cast<GLenum>(vbo->getGLtype()), GL_FALSE, vbo->getStride(), (void*)(intptr_t)offset);
				attributeCount++;
				offset += count * vbo->getTypeSize();
			}
			vbo->unbind();
		}
		unbind();
		m_IsValid = true;
	}

	void VertexArray::bind()
	{
		glBindVertexArray(m_Vao);	MCLONE_CHECK_GL_ERROR;
	}

	void VertexArray::unbind()
	{
		glBindVertexArray(0);	MCLONE_CHECK_GL_ERROR;
	}

}