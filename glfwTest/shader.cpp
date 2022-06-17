#include "shader.h"
#include <fstream>
#include <stdexcept>
#include <iostream>


Shader::Shader(const char* file_name, GLenum type)
{
	std::ifstream file(file_name);

	if (file.is_open())
	{
		this->objHandle = glCreateShader(type);

		// Copies all characters in the file until the next character is at the
		// end of the stream.
		std::string file_content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		
		const char* cStrContents = file_content.c_str();

		glShaderSource(objHandle, 1, &cStrContents, nullptr);
		glCompileShader(objHandle);

		// Check to see if the compilation was sucessful.
		GLint status;
		glGetShaderiv(objHandle, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetShaderiv(objHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(objHandle, infoLogLength, NULL, strInfoLog);


			std::string message = "Could not compile  ";
			switch (type)
			{
			case GL_VERTEX_SHADER: message.append("vertex: ");
				break;
			case GL_GEOMETRY_SHADER: message.append("geometry: ");
				break;
			case GL_FRAGMENT_SHADER: message.append("fragment: ");
				break;
			}

			message.append(file_name);
			std::cerr << strInfoLog << std::endl;
			delete[] strInfoLog;
			throw std::runtime_error(message);
		}
	}
	else
	{
		// Throw an exception that the file could not be opened.
		std::string message = "Could not open file: ";
		message.append(file_name);
		throw std::runtime_error(message);
	}
}

Shader::~Shader()
{
	glDeleteShader(this->objHandle);
}


VertexShader::VertexShader(const char* file_name) 
	: Shader(file_name, GL_VERTEX_SHADER)
{

}

FragmentShader::FragmentShader(const char* file_name)
	: Shader(file_name, GL_FRAGMENT_SHADER)
{

}

ShaderProgram::ShaderProgram()
{
	m_program = 0;
}

GLuint ShaderProgram::program()
{
	return m_program;
}


void ShaderProgram::use()
{
	glUseProgram(m_program);
}

bool ShaderProgram::Link()
{
	// Don't need to recreate the program if already linked.
	if (m_linked)
	{
		return true;
	}
	
	m_program = glCreateProgram();
	for (auto& element : m_shaderList) {
		glAttachShader(m_program, element->objHandle);
	}
	
	glLinkProgram(m_program);
	
	GLint isLinked = 0;
	glGetProgramiv(m_program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* logInfo = new char[maxLength + 1];
		glGetProgramInfoLog(m_program, maxLength, &maxLength, logInfo);

		// We don't need the program anymore.
		glDeleteProgram(m_program);

		// Use the infoLog as you see fit.
		std::cout << (char*)logInfo << std::endl;

		// In this simple program, we'll just leave
		return false;
	}
	
	for (auto& element : m_shaderList) {
		glDeleteShader(element->objHandle);
	}

	m_linked = true;
	return true;
}


bool ShaderProgram::addShader(const char* fileName, enum SHADER_TYPE type)
{
	switch(type)
	{
	case VERTEX_SHADER:
		m_shaderList.push_back(new VertexShader(fileName));
		break;
	case FRAGMENT_SHADER:
		m_shaderList.push_back(new FragmentShader(fileName));
		break;
	}

	return true;
}
