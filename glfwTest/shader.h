#pragma once

#include <glew.h>
#include <iostream>
#include <vector>

class Shader
{	
public:
	Shader(const char* file_name, GLenum type);
	//virtual int temp() = 0;
	GLuint objHandle;
	~Shader();

protected:
};

class VertexShader : public Shader
{
public:
	VertexShader(const char* file_name);
	//virtual int temp() { return 3; };
};

class FragmentShader : public Shader
{
public:
	FragmentShader(const char* file_name);
	//virtual int temp() { return 3; };
};

class ShaderProgram
{
public:
	ShaderProgram();
	void use();
	bool addShader(const char* fileName, enum SHADER_TYPE type);
	bool Link();
	GLuint program();

private:
	std::vector<Shader*> m_shaderList;
	bool m_linked = false;
	GLuint m_program;
};

enum SHADER_TYPE
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
};