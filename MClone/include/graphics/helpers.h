#pragma once
#include"log.h"
#include"glad/glad.h"

namespace graphics
{
	void checkGLError();
}

#ifndef MCLONE_CONFIG_RELEASE
#define MCLONE_CHECK_GL_ERROR graphics::checkGLError();
#else
#define MCLONE_CHECK_GL_ERROR (void)0;
#endif