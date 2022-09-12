#pragma once
#include <GL/glew.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace GLCore::Utils
{

	class OpenGLShader
	{
	public:
		OpenGLShader(const std::string &filepath);
		OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const;
		void Unbind() const;
		GLuint GetRendererID() const {return m_RendererID;}

		void SetInt(const std::string &name, int value);
		void SetIntArray(const std::string &name, int *values, uint32_t count);
		void SetFloat(const std::string &name, float value);
		void SetFloat2(const std::string &name, const glm::vec2 &value);
		void SetFloat3(const std::string &name, const glm::vec3 &value);
		void SetFloat4(const std::string &name, const glm::vec4 &value);
		void SetMat4(const std::string &name, const glm::mat4 &value);

		const std::string &GetName() const { return m_Name; }

		void UploadUniformInt(const std::string &name, int value);
		void UploadUniformIntArray(const std::string &name, int *values, uint32_t count);

		void UploadUniformFloat(const std::string &name, float value);
		void UploadUniformFloat2(const std::string &name, const glm::vec2 &value);
		void UploadUniformFloat3(const std::string &name, const glm::vec3 &value);
		void UploadUniformFloat4(const std::string &name, const glm::vec4 &value);

		void UploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
		void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

	private:
		std::string ReadFile(const std::string &filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string &source);

		void CreateProgram();

	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};

}