#include "Renderer/Shader.h"
#include "Util/OpenGLDebug.h"
#include "Core/Timer.h"
#include "Core/MyAssert.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

//#include <shaderc/shaderc.hpp>
//#include <spirv_cross/spirv_cross.hpp>
//#include <spirv_cross/spirv_glsl.hpp>

namespace GLCore::Utils
{

	static std::string ReadFileAsString(const std::string &filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize((size_t)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			LOG_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	GLuint Shader::CompileShader(GLenum type, const std::string &source)
	{
		GLuint shader = glCreateShader(type);

		const GLchar *sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			// LOG_ERROR("{0}", infoLog.data());
			CORE_ASSERT(false, "Shader compilation failure!");
		}

		return shader;
	}

	Shader *Shader::FromGLSLTextFiles(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
	{
		Shader *shader = new Shader();
		shader->LoadFromGLSLTextFiles(vertexShaderPath, fragmentShaderPath);
		return shader;
	}

	void Shader::LoadFromGLSLTextFiles(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
	{
		std::string vertexSource = ReadFileAsString(vertexShaderPath);
		std::string fragmentSource = ReadFileAsString(fragmentShaderPath);

		GLuint program = glCreateProgram();
		int glShaderIDIndex = 0;

		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
		glAttachShader(program, vertexShader);
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			LOG_ERROR("{0}", infoLog.data());
			// HZ_CORE_ASSERT(false, "Shader link failure!");
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		m_RendererID = program;
	}

	static GLenum ShaderTypeFromString(const std::string &type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		CORE_ASSERT(false, "UNKNOWN shader type");

		return 0;
	}

/* 	static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:
			return shaderc_glsl_vertex_shader;
		case GL_FRAGMENT_SHADER:
			return shaderc_glsl_fragment_shader;
		}
		return (shaderc_shader_kind)0;
	} */

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

	static const char *GetCacheDirectory()
	{
		// TODO: make sure the assets directory is valid
		return "assets/cache/shader/opengl";
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		std::string cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);
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

	static const char *GLShaderStageCachedVulkanFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:
			return ".cached_vulkan.vert";
		case GL_FRAGMENT_SHADER:
			return ".cached_vulkan.frag";
		}
		return "";
	}

	OpenGLShader::OpenGLShader(const std::string &filepath)
		: m_FilePath(filepath)
	{
		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		m_OpenGLSourceCode = PreProcess(source);

		{
			Timer timer;
			// CompileOrGetVulkanBinaries(shaderSources);
			// CompileOrGetOpenGLBinaries(shaderSources);
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

		// CompileOrGetVulkanBinaries(sources);
		//CompileOrGetOpenGLBinaries(sources);
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
				// HZ_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			ASSERT_CUSTOM(false, " shader file don't exist");
			// HZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	 	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source)
		{
			//HZ_PROFILE_FUNCTION();

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

		/*void OpenGLShader:: CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string> &shaderSources)
		{
			GLuint program = glCreateProgram();

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
			const bool optimize = true;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

			auto &shaderData = m_VulkanSPIRV;
			shaderData.clear();
			for (auto &&[stage, source] : shaderSources)
			{
				std::filesystem::path shaderFilePath = m_FilePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto &data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char *)data.data(), size);
				}
				else
				{
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						//CORE_ERROR(module.GetErrorMessage());
						CORE_ASSERT(false, module.GetErrorMessage());
					}

					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto &data = shaderData[stage];
						out.write((char *)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}
			}

			for (auto &&[stage, data] : shaderData)
				Reflect(stage, data);
		} */

	/* 	void OpenGLShader::CompileOrGetOpenGLBinaries(const std::unordered_map<GLenum, std::string> &shaderSources)
		{
			auto &shaderData = m_OpenGLSPIRV;

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_opengl, 410);
			const bool optimize = false;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

			//auto &shaderData = m_VulkanSPIRV;
			//shaderData.clear();
			m_OpenGLSourceCode.clear();
			//for (auto &&[stage, spirv] : m_VulkanSPIRV)
			for (auto &&[stage, source] : shaderSources)
			{
				std::filesystem::path shaderFilePath = m_FilePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto &data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char *)data.data(), size);
				}
				else
				{
					auto module = compiler.PreprocessGlsl(source.c_str(),
						source.size(),
						Utils::GLShaderStageToShaderC(stage),
						 m_FilePath.c_str(),
						 options);

					//spirv_cross::CompilerGLSL glslCompiler();
					//glslCompiler.


					//shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						//HZ_CORE_ERROR(module.GetErrorMessage());
						CORE_ASSERT(false, module.GetErrorMessage());
					}

					m_OpenGLSourceCode[stage] = {module.begin(),module.end()};
					auto &source = m_OpenGLSourceCode[stage];

					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto &data = shaderData[stage];
						out.write((char *)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}
			}
		}  */

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto &&[stage, source] : m_OpenGLSourceCode)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			const GLchar *sourceCStr = source.c_str();
			glShaderSource(shaderID, 1, &sourceCStr, 0);
			glCompileShader(shaderID);
			GLint isCompiled = 0;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shaderID);

				// LOG_ERROR("{0}", infoLog.data());
				CORE_ASSERT(false, "Shader compilation failure!");
			}

			// glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			// glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			// glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

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

	/*  	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t> &shaderData)
		{
			spirv_cross::Compiler compiler(shaderData);
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			for (const auto &resource : resources.uniform_buffers)
			{
				const auto &bufferType = compiler.get_type(resource.base_type_id);
				uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				int memberCount = bufferType.member_types.size();
			}
		}  */

	void OpenGLShader::Bind() const
	{
		// HZ_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		// HZ_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string &name, int value)
	{
		// HZ_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string &name, float value)
	{
		// HZ_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat2(const std::string &name, const glm::vec2 &value)
	{
		// HZ_PROFILE_FUNCTION();

		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value)
	{
		// HZ_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value)
	{
		// HZ_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value)
	{
		// HZ_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string &name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
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
