#pragma once

#include <GL/glew.h>

#include "Core/Log.h"
#include <string>

namespace GLCore::Utils
{

	bool GlCheckError(const std::string &func, const std::string &file, const int line);

	void GlClearError();

#define ASSERT_LOCAL(x) \
	if (!(x))     \
		__builtin_debugtrap();

#define GLCall(x)   \
	GLCore::Utils::GlClearError(); \
	x;              \
	ASSERT_LOCAL(GLCore::Utils::GlCheckError(#x, __FILE__, __LINE__))

	enum class DebugLogLevel
	{
		None = 0,
		HighAssert = 1,
		High = 2,
		Medium = 3,
		Low = 4,
		Notification = 5
	};

	void EnableGLDebugging();
	void SetGLDebugLogLevel(DebugLogLevel level);
	void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

}