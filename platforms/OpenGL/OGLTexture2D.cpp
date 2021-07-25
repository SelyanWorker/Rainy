
#include "OGLTexture2D.h"

namespace Rainy {

	GLuint textureBindIndex = 0;

	Texture2D* Texture2D::Create()
	{
		return new OGLTexture2D();
	}

	OGLTexture2D::OGLTexture2D()
	:	m_index(0),
		m_texUnit(0)
	{
		glGenTextures(1, &m_index);
	}

	OGLTexture2D::~OGLTexture2D()
	{
		glDeleteTextures(1, &m_index);
	}

	uint32_t OGLTexture2D::GetNative()
	{
		return m_index;
	}

	uint32_t OGLTexture2D::GetWidth()
	{
		return m_width;
	}

	uint32_t OGLTexture2D::GetHeight()
	{
		return m_height;
	}

	void OGLTexture2D::TextureData(uint32_t width, uint32_t height,
		uint32_t channels, TextureDataType dataType, void* data)
	{
		m_width = width;
		m_height = height;

		Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (dataType == TextureDataType::UNSIGNED_BYTE) m_dataType = GL_UNSIGNED_BYTE;
		else m_dataType = GL_FLOAT;

		m_dataFormat = GL_RGBA;
		if(m_dataType == GL_UNSIGNED_BYTE) m_internalFormat = GL_RGBA8;
		else m_internalFormat = GL_RGBA32F;
		GLint aligment = 4;
		if (channels == 3)
		{
			m_dataFormat = GL_RGB;
			if (m_dataType == GL_UNSIGNED_BYTE) m_internalFormat = GL_RGB8;
			else m_internalFormat = GL_RGB32F;
			aligment = 1;
		}
		else if (channels == 2)
		{
			m_dataFormat = GL_RG;
			if (m_dataType == GL_UNSIGNED_BYTE) m_internalFormat = GL_RG8;
			else m_internalFormat = GL_RG32F;
			aligment = 2;
		}
		else if (channels == 1)
		{
			m_dataFormat = GL_RED;
			if (m_dataType == GL_UNSIGNED_BYTE) m_internalFormat = GL_R8;
			else m_internalFormat = GL_R32F;
			aligment = 1;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, aligment);
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height,
			0, m_dataFormat, m_dataType, data);


		UnBind();
	}

	void OGLTexture2D::TextureData(uint32_t width, uint32_t height, void* data)
	{
		Bind();

		m_width = width;
		m_height = height;

		GLint aligment = 4;
		switch (m_dataFormat)
		{
		case GL_RGB: aligment = 1; break;
		case GL_RG: aligment = 2; break;
		case GL_RED: aligment = 1; break;
		default:
			break;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, aligment);
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height,
			0, m_dataFormat, m_dataType, data);

		UnBind();
	}

	void OGLTexture2D::TextureData(Image* image)
	{
		m_width = image->GetWidth();
		m_height = image->GetHeight();

		glBindTexture(GL_TEXTURE_2D, m_index);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		m_dataType = GL_UNSIGNED_BYTE;
		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;
		GLint aligment = 4;
		if (image->GetChannels() == 3)
		{
			m_internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
			aligment = 1;
		}
		else if (image->GetChannels() == 2)
		{
			m_internalFormat = GL_RG8;
			m_dataFormat = GL_RG;
			aligment = 2;
		}
		else if (image->GetChannels() == 1)
		{
			m_internalFormat = GL_R8;
			m_dataFormat = GL_RED;
			aligment = 1;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, aligment);
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height,
			0, m_dataFormat, m_dataType, image->GetData());

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OGLTexture2D::TextureSubData(uint32_t widthOffset, uint32_t heightOffset,
		uint32_t width, uint32_t height, void* data)
	{
		glBindTexture(GL_TEXTURE_2D, m_index);

		glTexSubImage2D(GL_TEXTURE_2D, 0, GLint(widthOffset), GLint(heightOffset),
			GLint(width), GLint(height), m_dataFormat, m_dataType, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void* OGLTexture2D::GetTextureData()
	{
		Bind();

		size_t channels = 4;
		GLint aligment = 4;
		switch (m_dataFormat)
		{
		case GL_RGB: aligment = 1; channels = 3; break;
		case GL_RG: aligment = 2; channels = 2; break;
		case GL_RED: aligment = 1;  channels = 1; break;
		default: break;
		}

		void* pixels;
		if (m_dataType == GL_FLOAT)
			pixels = new GLfloat[m_width * m_height * channels];
		else pixels = new GLubyte[m_width * m_height * channels];

		glGetTexImage(GL_TEXTURE_2D, 0, m_dataFormat, m_dataType, pixels);

		{
			GLubyte* fpixels = reinterpret_cast<GLubyte*>(pixels);
			for (size_t i = 0; i < m_width * m_height * channels; i++)
			{
				float val = fpixels[i];
				if (val);
			}
		}

		UnBind();

		return pixels;
	}

	void OGLTexture2D::Bind()
	{
		glActiveTexture(GL_TEXTURE0 + m_texUnit);
		glBindTexture(GL_TEXTURE_2D, m_index);
	}

	void OGLTexture2D::UnBind()
	{
		glActiveTexture(GL_TEXTURE0 + m_texUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OGLTexture2D::SetTexUnit(uint32_t unit)
	{
		m_texUnit = unit;
	}

}
