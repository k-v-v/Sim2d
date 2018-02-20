//
// Created by konstantin on 21/06/17.
//

#include "Shader.h"

#include <iostream>

ShaderException::ShaderException(char const * const message) throw () :
		runtime_error(message) {

}

UniformProxy::UniformProxy(GLuint programId, GLint uniformLocation) :
		programId(programId), uniformLocation(uniformLocation) {
}

GLuint Shader::currentProgramID = 0;

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {

	std::string vertexShaderCode;
	std::string fragmentShaderCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	//Ensure ifstream objects can throw exceptions
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(
			std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexShaderFile.open(std::string("shaders/") + vertexShaderPath);
		std::stringstream vertexStream;
		vertexStream << vertexShaderFile.rdbuf();
		vertexShaderCode = vertexStream.str();
	} catch (std::ifstream::failure exception) {
		throw ShaderException(
				(std::string("Couldn't open vertex shader file: ")
						+ vertexShaderPath).c_str());
	}

	try {
		fragmentShaderFile.open(std::string("shaders/") + fragmentShaderPath);
		std::stringstream fragmentStream;
		fragmentStream << fragmentShaderFile.rdbuf();
		fragmentShaderCode = fragmentStream.str();
	} catch (std::ifstream::failure exception) {
		throw ShaderException(
				(std::string("Couldn't open fragment shader file: ")
						+ fragmentShaderPath).c_str());
	}

	const GLchar * vertexSource = vertexShaderCode.c_str();
	const GLchar * fragmentSource = fragmentShaderCode.c_str();

	GLuint vertexShader;
	GLuint fragmentShader;
	int success;
	char infoLog[2048];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, 2048, NULL, infoLog);
		glDeleteShader(vertexShader);
		throw ShaderException(
				(std::string("Failed to compile :") + vertexShaderPath
						+ std::string(" with  error :") + std::string(infoLog)).c_str());
	}

	fragmentShader = glCreateShader( GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fragmentShader, 2048, NULL, infoLog);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		throw ShaderException(
				(std::string("Failed to compile :") + fragmentShaderPath
						+ std::string(" with  error :") + std::string(infoLog)).c_str());
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	glUseProgram(programID);

	// Check if it successfully linked
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		glGetProgramInfoLog(programID, 2048, NULL, infoLog);
		throw ShaderException(
				(std::string("Failed to link program : ") + fragmentShaderPath
						+ std::string(" with  error : ") + std::string(infoLog)).c_str());
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint uniformCount;

	const GLsizei buffSize = 64;
	GLint varSize;
	GLchar varName[buffSize];
	GLsizei varLength;
	GLenum varType;

	GLint uniformLocation;

	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &uniformCount);
	for (int i = 0; i < uniformCount; i++) {
		glGetActiveUniform(programID, i, buffSize, &varLength, &varSize,
				&varType, varName);
		uniformLocation = glGetUniformLocation(programID, varName);
		std::cout << "Uniform: " << varName << " location: " << uniformLocation
				<< "\n";
		ActiveUniforms.insert( { std::string(varName), uniformLocation });
	}

}

GLuint Shader::getID() {
	return programID;
}

void Shader::use() {
	//std::cout<<programID;
	currentProgramID = programID;
	glUseProgram(programID);
}
GLuint Shader::getUnirformLocation(std::string name){
	std::unordered_map<std::string, GLuint>::iterator it = ActiveUniforms.find(name);
	//if the uniform is present in the shader return its id
	if (it != ActiveUniforms.end())
		return it->second;
	else
		return 0;
}
Shader::~Shader() {
	glDeleteProgram(programID);
}

template<>
void UniformProxy::operator=(const float & value) {
	glUniform1f(uniformLocation, value);
}
template<>
void UniformProxy::operator=(const int & value) {
	glUniform1i(uniformLocation, value);
}
template<>
void UniformProxy::operator=(const bool & value) {
	glUniform1i(uniformLocation, value);
}
template<>
void UniformProxy::operator=(const glm::mat4 & value) {
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}
