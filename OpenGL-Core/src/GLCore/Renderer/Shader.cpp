#include "Renderer/Shader.h"
#include "Util/OpenGLDebug.h"
#include "Core/Timer.h"
#include "Core/MyAssert.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace GLCore::Utils
{

	static GLenum ShaderTypeFromString(const std::string &type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		CORE_ASSERT(false, "UNKNOWN shader type");

		return 0;
	}

	static const char *GLShaderStageToString(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:
			return "GL_VERTEX_SHADER";
		case GL_FRAGMENT_SHADER:
			return "GL_FRAGMENT_SHADER";
		}

		return nullptr;
	}

	static const char *GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:
			return ".cached_opengl.vert";
		case GL_FRAGMENT_SHADER:
			return ".cached_opengl.frag";
		}
		return "";
	}

	OpenGLShader::OpenGLShader(const std::string &filepath)
		: m_FilePath(filepath)
	{
		std::string source = ReadFile(filepath);
		m_OpenGLSourceCode = PreProcess(source);

		{
			//Timer timer;
			CreateProgram();
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
		: m_Name(name)
	{

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		CreateProgram();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::

		ReadFile(const std::string &filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				ASSERT_CUSTOM(false, "failed to read shader file!");
			}
		}
		else
		{
			ASSERT_CUSTOM(false, " shader file don't exist");
		}

		return result;
	}

	 	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source)
		{

			std::unordered_map<GLenum, std::string> shaderSources;

			const char *typeToken = "#type";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = source.find(typeToken, 0); // Start of shader type declaration line
			while (pos != std::string::npos)
			{
				size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
				CORE_ASSERT(eol != std::string::npos, "Syntax error");
				size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
				std::string type = source.substr(begin, eol - begin);
				CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

				size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
				CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
				pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

				shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
			}

			return shaderSources;
		}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto &&[stage, source] : m_OpenGLSourceCode)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			const GLchar *sourceCStr = source.c_str();
			GLCall(glShaderSource(shaderID, 1, &sourceCStr, 0));
			GLCall(glCompileShader(shaderID));
			GLint isCompiled = 0;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shaderID);

				LOG_ERROR("{0}", infoLog.data());
				CORE_ASSERT(false, "Shader compilation failure!");
			}

			GLCall(glAttachShader(program, shaderID));
		}

		GLCall(glLinkProgram(program));

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			LOG_ERROR("{0}", infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Bind() const
	{
		GLCall(glUseProgram(m_RendererID));
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string &name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string &name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat2(const std::string &name, const glm::vec2 &value)
	{
		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value)
	{

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string &name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count)
	{
		GLCall(GLint location = glGetUniformLocation(m_RendererID, name.c_str()));
		GLCall(glUniform1iv(location, count, values));
	}

	void OpenGLShader::UploadUniformFloat(const std::string &name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::mat3 &matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
