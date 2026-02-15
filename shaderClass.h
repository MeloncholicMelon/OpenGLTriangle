#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// setting id
	unsigned int ID;

	// Constructor takes path to shader files
	Shader(const char* vertexShader, const char* fragmentShader)
	{

		// retrieving Vertex/fragment source code
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		// check ifstream objects can throw exceptions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		//file reading
		try
		{
			// open files
			vShaderFile.open(vertexShader);
			fShaderFile.open(fragmentShader);
			std::stringstream vShaderStream, fShaderStream;

			// read file buffer content into stream
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			// close file handler
			vShaderFile.close();
			fShaderFile.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR : Shader : unsuccessful file read" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// compile shader
		unsigned int vertex, fragment;
		int success;
		char infolog[512];

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infolog);
			std::cout << "ERROR : shader : vertex : compilation failed\n" << infolog << std::endl;
		};

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);


		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infolog);
			std::cout << "ERROR : shader : fragment : compilation failed\n" << infolog << std::endl;
		};

		// shader program

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 512, NULL, infolog);
			std::cout << "ERROR : Program : Link failed\n" << infolog << std::endl;
		};
		
		glDeleteShader(fragment);
		glDeleteShader(vertex);
	}

	void use()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
};
#endif